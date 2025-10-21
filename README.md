# Secret Macro Injection Feature

You can now include sensitive macros in your firmware builds via repository secrets without exposing them in your public repository.

## How It Works

The build system automatically generates a `secret_macro` behavior during GitHub Actions builds using repository secrets. This allows you to include passwords, API keys, personal shortcuts, or other sensitive key sequences without committing them to your public codebase.

### Implementation

1. **String-to-Bindings Conversion**: Plain text strings are automatically converted to ZMK bindings at build time
2. **Dynamic Overlay Generation**: A `secret_macro.overlay` file is generated during the build with your secret content
3. **GitHub Secrets Integration**: Uses GitHub repository secrets to inject the actual macro content
4. **Pre-baked Build Environment**: Utilizes a containerized Zephyr environment for consistent builds

### Generated Macro Structure

The system creates a macro with these specifications:
- **Name**: `secret_macro`
- **Type**: ZMK behavior macro
- **Timing**: 10ms wait and tap intervals (optimized for quick typing; 5ms may also work)
- **Content**: Populated from GitHub repository secret

## Setup Instructions

### 1. Configure GitHub Repository Secret

1. Go to your repository **Settings** → **Secrets and variables** → **Actions**
2. Click **New repository secret**
3. Name: `STRING_PLACEHOLDER`
4. Value: Your secret text as a plain string (e.g., `Hello World!` or `MyP@ssw0rd123`)

**Note**: The system now accepts plain text strings and automatically converts them to ZMK bindings. You can still use raw bindings (e.g., `&kp H &kp E &kp L &kp L &kp O`) if needed.

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

### 3. Add Required Files and Workflow Jobs

#### Step 3.0: Add the String Converter Script

