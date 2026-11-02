# This is the standard import file for the Raspberry Pi Pico SDK
if (NOT DEFINED ENV{PICO_SDK_PATH} AND NOT PICO_SDK_PATH)
    message(FATAL_ERROR "Pico SDK not found. Set PICO_SDK_PATH.")
endif()
set(PICO_SDK_PATH ${PICO_SDK_PATH} CACHE PATH "Path to the Pico SDK")
include(${PICO_SDK_PATH}/external/pico_sdk_import.cmake)