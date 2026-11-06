#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/uart.h"
#include "hardware/spi.h"
#include "pico/audio_i2s.h"
#include "ff.h"

#define MINIMP3_IMPLEMENTATION
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wstringop-overflow"
#include "minimp3.h"
#pragma GCC diagnostic pop

#define UART_ID uart0
#define BAUD_RATE 115200
#define UART_TX_PIN 0
#define UART_RX_PIN 1

#define I2S_DIN_PIN 26
#define I2S_BCK_PIN 27
#define I2S_LRCK_PIN 28
#define SAMPLE_RATE 44100

char uart_buffer[128];
int uart_index = 0;
struct audio_buffer_pool* audio_pool = NULL;
volatile bool is_playing = false;
volatile bool is_paused = false;

struct audio_buffer_pool* init_i2s_audio() {
    printf("[LOG] Initializing I2S audio format configuration...\n");
    audio_format_t audio_format = {
        .sample_freq = SAMPLE_RATE,
        .format = AUDIO_BUFFER_FORMAT_PCM_S16,
        .channel_count = 2
    };

    struct audio_buffer_format producer_format = {
        .format = &audio_format,
        .sample_stride = 4 
    };

    struct audio_buffer_pool *producer_pool = audio_new_producer_pool(&producer_format, 4, 512);
    if (!producer_pool) {
        printf("[LOG ERROR] Failed to create audio producer pool!\n");
        return NULL;
    }

    audio_i2s_config_t config = {
        .data_pin = I2S_DIN_PIN,
        .clock_pin_base = I2S_BCK_PIN,
        .dma_channel = 0,
        .pio_sm = 0
    };

    printf("[LOG] Setting up I2S PIO state machine on pins DIN:%d, BCK:%d, LRCK:%d\n", I2S_DIN_PIN, I2S_BCK_PIN, I2S_LRCK_PIN);
    const struct audio_format *output_format = audio_i2s_setup(&audio_format, &config);
    if (!output_format) {
        printf("[LOG ERROR] Pico Audio I2S initialization failed!\n");
        return NULL;
    }

    audio_i2s_connect(producer_pool);
    audio_i2s_set_enabled(true);
    printf("[LOG] I2S Audio successfully initialized and enabled.\n");
    return producer_pool;
}

void init_hardware() {
    stdio_init_all();
    for (int i = 5; i > 0; i--) {
        printf("[LOG] Booting in %d seconds...\n", i);
        sleep_ms(1000);
    }

    printf("[LOG] Initializing UART0 (TX:%d, RX:%d at %d baud)...\n", UART_TX_PIN, UART_RX_PIN, BAUD_RATE);
    uart_init(UART_ID, BAUD_RATE);
    gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART);

    // NOTE: Manual SD card SPI initialization has been removed. 
    // Carl Kugler's FatFs_SPI library handles SPI1 and pin multiplexing via my_hw_config.c.

    audio_pool = init_i2s_audio();
    printf("[LOG] Hardware initialization sequence completed.\n");
}

void process_control_command(const char* cmd) {
    printf("[LOG UART RX] Received command string: %s\n", cmd);
    if (strncmp(cmd, "PLAY:", 5) == 0) {
        printf("Executing Play: %s\n", &cmd[5]);
        is_playing = true;
        is_paused = false;
    } else if (strcmp(cmd, "PAUSE") == 0) {
        is_paused = !is_paused;
        audio_i2s_set_enabled(!is_paused);
        printf("State: Paused = %d\n", is_paused);
    } else if (strcmp(cmd, "STOP") == 0 || strcmp(cmd, "MUTE") == 0) {
        is_paused = true;
        audio_i2s_set_enabled(false);
        printf("Audio Muted / Stopped.\n");
    } else if (strncmp(cmd, "SKIP", 4) == 0) {
        printf("Skipping to next track...\n");
    } else if (strncmp(cmd, "BACK", 4) == 0) {
        printf("Returning to previous track...\n");
    }
}

void poll_uart_commands() {
    while (uart_is_readable(UART_ID)) {
        char c = uart_getc(UART_ID);
        if (c == '\n' || c == '\r') {
            uart_buffer[uart_index] = '\0';
            if (uart_index > 0) {
                process_control_command(uart_buffer);
            }
            uart_index = 0;
        } else if (uart_index < sizeof(uart_buffer) - 1) {
            uart_buffer[uart_index++] = c;
        }
    }
}

int main() {
    init_hardware();
    
    FATFS fs;
    printf("[LOG] Attempting FatFs f_mount()...\n");
    // Pass logical drive "0" to match .pcName = "0" in my_hw_config.c
    FRESULT fr = f_mount(&fs, "0", 1);
    if (fr != FR_OK) {
        printf("[LOG ERROR] f_mount failed with error code: %d. Check SD card wiring or formatting.\n", fr);
    } else {
        printf("[LOG] FatFs mounted successfully!\n");
    }

    printf("[LOG] Entering main application loop.\n");
    while (true) {
        poll_uart_commands();
        tight_loop_contents();
    }
    return 0;
}