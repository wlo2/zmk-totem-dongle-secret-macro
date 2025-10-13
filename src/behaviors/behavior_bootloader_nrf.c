/*
 * Copyright (c) 2024 The ZMK Contributors
 *
 * SPDX-License-Identifier: MIT
 */

#define DT_DRV_COMPAT zmk_behavior_bootloader_nrf

#include <zephyr/device.h>
#include <drivers/behavior.h>
#include <zephyr/logging/log.h>
#include <zephyr/sys/reboot.h>
#include <zmk/behavior.h>
#include <zephyr/retention/bootmode.h>
#include <zephyr/drivers/retained_mem.h>

#if defined(CONFIG_SOC_FAMILY_NORDIC_NRF)
#include <hal/nrf_power.h>
#endif

#if defined(SOFTDEVICE_PRESENT)
#include <nrf_soc.h>
#endif

#if DT_HAS_COMPAT_STATUS_OKAY(DT_DRV_COMPAT)

LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

static int bootloader_nrf_init(const struct device *dev) {
#if defined(CONFIG_SOC_FAMILY_NORDIC_NRF)
    /* Diagnostic: capture reset reason at startup to verify warm reset path */
    LOG_INF("RESETREAS: 0x%08x", (unsigned int)NRF_POWER->RESETREAS);
#endif
    return 0;
}

static int on_keymap_binding_pressed(struct zmk_behavior_binding *binding,
                                   struct zmk_behavior_binding_event event) {
    ARG_UNUSED(binding);
    ARG_UNUSED(event);

#if defined(CONFIG_BEHAVIOR_BOOTLOADER_NRF_MODE_UF2)
    LOG_INF("Bootloader (UF2) trigger: setting GPREGRET/GPREGRET2=0x57 and warm reboot");

    /* Optional: write via explicit retained_mem nodes if available (gpregret index 0 only) */
#if DT_NODE_HAS_STATUS(DT_NODELABEL(gpregret), okay)
    {
        const struct device *gp0 = DEVICE_DT_GET(DT_NODELABEL(gpregret));
        if (device_is_ready(gp0)) {
            uint8_t val = 0x57;
            int w0 = retained_mem_write(gp0, 0, &val, 1);
            if (w0 < 0) {
                LOG_ERR("retained_mem_write gpregret failed: %d", w0);
            }
        }
    }
#endif
#if DT_NODE_HAS_STATUS(DT_NODELABEL(gpregret1), okay)
    {
        const struct device *gp1 = DEVICE_DT_GET(DT_NODELABEL(gpregret1));
        if (device_is_ready(gp1)) {
            uint8_t val = 0x57;
            int w1 = retained_mem_write(gp1, 0, &val, 1);
            if (w1 < 0) {
                LOG_ERR("retained_mem_write gpregret1 failed: %d", w1);
            }
        }
    }
#endif

    /* Always ensure both GPREGRET registers are set for UF2, regardless of retained_mem availability */
#if defined(SOFTDEVICE_PRESENT)
    (void)sd_power_gpregret_set(0, 0x57);
    (void)sd_power_gpregret_set(1, 0x57);
    LOG_INF("GPREGRET: 0x%02x GPREGRET2: 0x%02x", (int)NRF_POWER->GPREGRET, (int)NRF_POWER->GPREGRET2);
    sd_nvic_SystemReset();
    return ZMK_BEHAVIOR_OPAQUE;
#elif defined(CONFIG_SOC_FAMILY_NORDIC_NRF)
    NRF_POWER->GPREGRET = 0x57;
    NRF_POWER->GPREGRET2 = 0x57;
    LOG_INF("GPREGRET: 0x%02x GPREGRET2: 0x%02x", (int)NRF_POWER->GPREGRET, (int)NRF_POWER->GPREGRET2);
    sys_reboot(SYS_REBOOT_WARM);
    return ZMK_BEHAVIOR_OPAQUE;
#else
    LOG_WRN("UF2 GPREGRET write requested on non-Nordic SoC; skipping write");
    sys_reboot(SYS_REBOOT_WARM);
    return ZMK_BEHAVIOR_OPAQUE;
#endif

#elif defined(CONFIG_BEHAVIOR_BOOTLOADER_NRF_MODE_MCUBOOT)
    LOG_INF("Bootloader (MCUboot) trigger via Boot Mode API");
    int ret = bootmode_set(BOOT_MODE_TYPE_BOOTLOADER);
    if (ret < 0) {
        LOG_ERR("Failed to set MCUboot boot mode: %d", ret);
    }
    sys_reboot(SYS_REBOOT_WARM);
    return ZMK_BEHAVIOR_OPAQUE;

#else
    LOG_ERR("No bootloader mode selected. Enable UF2 or MCUboot mode in Kconfig.");
    return ZMK_BEHAVIOR_OPAQUE;
#endif
}

static int on_keymap_binding_released(struct zmk_behavior_binding *binding,
                                    struct zmk_behavior_binding_event event) {
    return ZMK_BEHAVIOR_OPAQUE;
}

static const struct behavior_driver_api behavior_bootloader_nrf_driver_api = {
    .binding_pressed = on_keymap_binding_pressed,
    .binding_released = on_keymap_binding_released,
    .locality = BEHAVIOR_LOCALITY_CENTRAL,
};

BEHAVIOR_DT_INST_DEFINE(0, bootloader_nrf_init, NULL, NULL, NULL, APPLICATION,
                        CONFIG_KERNEL_INIT_PRIORITY_DEFAULT, &behavior_bootloader_nrf_driver_api);

#endif