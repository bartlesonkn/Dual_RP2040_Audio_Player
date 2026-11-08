#pragma once

#include "hw_config.h"

#ifdef __cplusplus
extern "C" {
#endif

// Accessor function prototypes required by the library
size_t sd_get_num();
sd_card_t *sd_get_by_num(size_t num);
size_t spi_get_num();
spi_t *spi_get_by_num(size_t num);

#ifdef __cplusplus
}
#endif