Create the file `.github/workflows/string_to_zmk.sh` in your repository with the converter script that handles character-to-binding translation. This script supports:
- Letters (a-z, A-Z with automatic shift)
- Numbers (0-9)
- Special characters (!, @, #, $, etc.)
- Punctuation marks

Make sure the file is executable (it will be made executable automatically during the build).

#### Step 3.1: Add the `Prepare Variables` Job

Add this job to set up the required environment variables:

```yaml
- name: Prepare variables
  shell: sh -x {0}
  env:
    board: ${{ matrix.board }}
    shield: ${{ matrix.shield }}
    artifact_name: ${{ matrix.artifact-name }}
    snippet: ${{ matrix.snippet }}
  run: |
    # Use pre-baked west workspace inside the container image
    echo "base_dir=/opt/zmk-env" >> $GITHUB_ENV
    echo "ZEPHYR_BASE=/opt/zmk-env/zephyr" >> $GITHUB_ENV
    echo "ZMK_AUTOCORRECT_PATH=${GITHUB_WORKSPACE}/zmk_autocorrect" >> $GITHUB_ENV
    echo "ANTE_MORPH_PATH=${GITHUB_WORKSPACE}/zmk-antecedent-morph" >> $GITHUB_ENV
    export zmk_load_arg=" -DZMK_EXTRA_MODULES='${GITHUB_WORKSPACE};${GITHUB_WORKSPACE}/zmk_autocorrect;${GITHUB_WORKSPACE}/zmk-antecedent-morph'"

    if [ -n "${snippet}" ]; then
      extra_west_args="-S \"${snippet}\""
    fi

    echo "zephyr_version=${ZEPHYR_VERSION}" >> $GITHUB_ENV
    echo "extra_west_args=${extra_west_args}" >> $GITHUB_ENV
    echo "extra_cmake_args=${shield:+-DSHIELD=\"$shield\"}${zmk_load_arg}" >> $GITHUB_ENV
    echo "display_name=${shield:+$shield - }${board}" >> $GITHUB_ENV
    echo "artifact_name=${artifact_name:-${shield:+$shield-}${board}-zmk}" >> $GITHUB_ENV
```

**Key Function**: This job sets environment variables for the pre-baked Zephyr workspace and module paths.

#### Step 3.2: Add the `Secret Macro Generation` Job

Add this job after the "Prepare variables" step:

```yaml
- name: Generate secret macro overlay
  run: |
    echo "Generating secret macro overlay..."
    
    # Create config directory
    mkdir -p "${GITHUB_WORKSPACE}/config"
    
    # Make the converter script executable
    chmod +x "${GITHUB_WORKSPACE}/.github/workflows/string_to_zmk.sh"
    
    # Check if secret is available
    if [ -z "${{ secrets.STRING_PLACEHOLDER }}" ]; then
      echo "Warning: STRING_PLACEHOLDER secret not set or empty"
      echo "Creating fallback macro that outputs placeholder text"
      
      # Create fallback macro
      printf "behaviors {\n" > "${GITHUB_WORKSPACE}/config/secret_macro.overlay"
      printf "    secret_macro: secret_macro {\n" >> "${GITHUB_WORKSPACE}/config/secret_macro.overlay"
      printf "        compatible = \"zmk,behavior-macro\";\n" >> "${GITHUB_WORKSPACE}/config/secret_macro.overlay"
      printf "        #binding-cells = <0>;\n" >> "${GITHUB_WORKSPACE}/config/secret_macro.overlay"
      printf "        bindings = <&kp S &kp E &kp C &kp R &kp E &kp T>;\n" >> "${GITHUB_WORKSPACE}/config/secret_macro.overlay"
      printf "        wait-ms = <10>;\n" >> "${GITHUB_WORKSPACE}/config/secret_macro.overlay"
      printf "        tap-ms = <10>;\n" >> "${GITHUB_WORKSPACE}/config/secret_macro.overlay"
      printf "    };\n" >> "${GITHUB_WORKSPACE}/config/secret_macro.overlay"
      printf "};\n" >> "${GITHUB_WORKSPACE}/config/secret_macro.overlay"
      
      echo "Fallback secret macro created (types 'SECRET')"
    else
      echo "STRING_PLACEHOLDER secret found, converting string to bindings"
      
      SECRET_STRING="${{ secrets.STRING_PLACEHOLDER }}"
      
      # Check if the secret is already in binding format (starts with &)
      if echo "$SECRET_STRING" | grep -q '^&'; then
        echo "Secret appears to be in binding format already, using as-is"
        BINDINGS="$SECRET_STRING"
      else
        echo "Converting string to ZMK bindings using external script..."
        BINDINGS=$("${GITHUB_WORKSPACE}/.github/workflows/string_to_zmk.sh" "$SECRET_STRING")
        
        if [ $? -ne 0 ] || [ -z "$BINDINGS" ]; then
          echo "Error: Failed to convert string to bindings"
          exit 1
        fi
      fi
      
      # Create the secret macro
      printf "behaviors {\n" > "${GITHUB_WORKSPACE}/config/secret_macro.overlay"
      printf "    secret_macro: secret_macro {\n" >> "${GITHUB_WORKSPACE}/config/secret_macro.overlay"
      printf "        compatible = \"zmk,behavior-macro\";\n" >> "${GITHUB_WORKSPACE}/config/secret_macro.overlay"
      printf "        #binding-cells = <0>;\n" >> "${GITHUB_WORKSPACE}/config/secret_macro.overlay"
      echo "        bindings = <$BINDINGS>;" >> "${GITHUB_WORKSPACE}/config/secret_macro.overlay"
      printf "        wait-ms = <10>;\n" >> "${GITHUB_WORKSPACE}/config/secret_macro.overlay"
      printf "        tap-ms = <10>;\n" >> "${GITHUB_WORKSPACE}/config/secret_macro.overlay"
      printf "    };\n" >> "${GITHUB_WORKSPACE}/config/secret_macro.overlay"
      printf "};\n" >> "${GITHUB_WORKSPACE}/config/secret_macro.overlay"
      
      echo "Custom secret macro created successfully"
    fi
    
    # Verify the generated file exists without displaying content
    if [ -f "${GITHUB_WORKSPACE}/config/secret_macro.overlay" ]; then
      echo "Secret macro overlay file generated"
      
      # Basic syntax validation
      if grep -q "behaviors {" "${GITHUB_WORKSPACE}/config/secret_macro.overlay" && \
         grep -q "secret_macro:" "${GITHUB_WORKSPACE}/config/secret_macro.overlay" && \
         grep -q "bindings = <" "${GITHUB_WORKSPACE}/config/secret_macro.overlay"; then
        echo "Overlay file structure validation passed"
      else
        echo "Error: Generated overlay file has invalid structure"
        exit 1
      fi
    else
      echo "Error: Failed to create secret_macro.overlay file"
      exit 1
    fi
  env:
    STRING_PLACEHOLDER: ${{ secrets.STRING_PLACEHOLDER }}
```

#### Step 3.3: Ensure Your Build Command Uses the Config

Your West build command should reference the workspace config directory:

```yaml
- name: West Build (${{ env.display_name }})
  working-directory: /opt/zmk-env
  shell: sh -x {0}
  env:
    ZEPHYR_BASE: ${{ env.ZEPHYR_BASE }}
  run: west build -s zmk/app -d "${{ env.build_dir }}" -b "${{ matrix.board }}" ${{ env.extra_west_args }} -- -DZMK_CONFIG=${GITHUB_WORKSPACE}/config -DZephyr_DIR=${ZEPHYR_BASE}/share/zephyr-package/cmake ${{ env.extra_cmake_args }} ${{ matrix.cmake-args }}
```
## Example Use Cases
- **Passwords**: `MyP@ssw0rd123`
- **Email addresses**: `john@example.com`
- **Usernames**: `john_doe_2024`

### Raw Binding Format
If you prefer to use raw ZMK bindings:
- **Manual bindings**: `&kp H &kp E &kp L &kp L &kp O`
- **With modifiers**: `&kp LS(H) &kp E &kp L &kp L &kp O`

## Supported Characters

The converter supports:
- **Lowercase letters**: a-z
- **Uppercase letters**: A-Z (automatically adds shift)
- **Numbers**: 0-9
- **Space**: (space character)
- **Special characters**: ! @ # $ % ^ & * ( ) - _ = + [ ] { } \ | ; : ' " , < . > / ? ` ~

Unsupported characters will generate a warning and be skipped during conversion.

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

## `&bootloader` Support for nRF52840 with Adafruit bootloader (Zephyr 4.1)

This configuration uses ZMK's bootloader support from the Zephyr 4.1 branch (`petejohanson/zmk@core/move-to-zephyr-4-1`). The `&bootloader` behavior allows entering the device bootloader from a keypress.

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
- **ZMK branch**: `petejohanson/zmk@core/move-to-zephyr-4-1` with Zephyr 4.1 support (still in beta)