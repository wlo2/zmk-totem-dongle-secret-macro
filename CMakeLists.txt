# ZMK Module CMakeLists.txt
# Simple module integration for bootloader fix

if(CONFIG_ZMK)
    target_sources(app PRIVATE src/bootloader_fix.c)
endif()