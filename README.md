# zmk-totem-dongle

ZMK config for a wireless [TOTEM](https://github.com/GEIGEIGEIST/TOTEM) (38 keys, column-staggered split)
running on three Seeed Studio XIAO nRF52840 boards: two battery-powered halves plus a USB dongle acting
as the split central.

# Config modifications

## Dongle keyscanning disabled

The dongle has no switches wired to it, so `chosen` points at a mock kscan and the
real matrix node inherited from `totem.dtsi` is disabled outright. Both halves are
needed: re-pointing `chosen` only changes which kscan ZMK reads from, while a node
with no `status` property still defaults to enabled and would have the matrix driver
configure nine unconnected GPIOs on the dongle.

```dts
/ {
    chosen {
        zmk,kscan = &mock_kscan;
    };

    mock_kscan: kscan_1 {
        compatible = "zmk,kscan-mock";
        columns = <0>;
        rows = <0>;
        events = <0>;
    };
};

&kscan0 {
    status = "disabled";
};
```

## Boilerplate keymap
Removed as redundant

## `timestamp_generator` job
Artifacts have naming scheme `zmk_DDMMYYYY_HHMM`

# Layout description
tbd

# Attributions
Initially forked from https://github.com/eigatech/zmk-config

## `&bootloader` Support for nRF52840 with Adafruit bootloader (Zephyr 4.1)

This configuration uses ZMK's bootloader support using Zephyr 4.1. The `&bootloader` behavior allows entering the device bootloader from a keypress.

### Configuration

#### Dongle Configuration

The dongle is configured to support bootloader entry via retention memory and magic mapper:

**`boards/shields/totem/totem_dongle.conf`:**
```conf
CONFIG_RETENTION=y
CONFIG_RETENTION_BOOT_MODE=y
CONFIG_RETAINED_MEM=y
CONFIG_ZMK_BOOTMODE_MAGIC_VALUE_BOOTLOADER_TYPE_ADAFRUIT_NRF52=y
```

### Usage

#### Triggering Bootloader on Dongle from periferals

> [!IMPORTANT]
> Requires to be called from macro (for `Central` locality) 

### Requirements

- **Boards**: `xiao_ble` board definition for Seeeduino XIAO BLE and BLE Sense
- **UF2 bootloader**: Adafruit/TinyUF2 bootloader version 0.9.2 or later required on XIAO BLE boards. Older Seeed UF2 bootloaders (e.g. 0.6.x) may not support this behavior.
