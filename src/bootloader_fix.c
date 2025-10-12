/*
 * Bootloader fix for Seeed XIAO nRF52840
 * 
 * This file provides a working bootloader reboot implementation
 * that sets the proper GPREGRET register before reset.
 */

#include <zephyr/kernel.h>
#include <zephyr/sys/reboot.h>
#include <zephyr/logging/log.h>

#ifdef CONFIG_BOOTLOADER_FIX_NRF52840
#include <hal/nrf_power.h>

LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

// Define bootloader magic constants
#define BOOTLOADER_DFU_GPREGRET_MASK      (0x01)
#define SYS_REBOOT_TO_BOOTLOADER          (2)

// Override the sys_arch_reboot implementation
// Since Nordic implementation is not weak, we need to provide strong override
void sys_arch_reboot(int type)
{
    LOG_INF("sys_arch_reboot called with type: %d", type);
    
    // Handle bootloader-specific reboot
    if (type == SYS_REBOOT_TO_BOOTLOADER) {
        LOG_INF("Entering bootloader mode...");
        
        // Set the DFU trigger bit in GPREGRET register
        // This tells the bootloader to enter DFU mode on next boot
        nrf_power_gpregret_set(NRF_POWER, BOOTLOADER_DFU_GPREGRET_MASK);
        
        LOG_INF("GPREGRET set to 0x%02x", nrf_power_gpregret_get(NRF_POWER));
    } else {
        LOG_INF("Normal reboot (type %d)", type);
        
        // For normal reboots, clear GPREGRET to ensure normal boot
        nrf_power_gpregret_set(NRF_POWER, 0x00);
    }
    
    // Small delay to ensure GPREGRET write completes
    k_msleep(10);
    
    // Perform the actual system reset
    NVIC_SystemReset();
    
    // Should never reach here
    LOG_ERR("SystemReset failed!");
    while (1) {
        k_cpu_idle();
    }
}

#endif /* CONFIG_BOOTLOADER_FIX_NRF52840 */