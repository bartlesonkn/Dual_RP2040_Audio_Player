cmake_minimum_required(VERSION 3.13)

# Include the Pico SDK import script
include(pico_sdk_import.cmake)

project(pico_audio_player C CXX ASM)

# Initialize the Pico SDK
pico_sdk_init()

# Add Carl Kugler's FatFs library subdirectory
# (Ensure 'no-OS-FatFS-SD-SPI-RPi-Pico' matches your exact folder name in the project root)
add_subdirectory(no-OS-FatFS-SD-SPI-RPi-Pico/FatFs_SPI build)

# Define your main executable and source files
add_executable(pico_audio_player 
    main.cpp  
)

# Link all required Pico SDK hardware modules, audio libraries, and FatFs SPI target
target_link_libraries(pico_audio_player 
    pico_stdlib 
    hardware_spi 
    hardware_uart 
    hardware_pio 
    hardware_dma 
    pico_audio_i2s
    FatFs_SPI
)

# Enable USB output for standard print statements (stdio) and disable UART for stdio
pico_enable_stdio_usb(pico_audio_player 1)
pico_enable_stdio_uart(pico_audio_player 0)

# Generate extra output formats (.uf2, .bin, .hex)
pico_add_extra_outputs(pico_audio_player)