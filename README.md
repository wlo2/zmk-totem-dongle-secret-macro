# Secret Macro Injection Feature

You can now include sensitive macros in your firmware builds via repository secrets without exposing them in your public repository.

## How It Works

The build system automatically generates `secret_macro` and `secret_macro2` behaviors during GitHub Actions builds using repository secrets. This allows you to include passwords, API keys, personal shortcuts, or other sensitive key sequences without committing them to your public codebase.

### Implementation

1. **String-to-Bindings Conversion**: Plain text strings are automatically converted to ZMK bindings at build time
2. **Dynamic Overlay Generation**: `secret_macro.overlay` and `secret_macro2.overlay` files are generated during the build with your secret content
3. **GitHub Secrets Integration**: Uses GitHub repository secrets to inject the actual macro content
4. **Pre-baked Build Environment**: Utilizes a containerized Zephyr environment for consistent builds

### Generated Macro Structure

The system creates macros with these specifications:
- **Names**: `secret_macro` and `secret_macro2`
- **Type**: ZMK behavior macros
- **Timing**: 10ms wait and tap intervals (optimized for quick typing; 5ms may also work)
- **Content**: Populated from GitHub repository secrets
- **Security**: No secret values appear in build logs

## Quick Start: Migrating from Default ZMK Workflow

If you're currently using the default ZMK build workflow, follow these steps to add secret macro support.

### Current Default Workflow

Default workflow:
 `.github/workflows/build.yml`:

```yaml
name: Build ZMK firmware

on: [push, pull_request, workflow_dispatch]

jobs:
  build:
    uses: zmkfirmware/zmk/.github/workflows/build-user-config.yml@main
```

### Step 1: Create the String Converter Script

Create `.github/workflows/string_to_zmk.sh`:

```bash
#!/bin/bash

# Converts plain text to ZMK keypress bindings

input_string="$1"

if [ -z "$input_string" ]; then
    echo "Error: No input string provided" >&2
    exit 1
fi

output=""

for (( i=0; i<${#input_string}; i++ )); do
    char="${input_string:$i:1}"
    
    case "$char" in
        # Lowercase letters
        a) output="$output &kp A" ;;
        b) output="$output &kp B" ;;
        c) output="$output &kp C" ;;
        d) output="$output &kp D" ;;
        e) output="$output &kp E" ;;
        f) output="$output &kp F" ;;
        g) output="$output &kp G" ;;
        h) output="$output &kp H" ;;
        i) output="$output &kp I" ;;
        j) output="$output &kp J" ;;
        k) output="$output &kp K" ;;
        l) output="$output &kp L" ;;
        m) output="$output &kp M" ;;
        n) output="$output &kp N" ;;
        o) output="$output &kp O" ;;
        p) output="$output &kp P" ;;
        q) output="$output &kp Q" ;;
        r) output="$output &kp R" ;;
        s) output="$output &kp S" ;;
        t) output="$output &kp T" ;;
        u) output="$output &kp U" ;;
        v) output="$output &kp V" ;;
        w) output="$output &kp W" ;;
        x) output="$output &kp X" ;;
        y) output="$output &kp Y" ;;
        z) output="$output &kp Z" ;;
        
        # Uppercase letters (with shift)
        A) output="$output &kp LS(A)" ;;
        B) output="$output &kp LS(B)" ;;
        C) output="$output &kp LS(C)" ;;
        D) output="$output &kp LS(D)" ;;
        E) output="$output &kp LS(E)" ;;
        F) output="$output &kp LS(F)" ;;
        G) output="$output &kp LS(G)" ;;
        H) output="$output &kp LS(H)" ;;
        I) output="$output &kp LS(I)" ;;
        J) output="$output &kp LS(J)" ;;
        K) output="$output &kp LS(K)" ;;
        L) output="$output &kp LS(L)" ;;
        M) output="$output &kp LS(M)" ;;
        N) output="$output &kp LS(N)" ;;
        O) output="$output &kp LS(O)" ;;
        P) output="$output &kp LS(P)" ;;
        Q) output="$output &kp LS(Q)" ;;
        R) output="$output &kp LS(R)" ;;
        S) output="$output &kp LS(S)" ;;
        T) output="$output &kp LS(T)" ;;
        U) output="$output &kp LS(U)" ;;
        V) output="$output &kp LS(V)" ;;
        W) output="$output &kp LS(W)" ;;
        X) output="$output &kp LS(X)" ;;
        Y) output="$output &kp LS(Y)" ;;
        Z) output="$output &kp LS(Z)" ;;
        
        # Numbers
        0) output="$output &kp N0" ;;
        1) output="$output &kp N1" ;;
        2) output="$output &kp N2" ;;
        3) output="$output &kp N3" ;;
        4) output="$output &kp N4" ;;
        5) output="$output &kp N5" ;;
        6) output="$output &kp N6" ;;
        7) output="$output &kp N7" ;;
        8) output="$output &kp N8" ;;
        9) output="$output &kp N9" ;;
        
        # Special characters
        ' ') output="$output &kp SPACE" ;;
        '!') output="$output &kp EXCL" ;;
        '@') output="$output &kp AT" ;;
        '#') output="$output &kp HASH" ;;
        '$') output="$output &kp DLLR" ;;
        '%') output="$output &kp PRCNT" ;;
        '^') output="$output &kp CARET" ;;
        '&') output="$output &kp AMPS" ;;
        '*') output="$output &kp ASTRK" ;;
        '(') output="$output &kp LPAR" ;;
        ')') output="$output &kp RPAR" ;;
        '-') output="$output &kp MINUS" ;;
        '_') output="$output &kp UNDER" ;;
        '=') output="$output &kp EQUAL" ;;
        '+') output="$output &kp PLUS" ;;
        '[') output="$output &kp LBKT" ;;
        ']') output="$output &kp RBKT" ;;
        '{') output="$output &kp LBRC" ;;
        '}') output="$output &kp RBRC" ;;
        '\') output="$output &kp BSLH" ;;
        '|') output="$output &kp PIPE" ;;
        ';') output="$output &kp SEMI" ;;
        ':') output="$output &kp COLON" ;;
        "'") output="$output &kp SQT" ;;
        '"') output="$output &kp DQT" ;;
        ',') output="$output &kp COMMA" ;;
        '<') output="$output &kp LT" ;;
        '.') output="$output &kp DOT" ;;
        '>') output="$output &kp GT" ;;
        '/') output="$output &kp FSLH" ;;
        '?') output="$output &kp QMARK" ;;
        '`') output="$output &kp GRAVE" ;;
        '~') output="$output &kp TILDE" ;;
        
        *)
            echo "Warning: Unsupported character '$char' (skipping)" >&2
            ;;
    esac
