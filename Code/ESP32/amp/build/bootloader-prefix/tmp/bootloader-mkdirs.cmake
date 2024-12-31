# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/Users/duane/Code/Github/esp-idf/components/bootloader/subproject"
  "/Users/duane/Code/personal/projects/hifi-amplifier/Code/ESP32/build/bootloader"
  "/Users/duane/Code/personal/projects/hifi-amplifier/Code/ESP32/build/bootloader-prefix"
  "/Users/duane/Code/personal/projects/hifi-amplifier/Code/ESP32/build/bootloader-prefix/tmp"
  "/Users/duane/Code/personal/projects/hifi-amplifier/Code/ESP32/build/bootloader-prefix/src/bootloader-stamp"
  "/Users/duane/Code/personal/projects/hifi-amplifier/Code/ESP32/build/bootloader-prefix/src"
  "/Users/duane/Code/personal/projects/hifi-amplifier/Code/ESP32/build/bootloader-prefix/src/bootloader-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/Users/duane/Code/personal/projects/hifi-amplifier/Code/ESP32/build/bootloader-prefix/src/bootloader-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/Users/duane/Code/personal/projects/hifi-amplifier/Code/ESP32/build/bootloader-prefix/src/bootloader-stamp${cfgdir}") # cfgdir has leading slash
endif()
