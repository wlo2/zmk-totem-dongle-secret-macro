# ZMK Module CMakeLists.txt
# Following official ZMK new behavior development guidelines

if(CONFIG_ZMK_BEHAVIOR_BOOTLOADER_NRF)
    target_sources(app PRIVATE src/behaviors/behavior_bootloader_nrf.c)
endif()