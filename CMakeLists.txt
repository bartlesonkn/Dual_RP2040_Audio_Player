cmake_minimum_required(VERSION 3.13)

# Pull in Raspberry Pi Pico SDK and Pico Extras
include(pico_sdk_import.cmake)
include(pico_extras_import.cmake)

project(pico_audio_player C CXX ASM)

# Initialize the Pico SDK
pico_sdk_init()

# Add Carl Kugler's library source subdirectory
add_subdirectory(no-OS-FatFS-SD-SDIO-SPI-RPi-Pico/src)

# Define executable target and source files
add_executable(pico_audio_player 
    main.cpp  
    my_hw_config.c
)

# Put the library's include directory FIRST so ff.c finds the exact correct ffconf.h
target_include_directories(pico_audio_player PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/no-OS-FatFS-SD-SDIO-SPI-RPi-Pico/src/include
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/no-OS-FatFS-SD-SDIO-SPI-RPi-Pico/src/sd_driver
    ${CMAKE_CURRENT_SOURCE_DIR}/no-OS-FatFS-SD-SDIO-SPI-RPi-Pico/src/ff15/source
)

# Link required hardware libraries and the library target
target_link_libraries(pico_audio_player 
    pico_stdlib    
    hardware_spi    
    hardware_uart    
    hardware_pio    
    hardware_dma    
    pico_audio_i2s  
    no-OS-FatFS-SD-SDIO-SPI-RPi-Pico  
)

# Enable USB standard output and disable UART stdio
pico_enable_stdio_usb(pico_audio_player 1)
pico_enable_stdio_uart(pico_audio_player 0)

# Generate map, bin, hex, and uf2 extra output files
pico_add_extra_outputs(pico_audio_player)