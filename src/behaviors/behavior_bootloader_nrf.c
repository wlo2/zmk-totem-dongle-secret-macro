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

#if DT_HAS_COMPAT_STATUS_OKAY(DT_DRV_COMPAT)

LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

static int bootloader_nrf_init(const struct device *dev) { return 0; }

static int on_keymap_binding_pressed(struct zmk_behavior_binding *binding,
                                   struct zmk_behavior_binding_event event) {
    ARG_UNUSED(binding);
    ARG_UNUSED(event);

#if defined(CONFIG_BEHAVIOR_BOOTLOADER_NRF_MODE_UF2)
    LOG_INF("Bootloader (UF2) trigger: writing GPREGRET=0x57 and rebooting (cold)");

    /* Prefer Zephyr retained_mem API if available */
    const struct device *gp = DEVICE_DT_GET_ONE(nordic_nrf_gpregret);
    if (device_is_ready(gp)) {
        uint8_t val = 0x57;
        int wret = retained_mem_write(gp, 0, &val, 1);
        if (wret < 0) {
            LOG_ERR("retained_mem_write failed: %d", wret);
        }
    } else {
        LOG_WRN("retained_mem device not ready; falling back to direct GPREGRET write (Nordic only)");

#if defined(CONFIG_SOC_FAMILY_NORDIC_NRF)
        NRF_POWER->GPREGRET = 0x57;
#else
        LOG_WRN("UF2 GPREGRET write requested on non-Nordic SoC; skipping write");
#endif
    }

#if defined(CONFIG_SOC_FAMILY_NORDIC_NRF)
    LOG_INF("GPREGRET now: 0x%02x", (int)NRF_POWER->GPREGRET);
#endif
    LOG_INF("Locality: CENTRAL");
    sys_reboot(SYS_REBOOT_COLD);
    return ZMK_BEHAVIOR_OPAQUE;

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