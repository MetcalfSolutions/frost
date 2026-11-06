# Frost

Live opacity and blur control for Glassy windows in Omarchy.

Frost gives you instant, visual control over window transparency and frosted glass blur across all workspaces in Omarchy. Adjustments update your compositor in real time as you drag the sliders and automatically persist across reboots.

## Features

- **Window Opacity**: Dual sliders for focused (active) and unfocused (inactive) windows, with a sync lock to keep them matching.
- **Frosted Blur**: Sliders for blur radius (size) and depth (passes).
- **Fine Tuning**: Expandable controls for frosted noise/grain, contrast, brightness, and vibrancy.
- **Omarchy Native**: Follows your active Omarchy theme colors, accent, window rounding, and desktop language.
- **Automated Persistence**: Saves changes cleanly to `~/.config/hypr/looknfeel.lua`.
- **Keyboard First**: Every action is reachable from the keyboard.
- **CLI & Agent Friendly**: Complete command-line interface for headless control and automation scripts.

## Keyboard Shortcuts

| Key | Action |
| :--- | :--- |
| `R` | Reset all settings to Omarchy glass defaults |
| `?` | Toggle the keyboard shortcuts overlay |
| `Esc` | Close dialog or exit |
| `Q` | Quit Frost |

## Command-Line Usage

Frost can be driven directly from scripts, terminal shortcuts, or AI agents without opening a window:

```bash
# Print current settings in plain text or JSON
frost get
frost get --json

# Set active and inactive opacity
frost set --opacity 0.90
frost set --active-opacity 0.95 --inactive-opacity 0.85

# Adjust blur radius and passes
frost set --blur-size 6 --blur-passes 3

# Reset to Omarchy defaults
frost reset

# Launch the graphical interface
frost open
```

## Building and Testing

```bash
# Build binary
bin/build

# Run offscreen unit tests
bin/test

# Install as Arch package (adds to Omarchy launcher)
bin/install
```

## License

MIT License. See [LICENSE](LICENSE) for details.
