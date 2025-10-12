# ZMK Module CMakeLists.txt
# This file is required for ZMK module integration

if(CONFIG_ZMK)
    # Add custom source files
    target_sources(app PRIVATE src/bootloader_fix.c)
    
    # Add include directories if needed
    target_include_directories(app PRIVATE include)
endif()