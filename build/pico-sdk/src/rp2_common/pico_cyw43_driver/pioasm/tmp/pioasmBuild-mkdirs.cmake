# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/home/codespace/pico-sdk/tools/pioasm"
  "/workspaces/Dual_RP2040_Audio_Player/build/pioasm"
  "/workspaces/Dual_RP2040_Audio_Player/build/pioasm-install"
  "/workspaces/Dual_RP2040_Audio_Player/build/pico-sdk/src/rp2_common/pico_cyw43_driver/pioasm/tmp"
  "/workspaces/Dual_RP2040_Audio_Player/build/pico-sdk/src/rp2_common/pico_cyw43_driver/pioasm/src/pioasmBuild-stamp"
  "/workspaces/Dual_RP2040_Audio_Player/build/pico-sdk/src/rp2_common/pico_cyw43_driver/pioasm/src"
  "/workspaces/Dual_RP2040_Audio_Player/build/pico-sdk/src/rp2_common/pico_cyw43_driver/pioasm/src/pioasmBuild-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/workspaces/Dual_RP2040_Audio_Player/build/pico-sdk/src/rp2_common/pico_cyw43_driver/pioasm/src/pioasmBuild-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/workspaces/Dual_RP2040_Audio_Player/build/pico-sdk/src/rp2_common/pico_cyw43_driver/pioasm/src/pioasmBuild-stamp${cfgdir}") # cfgdir has leading slash
endif()
