#include "my_hw_config.h"

static spi_t spis[] = {
    {
        .hw_inst = spi1,
        .sck_gpio = 6,
        .mosi_gpio = 7,
        .miso_gpio = 4,
        .baud_rate = 500 * 1000  // Adjusted to 500 kHz for breakout level-shifter compatibility
    }
};

static sd_card_t sd_cards[] = {
    {
        .pcName = "0",
        .spi = &spis[0],
        .ss_gpio = 5
    }
};

size_t sd_get_num() { return count_of(sd_cards); }
sd_card_t *sd_get_by_num(size_t num) { return &sd_cards[num]; }
size_t spi_get_num() { return count_of(spis); }
spi_t *spi_get_by_num(size_t num) { return &spis[num]; }