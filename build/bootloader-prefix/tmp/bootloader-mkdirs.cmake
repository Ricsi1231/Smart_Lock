# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/home/nagyrichard/ESP/esp-idf/components/bootloader/subproject"
  "/home/nagyrichard/Documents/Smart_Lock/Firmware/build/bootloader"
  "/home/nagyrichard/Documents/Smart_Lock/Firmware/build/bootloader-prefix"
  "/home/nagyrichard/Documents/Smart_Lock/Firmware/build/bootloader-prefix/tmp"
  "/home/nagyrichard/Documents/Smart_Lock/Firmware/build/bootloader-prefix/src/bootloader-stamp"
  "/home/nagyrichard/Documents/Smart_Lock/Firmware/build/bootloader-prefix/src"
  "/home/nagyrichard/Documents/Smart_Lock/Firmware/build/bootloader-prefix/src/bootloader-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/home/nagyrichard/Documents/Smart_Lock/Firmware/build/bootloader-prefix/src/bootloader-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/home/nagyrichard/Documents/Smart_Lock/Firmware/build/bootloader-prefix/src/bootloader-stamp${cfgdir}") # cfgdir has leading slash
endif()