done

# Remove leading space and output
echo "${output# }"
```

Make it executable:
```bash
chmod +x .github/workflows/string_to_zmk.sh
```

### Step 2: Replace Your Build Workflow

Replace your `.github/workflows/build.yml` with this basic secret macro workflow:

```yaml
name: Build ZMK firmware

on: [push, pull_request, workflow_dispatch]

jobs:
  build:
    runs-on: ubuntu-latest
    container:
      image: zmkfirmware/zmk-build-arm:stable
    name: Build
    steps:
      - name: Checkout
        uses: actions/checkout@v4

      - name: Cache West modules
        uses: actions/cache@v4
        env:
          cache-name: cache-zephyr-modules
        with:
          path: |
            modules/
            tools/
            zephyr/
            bootloader/
          key: ${{ runner.os }}-build-${{ env.cache-name }}-${{ hashFiles('config/west.yml') }}
          restore-keys: |
            ${{ runner.os }}-build-${{ env.cache-name }}-
            ${{ runner.os }}-build-
            ${{ runner.os }}-

      - name: West Init
        run: west init -l config

      - name: West Update
        run: west update

      - name: West Zephyr export
        run: west zephyr-export

      - name: Generate secret macro overlays
        run: |
          echo "Generating secret macro overlays..."
          
          mkdir -p config
          chmod +x .github/workflows/string_to_zmk.sh
          
          # Function to generate a macro
          generate_macro() {
            local secret_value="$1"
            local macro_name="$2"
            local output_file="$3"
            local fallback_text="$4"
            
            if [ -z "$secret_value" ]; then
              echo "Warning: $macro_name secret not set, creating fallback"
              BINDINGS=$(.github/workflows/string_to_zmk.sh "$fallback_text" 2>/dev/null)
            else
              echo "$macro_name secret found, converting to bindings"
              if echo "$secret_value" | grep -q '^&'; then
                BINDINGS="$secret_value"
              else
                BINDINGS=$(.github/workflows/string_to_zmk.sh "$secret_value" 2>/dev/null)
                if [ $? -ne 0 ] || [ -z "$BINDINGS" ]; then
                  echo "Error: Failed to convert $macro_name"
                  exit 1
                fi
              fi
            fi
            
            cat > "$output_file" << EOF
          behaviors {
              $macro_name: $macro_name {
                  compatible = "zmk,behavior-macro";
                  #binding-cells = <0>;
                  bindings = <$BINDINGS>;
                  wait-ms = <10>;
                  tap-ms = <10>;
              };
          };
          EOF
            
            if [ -f "$output_file" ]; then
              echo "$macro_name overlay created successfully"
            else
              echo "Error: Failed to create $output_file"
              exit 1
            fi
          }
          
          # Generate both secret macros
          generate_macro "${{ secrets.STRING_PLACEHOLDER }}" "secret_macro" "config/secret_macro.overlay" "SECRET"
          generate_macro "${{ secrets.STRING_PLACEHOLDER2 }}" "secret_macro2" "config/secret_macro2.overlay" "SECRET2"
        env:
          STRING_PLACEHOLDER: ${{ secrets.STRING_PLACEHOLDER }}
          STRING_PLACEHOLDER2: ${{ secrets.STRING_PLACEHOLDER2 }}

      - name: West Build (Corne Left)
        run: west build -s zmk/app -b nice_nano_v2 -- -DSHIELD=corne_left -DZMK_CONFIG="${GITHUB_WORKSPACE}/config"

      - name: Rename Corne Left artifact
        run: cp build/zephyr/zmk.uf2 corne_left-nice_nano_v2-zmk.uf2

      - name: West Build (Corne Right)
        run: west build -s zmk/app -b nice_nano_v2 -p -- -DSHIELD=corne_right -DZMK_CONFIG="${GITHUB_WORKSPACE}/config"

      - name: Rename Corne Right artifact
        run: cp build/zephyr/zmk.uf2 corne_right-nice_nano_v2-zmk.uf2

      - name: Upload artifacts
        uses: actions/upload-artifact@v4
        with:
          name: firmware
          path: |
            corne_left-nice_nano_v2-zmk.uf2
            corne_right-nice_nano_v2-zmk.uf2
