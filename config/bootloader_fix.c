/*
 * Bootloader fix for Seeed XIAO nRF52840
 * 
 * This file provides a working bootloader reboot implementation
 * that sets the proper GPREGRET register before reset.
 */

#include <zephyr/kernel.h>
#include <zephyr/sys/reboot.h>
#include <zephyr/logging/log.h>

#if defined(CONFIG_SOC_NRF52840) || defined(CONFIG_SOC_SERIES_NRF52X) || defined(CONFIG_SOC_NRF52840_QIAA)
#include <hal/nrf_power.h>
#ifdef CONFIG_NRF_FORCE_RAM_ON_REBOOT
#include <nrfx_ram_ctrl.h>
#endif

LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

// Define bootloader magic constants
#define BOOTLOADER_DFU_GPREGRET_MASK      (0x01)
#define SYS_REBOOT_TO_BOOTLOADER          (2)

// Override the weak sys_arch_reboot implementation
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
    
#ifdef CONFIG_NRF_FORCE_RAM_ON_REBOOT
    // Keep RAM powered during reboot (from original Nordic implementation)
    uint8_t *ram_start;
    size_t ram_size = 0;

#if defined(NRF_MEMORY_RAM_BASE)
    ram_start = (uint8_t *)NRF_MEMORY_RAM_BASE;
#else
    ram_start = (uint8_t *)NRF_MEMORY_RAM0_BASE;
#endif

#if defined(NRF_MEMORY_RAM_SIZE)
    ram_size += NRF_MEMORY_RAM_SIZE;
#endif
#if defined(NRF_MEMORY_RAM0_SIZE)
    ram_size += NRF_MEMORY_RAM0_SIZE;
#endif
#if defined(NRF_MEMORY_RAM1_SIZE)
    ram_size += NRF_MEMORY_RAM1_SIZE;
#endif
#if defined(NRF_MEMORY_RAM2_SIZE)
    ram_size += NRF_MEMORY_RAM2_SIZE;
#endif

    if (ram_size > 0) {
        nrfx_ram_ctrl_power_enable_set(ram_start, ram_size, true);
    }
#endif

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

#endif /* nRF52840 variants */