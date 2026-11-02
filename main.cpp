#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/uart.h"
#include "hardware/spi.h"

#define UART_ID uart0
#define BAUD_RATE 9600
#define UART_TX_PIN 0
#define UART_RX_PIN 1

// SD Card SPI Pins (SPI 1)
#define SD_SPI spi1
#define SD_SCK_PIN 6
#define SD_MOSI_PIN 7
#define SD_MISO_PIN 4
#define SD_CS_PIN 5

// PCM5102 I2S Pins (PIO-based)
#define I2S_BCK_PIN 26
#define I2S_LRCK_PIN 27
#define I2S_DIN_PIN 28

// Buffer for incoming UART commands
char uart_buffer[128];
int uart_index = 0;

void init_hardware() {
    // 1. Initialize standard stdio
    stdio_init_all();

    // 2. Initialize UART for Inter-Board Communication
    uart_init(UART_ID, BAUD_RATE);
    gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART);

    // 3. Initialize SPI 1 for Adafruit MicroSD Breakout
    spi_init(SD_SPI, 10000000); // 10 MHz
    gpio_set_function(SD_SCK_PIN, GPIO_FUNC_SPI);
    gpio_set_function(SD_MOSI_PIN, GPIO_FUNC_SPI);
    gpio_set_function(SD_MISO_PIN, GPIO_FUNC_SPI);
    
    gpio_init(SD_CS_PIN);
    gpio_set_dir(SD_CS_PIN, GPIO_OUT);
    gpio_put(SD_CS_PIN, 1); // Deselect card

    printf("Audio Pico C++ Firmware Initialized.\n");
}

void process_play_command(const char* filename) {
    printf("Received request to play: %s\n", filename);
    // TODO: Add FatFs file reading and MP3 decoding logic here
}

void poll_uart_commands() {
    while (uart_is_readable(UART_ID)) {
        char c = uart_getc(UART_ID);
        if (c == '\n' || c == '\r') {
            uart_buffer[uart_index] = '\0';
            if (strncmp(uart_buffer, "PLAY:", 5) == 0) {
                process_play_command(&uart_buffer[5]);
            }
            uart_index = 0; // Reset buffer
        } else if (uart_index < sizeof(uart_buffer) - 1) {
            uart_buffer[uart_index++] = c;
        }
    }
}

int main() {
    init_hardware();

    // Main runtime loop
    while (true) {
        poll_uart_commands();
        tight_loop_contents();
    }

    return 0;
}