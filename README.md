# Secret Macro Injection Feature

You can now include sensitive macros in your firmware builds via repository secrets without exposing them in your public repository.

## How It Works

The build system automatically generates a `secret_macro` behavior during GitHub Actions builds using repository secrets. This allows you to include passwords, API keys, personal shortcuts, or other sensitive key sequences without committing them to your public codebase.

### Implementation

1. **Isolated Build Environment**: Config files are copied to a temporary directory to avoid contaminating the workspace
2. **Dynamic Overlay Generation**: A `secret_macro.overlay` file is generated at build time with your secret content
3. **GitHub Secrets Integration**: Uses GitHub repository secrets to inject the actual macro content

### Generated Macro Structure

The system creates a macro with these specifications:
- **Name**: `secret_macro`
- **Type**: ZMK behavior macro
- **Timing**: 10ms wait and tap intervals (for quick typing of long strings. I haven't tested further, probably even 5ms is good)
- **Content**: Populated from GitHub repository secret

## Setup Instructions

### 1. Configure GitHub Repository Secret

1. Go to your repository **Settings** → **Secrets and variables** → **Actions**
2. Click **New repository secret**
3. Name: `STRING_PLACEHOLDER`
4. Value: Your ZMK macro bindings (e.g., `&kp H &kp E &kp L &kp L &kp O`)

### 2. Use the Secret Macro in Your Keymap

Add the `&secret_macro` keycode to any key position in your keymap:

```dts
keymap {
    compatible = "zmk,keymap";
    
    default_layer {
        bindings = <
            // ... other keys ...
            &secret_macro    
            // ... other keys ...
        >;
    };
};
```

### 3. Adding build jobs

#### Step 3.0: Add the `Prepare Variables Job`
First, add this job to set up the required environment variables (if you don't already have it):

```yaml
- name: Prepare variables
  shell: sh -x {0}
  env:
    board: ${{ matrix.board }}
    shield: ${{ matrix.shield }}
    artifact_name: ${{ matrix.artifact-name }}
    snippet: ${{ matrix.snippet }}
  run: |
    if [ -e zephyr/module.yml ]; then
      export zmk_load_arg=" -DZMK_EXTRA_MODULES='${GITHUB_WORKSPACE}'"
      new_tmp_dir="${TMPDIR:-/tmp}/zmk-config"
      mkdir -p "${new_tmp_dir}"
      echo "base_dir=${new_tmp_dir}" >> $GITHUB_ENV
    else
      echo "base_dir=${GITHUB_WORKSPACE}" >> $GITHUB_ENV
    fi

    if [ -n "${snippet}" ]; then
      extra_west_args="-S \"${snippet}\""
    fi

    echo "zephyr_version=${ZEPHYR_VERSION}" >> $GITHUB_ENV
    echo "extra_west_args=${extra_west_args}" >> $GITHUB_ENV
    echo "extra_cmake_args=${shield:+-DSHIELD=\"$shield\"}${zmk_load_arg}" >> $GITHUB_ENV
    echo "display_name=${shield:+$shield - }${board}" >> $GITHUB_ENV
    echo "artifact_name=${artifact_name:-${shield:+$shield-}${board}-zmk}" >> $GITHUB_ENV
```

**Key Function**: This job sets the `base_dir` environment variable that determines whether to use an isolated temporary directory or the workspace directory, which is essential for the next steps.

#### Step 3.1: Add the `Copy Config Files Job`
Add this job to your `.github/workflows/build.yml` file after the "Prepare variables"  step:

```yaml
- name: Copy config files to isolated temporary directory
  run: |
    if [ "${{ env.base_dir }}" != "${GITHUB_WORKSPACE}" ]; then
      mkdir "${{ env.base_dir }}/config"
      cp -R config/* "${{ env.base_dir }}/config/"
    fi
```

**Requirements**: 
- Your workflow must have a `base_dir` environment variable set (typically in a "Prepare variables" step)
- This job should run before any build steps that need the config files

#### Step 3.2: Add the `Secret Macro Generation` Job
Add this job after the copy config files step:

```yaml
- name: Generate secret macro overlay
  run: |
    echo "Generating secret macro overlay..."
    
    # Create config directory
    mkdir -p "${{ env.base_dir }}/config"
    
    # Check if secret is available
    if [ -z "${{ secrets.STRING_PLACEHOLDER }}" ]; then
      echo "Warning: STRING_PLACEHOLDER secret not set or empty"
      echo "Creating fallback macro that outputs placeholder text"
      
      # Create fallback macro
      printf "behaviors {\n" > "${{ env.base_dir }}/config/secret_macro.overlay"
      printf "    secret_macro: secret_macro {\n" >> "${{ env.base_dir }}/config/secret_macro.overlay"
      printf "        compatible = \"zmk,behavior-macro\";\n" >> "${{ env.base_dir }}/config/secret_macro.overlay"
      printf "        #binding-cells = <0>;\n" >> "${{ env.base_dir }}/config/secret_macro.overlay"
      printf "        bindings = <&kp S &kp E &kp C &kp R &kp E &kp T>;\n" >> "${{ env.base_dir }}/config/secret_macro.overlay"
      printf "        wait-ms = <10>;\n" >> "${{ env.base_dir }}/config/secret_macro.overlay"
      printf "        tap-ms = <10>;\n" >> "${{ env.base_dir }}/config/secret_macro.overlay"
      printf "    };\n" >> "${{ env.base_dir }}/config/secret_macro.overlay"
      printf "};\n" >> "${{ env.base_dir }}/config/secret_macro.overlay"
      
      echo "Fallback secret macro created (types 'SECRET')"
    else
      echo "STRING_PLACEHOLDER secret found, creating custom macro"
      
      # Create the actual secret macro
      printf "behaviors {\n" > "${{ env.base_dir }}/config/secret_macro.overlay"
      printf "    secret_macro: secret_macro {\n" >> "${{ env.base_dir }}/config/secret_macro.overlay"
      printf "        compatible = \"zmk,behavior-macro\";\n" >> "${{ env.base_dir }}/config/secret_macro.overlay"
      printf "        #binding-cells = <0>;\n" >> "${{ env.base_dir }}/config/secret_macro.overlay"
      echo "        bindings = <${{ secrets.STRING_PLACEHOLDER }}>;" >> "${{ env.base_dir }}/config/secret_macro.overlay"
      printf "        wait-ms = <10>;\n" >> "${{ env.base_dir }}/config/secret_macro.overlay"
      printf "        tap-ms = <10>;\n" >> "${{ env.base_dir }}/config/secret_macro.overlay"
      printf "    };\n" >> "${{ env.base_dir }}/config/secret_macro.overlay"
      printf "};\n" >> "${{ env.base_dir }}/config/secret_macro.overlay"
      
      echo "Custom secret macro created successfully"
    fi
    
    # Verify the generated file
    if [ -f "${{ env.base_dir }}/config/secret_macro.overlay" ]; then
      echo "Generated overlay file:"
      cat "${{ env.base_dir }}/config/secret_macro.overlay"
    else
      echo "Error: Failed to create secret_macro.overlay file"
      exit 1
    fi
  env:
    STRING_PLACEHOLDER: ${{ secrets.STRING_PLACEHOLDER }}
```

#### Step 3.3: Verify Your Workflow Dependencies
Ensure your workflow has these required elements:
- A job that sets the `base_dir` environment variable
- The `secrets.STRING_PLACEHOLDER` reference in the environment section
- Your build job uses `-DZMK_CONFIG=${{ env.base_dir }}/config` to include the generated overlay

## Example Use Cases

- **Passwords**: `&kp P &kp A &kp S &kp S &kp W &kp O &kp R &kp D`
- **Email addresses**: `&kp J &kp O &kp H &kp N &kp AT &kp E &kp X &kp A &kp M &kp P &kp L &kp E &kp DOT &kp C &kp O &kp M`
- **API keys**: Complex sequences of characters and numbers
- **Personal shortcuts**: Frequently used text snippets

## Important Notes

- The secret macro is only available in GitHub Actions builds
- Local builds will fail if they reference `&secret_macro` without the overlay
- Consider using conditional compilation if you need local build compatibility
- Keep your secret content in valid ZMK macro binding format

## Security considerations
Although not fully secure—someone with physical access to the board could potentially extract the secret—doing so requires knowing the board layout and is difficult to reverse-engineer if the layout is unknown. Evaluate your risks.

# Config modifications

## Dongle keyscanning disabled
```dts
/ {
    chosen {
        zmk,kscan = &mock_kscan;
        wakeup-source;
    };

    mock_kscan: kscan_1 {
        compatible = "zmk,kscan-mock";
        columns = <0>;
        rows = <0>;
        events = <0>;
    };
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

## Bootloader Trigger Behavior (`&bootloader_nrf`)

This config provides a custom ZMK behavior to enter bootloader on Nordic SoCs with two selectable modes.

### Modes

- **UF2 (default)**: Writes `GPREGRET=0x57` and performs a warm reboot.
  - Kconfig: `CONFIG_ZMK_BEHAVIOR_BOOTLOADER_NRF=y`, `CONFIG_BEHAVIOR_BOOTLOADER_NRF_MODE_UF2=y`.
  - SoC: Nordic nRF (`CONFIG_SOC_FAMILY_NORDIC_NRF`).
  - No Zephyr Boot Mode or retention required.

- **MCUboot Serial Recovery**: Sets Zephyr Boot Mode to `BOOT_MODE_TYPE_BOOTLOADER` and warm reboots.
  - Kconfig (enable when using MCUboot):
    - `CONFIG_ZMK_BEHAVIOR_BOOTLOADER_NRF=y`
    - `CONFIG_BEHAVIOR_BOOTLOADER_NRF_MODE_MCUBOOT=y`
    - `CONFIG_RETENTION=y`
    - `CONFIG_RETENTION_BOOT_MODE=y`
    - `CONFIG_RETAINED_MEM=y`
    - `CONFIG_MCUBOOT_SERIAL=y`
    - `CONFIG_BOOT_SERIAL_BOOT_MODE=y`
  - DT requirements (see below): chosen boot-mode with a retention node.

### DeviceTree Requirements

- Behavior node is provided by `config/bootloader_fix.dtsi` and included in `config/totem.keymap`.
- For MCUboot path: ensure a retention boot-mode node and chosen is present.
  - Example (present in `boards/shields/totem/totem_*.overlay`):
```dts
&gpregret1 {
    status = "okay";
    boot_mode0: retention@0 {
        compatible = "zephyr,retention";
        status = "okay";
        reg = <0x0 0x1>;
    };
};

/ {
    chosen {
        zephyr,boot-mode = &boot_mode0;
    };
};
```
