#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "hardware/uart.h"
#include "hardware/spi.h"
#include "hardware/watchdog.h"
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

    audio_pool = init_i2s_audio();
    printf("[LOG] Hardware initialization sequence completed.\n");
}

void stream_mp3_file(const char* filename) {
    FIL file;
    FRESULT fr = f_open(&file, filename, FA_READ);
    if (fr != FR_OK) {
        printf("[LOG ERROR] Could not open file '%s'. FatFs error: %d\n", filename, fr);
        printf("  - Error 3  (FR_NOT_READY): SD card hardware not responding (check SPI wiring/power).\n");
        printf("  - Error 4  (FR_NO_FILE): File not found on the SD card.\n");
        return;
    }

    printf("[LOG] Starting playback stream for: %s\n", filename);
    
    mp3dec_t mp3;
    mp3dec_init(&mp3);

    #define READ_BUF_SIZE 4096
    uint8_t *read_buf = (uint8_t*)malloc(READ_BUF_SIZE + MINIMP3_MAX_SAMPLES_PER_FRAME * 2);
    if (!read_buf) {
        printf("[LOG ERROR] Failed to allocate read buffer memory!\n");
        f_close(&file);
        return;
    }

    UINT bytes_read = 0;
    f_read(&file, read_buf, READ_BUF_SIZE, &bytes_read);
    int bytes_left = bytes_read;
    uint8_t *read_ptr = read_buf;

    is_playing = true;

    while (is_playing) {
        // Feed the watchdog during long audio loops so playback doesn't trigger a timeout reset
        watchdog_update();

        if (is_paused) {
            sleep_ms(50);
            continue;
        }

        // Refill buffer from SD card when running low
        if (bytes_left < 1024 && !f_eof(&file)) {
            memmove(read_buf, read_ptr, bytes_left);
            UINT br = 0;
            f_read(&file, read_buf + bytes_left, READ_BUF_SIZE - bytes_left, &br);
            bytes_left += br;
            read_ptr = read_buf;
        }

        if (bytes_left <= 0) {
            break; // Reached end of file
        }

        mp3dec_frame_info_t info;
        int16_t pcm[MINIMP3_MAX_SAMPLES_PER_FRAME * 2];
        
        int samples_decoded = mp3dec_decode_frame(&mp3, read_ptr, bytes_left, pcm, &info);

        if (samples_decoded > 0) {
            struct audio_buffer *buffer = take_audio_buffer(audio_pool, true);
            int16_t *buffer_s16 = (int16_t*)buffer->buffer->bytes;
            
            int samples_to_copy = samples_decoded * info.channels;
            for (int i = 0; i < samples_to_copy; i++) {
                buffer_s16[i] = pcm[i];
            }
            buffer->sample_count = samples_decoded;
            
            give_audio_buffer(audio_pool, buffer);

            int bytes_consumed = info.frame_bytes;
            if (bytes_consumed <= 0) break;
            read_ptr += bytes_consumed;
            bytes_left -= bytes_consumed;
        } else {
            // Resync by advancing 1 byte if frame decode fails
            read_ptr++;
            bytes_left--;
        }
    }

    free(read_buf);
    f_close(&file);
    printf("[LOG] Playback finished for: %s\n", filename);
    is_playing = false;
}

void process_control_command(const char* cmd) {
    printf("[LOG UART RX] Received command string: %s\n", cmd);
    
    if (strncmp(cmd, "PLAY:", 5) == 0) {
        const char* filename = &cmd[5];
        stream_mp3_file(filename);
    } 
    else if (strcmp(cmd, "PAUSE") == 0) {
        is_paused = !is_paused;
        audio_i2s_set_enabled(!is_paused);
        printf("State: Paused = %d\n", is_paused);
    } 
    else if (strcmp(cmd, "STOP") == 0 || strcmp(cmd, "MUTE") == 0) {
        is_playing = false;
        is_paused = true;
        audio_i2s_set_enabled(false);
        printf("Audio Muted / Stopped.\n");
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
    
    // Enable hardware watchdog (2-second timeout protection against locks)
    watchdog_enable(2000, 1);
    printf("[LOG] Hardware watchdog enabled (2s timeout active).\n");

    FATFS fs;
    printf("[LOG] Registering FatFs logical drive (lazy mount mode opt=0)...\n");
    // opt = 0 defers hardware handshake until f_open(), ensuring clean boots without locking up
    FRESULT fr = f_mount(&fs, "0:", 0);
    if (fr != FR_OK) {
        printf("[LOG ERROR] f_mount registration failed with code: %d\n", fr);
    } else {
        printf("[LOG] FatFs logical drive registered successfully on drive 0:.\n");
    }

    printf("[LOG] Entering main application loop. Send commands via UART (e.g., PLAY:song.mp3)\n");
    while (true) {
        // Feed the watchdog continuously to confirm normal execution
        watchdog_update();
        
        poll_uart_commands();
        tight_loop_contents();
    }
    return 0;
}