cmake_minimum_required(VERSION 3.13)

# Pull in Raspberry Pi Pico SDK (requires PICO_SDK_PATH environment variable)
include(pico_sdk_import.cmake)

project(pico_audio_player C CXX ASM)

# Initialize the Pico SDK
pico_sdk_init()

# Add Carl Kugler's FatFs SPI library submodule
add_subdirectory(no-OS-FatFS-SD-SPI-RPi-Pico/FatFs_SPI build)

# Define the executable target (only main.cpp is needed since the submodule handles diskio/ff)
add_executable(pico_audio_player 
    main.cpp  
)

# Link required libraries for hardware peripherals, I2S audio, and FatFs SPI submodule
target_link_libraries(pico_audio_player 
    pico_stdlib   
    hardware_spi   
    hardware_uart   
    hardware_pio   
    hardware_dma   
    pico_audio_i2s  
    FatFs_SPI  
)

# Enable USB standard output for serial logging/debugging and disable UART stdio
pico_enable_stdio_usb(pico_audio_player 1)
pico_enable_stdio_uart(pico_audio_player 0)

# Generate map, bin, hex, and uf2 extra output files
pico_add_extra_outputs(pico_audio_player)