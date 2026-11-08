#include "my_hw_config.h"
#include "hw_config.h"

// Define the SPI controller configuration (SPI0 on pins 4, 6, 7)
static spi_t spis[] = {
    {
        .hw_inst = spi0,
        .sck_gpio = 6,
        .mosi_gpio = 7,
        .miso_gpio = 4,
        .baud_rate = 500 * 1000  // 500 kHz initial handshake speed
    }
};

// Define the SPI interface mapping and Chip Select pin (CS: 5)
static sd_spi_if_t spi_ifs[] = {
    {
        .spi = &spis[0],
        .ss_gpio = 5             
    }
};

// Define the SD card socket configuration (drive name "0:" is assigned by index)
static sd_card_t sd_cards[] = {
    {
        .type = SD_IF_SPI,
        .spi_if_p = &spi_ifs[0]
    }
};

// Required accessor functions for Carl Kugler's library
size_t sd_get_num() { return count_of(sd_cards); }
sd_card_t *sd_get_by_num(size_t num) { return &sd_cards[num]; }
size_t spi_get_num() { return count_of(spis); }
spi_t *spi_get_by_num(size_t num) { return &spis[num]; }