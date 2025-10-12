# Secret Macro Injection Feature

You can now include sensitive macros in your firmware builds via repository secrets without exposing them in your public repository.

## How It Works

The build system automatically generates a `secret_macro` behavior during GitHub Actions builds using repository secrets. This allows you to include passwords, API keys, personal shortcuts, or other sensitive key sequences without committing them to your public codebase.

### Technical Implementation

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

Add the secret macro to any key position in your keymap:

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
