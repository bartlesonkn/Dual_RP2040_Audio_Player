cmake_minimum_required(VERSION 3.13)

# Pull in Raspberry Pi Pico SDK (ensure PICO_SDK_PATH is set in your environment)
include(pico_sdk_import.cmake)

project(pico_audio_player C CXX ASM)

# Initialize the Pico SDK
pico_sdk_init()

# Define the executable target and include all necessary source files
add_executable(pico_audio_player 
    main.cpp  
    diskio.c  
    ff.c  
)

# Link required libraries for hardware peripherals, FatFs, and I2S audio
target_link_libraries(pico_audio_player 
    pico_stdlib   
    hardware_spi   
    hardware_uart   
    hardware_pio   
    hardware_dma   
    pico_audio_i2s  
    fatfs  
)

# Enable USB standard output for serial logging/debugging and disable UART stdio
pico_enable_stdio_usb(pico_audio_player 1)
pico_enable_stdio_uart(pico_audio_player 0)

# Generate map, bin, hex, and uf2 extra output files
pico_add_extra_outputs(pico_audio_player)