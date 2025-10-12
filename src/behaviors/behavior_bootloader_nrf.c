/*
 * Copyright (c) 2024 The ZMK Contributors
 *
 * SPDX-License-Identifier: MIT
 */

#define DT_DRV_COMPAT zmk_behavior_bootloader_nrf

#include <zephyr/device.h>
#include <drivers/behavior.h>
#include <zephyr/logging/log.h>
#include <zmk/behavior.h>

#if DT_HAS_COMPAT_STATUS_OKAY(DT_DRV_COMPAT)

#if defined(CONFIG_SOC_NRF52840) || defined(CONFIG_SOC_SERIES_NRF52X) || defined(CONFIG_SOC_NRF52840_QIAA)
#include <hal/nrf_power.h>
#endif

LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

static int bootloader_nrf_init(const struct device *dev) { return 0; }

static int on_keymap_binding_pressed(struct zmk_behavior_binding *binding,
                                   struct zmk_behavior_binding_event event) {
    LOG_INF("nRF bootloader behavior triggered");
    
#if defined(CONFIG_SOC_NRF52840) || defined(CONFIG_SOC_SERIES_NRF52X) || defined(CONFIG_SOC_NRF52840_QIAA)
    // Set DFU trigger in GPREGRET register
    nrf_power_gpregret_set(NRF_POWER, 0x01);
    LOG_INF("GPREGRET set to 0x01 for DFU mode");
    
    // Small delay to ensure register write completes
    k_msleep(10);
#endif
    
    // Perform system reset
    NVIC_SystemReset();
    
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