```

**Important**: Replace `corne_left`/`corne_right` and `nice_nano_v2` with your actual keyboard shield and board names.

### Step 3: Configure Repository Secrets

1. Go to your repository **Settings** → **Secrets and variables** → **Actions**
2. Click **New repository secret**
3. Add your secrets:
   - Name: `STRING_PLACEHOLDER`, Value: Your first secret (e.g., `MyPassword123`)
   - Name: `STRING_PLACEHOLDER2`, Value: Your second secret (e.g., `user@email.com`)

### Step 4: Update Your Keymap

Add the include statements and use the macros in your `config/*.keymap` file:

```c
#include <behaviors.dtsi>
#include <dt-bindings/zmk/keys.h>

// Include the secret macro overlays
#include "secret_macro.overlay"
#include "secret_macro2.overlay"

/ {
    keymap {
        compatible = "zmk,keymap";
        
        default_layer {
            bindings = <
                // Example: add secret macros to your layout
                &kp Q &kp W &kp E &kp R &kp T
                &secret_macro &secret_macro2 &kp D &kp F &kp G
                // ... rest of your layout
            >;
        };
    };
};
```

## Supported Characters

The converter supports:
- **Lowercase letters**: a-z
- **Uppercase letters**: A-Z (automatically adds shift)
- **Numbers**: 0-9
- **Space**: (space character)
- **Special characters**: ! @ # $ % ^ & * ( ) - _ = + [ ] { } \ | ; : ' " , < . > / ? ` ~

Unsupported characters will generate a warning and be skipped during conversion.

## Troubleshooting

### Build fails with "secret_macro not found"

Make sure you've included the overlay in your keymap:
```c
#include "secret_macro.overlay"
```

### Workflow runs but uses fallback text

Check that your repository secret is correctly named (`STRING_PLACEHOLDER` or `STRING_PLACEHOLDER2`) and has a value set.

### Special characters not working

Verify the character is in the supported list above. Some characters may require different ZMK keycodes depending on your base keymap layout.

### Build works locally but fails in Actions

Ensure the `string_to_zmk.sh` script is committed to your repository and the path is correct in the workflow file.

## Important Notes

- The secret macro is only available in GitHub Actions builds
- Local builds will fail if they reference `&secret_macro` without the overlay
- Consider using conditional compilation if you need local build compatibility

## Security considerations
Although not fully secure—someone with physical access to the board could potentially extract the secret—doing so requires knowing the board layout and is close to impossible to bruteforce if the layout is unknown to the attacker. Evaluate your risks.

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