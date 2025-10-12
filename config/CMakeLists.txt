# ZMK Config CMakeLists.txt
# This file ensures custom C files are compiled into the firmware

target_sources(app PRIVATE bootloader_fix.c)