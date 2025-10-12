/*
 * Copyright (c) 2024 The ZMK Contributors
 *
 * SPDX-License-Identifier: MIT
 */

#define DT_DRV_COMPAT zmk_behavior_bootloader_nrf

#include <zephyr/device.h>
#include <drivers/behavior.h>
#include <zephyr/logging/log.h>
#include <zephyr/retention/bootmode.h>
#include <zephyr/sys/reboot.h>
#include <zmk/behavior.h>

#if DT_HAS_COMPAT_STATUS_OKAY(DT_DRV_COMPAT)

LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

static int bootloader_nrf_init(const struct device *dev) { return 0; }

static int on_keymap_binding_pressed(struct zmk_behavior_binding *binding,
                                   struct zmk_behavior_binding_event event) {
    LOG_INF("nRF bootloader behavior triggered via Boot Mode API");
    
    // Use the official Zephyr Boot Mode API
    int ret = bootmode_set(BOOT_MODE_TYPE_BOOTLOADER);
    if (ret < 0) {
        LOG_ERR("Failed to set boot mode: %d", ret);
        return ret;
    }
    
    LOG_INF("Boot mode set to BOOTLOADER, rebooting...");
    
    // Use the official reboot API
    sys_reboot(SYS_REBOOT_WARM);
    
    return ZMK_BEHAVIOR_OPAQUE;
}

static int on_keymap_binding_released(struct zmk_behavior_binding *binding,
                                    struct zmk_behavior_binding_event event) {
    return ZMK_BEHAVIOR_OPAQUE;
}

static const struct behavior_driver_api behavior_bootloader_nrf_driver_api = {
    .binding_pressed = on_keymap_binding_pressed,
    .binding_released = on_keymap_binding_released,
};

BEHAVIOR_DT_INST_DEFINE(0, bootloader_nrf_init, NULL, NULL, NULL, APPLICATION,
                        CONFIG_KERNEL_INIT_PRIORITY_DEFAULT, &behavior_bootloader_nrf_driver_api);

#endif