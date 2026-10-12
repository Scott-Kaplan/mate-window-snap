# Changelog

Below are the changes that have been made to `mate-window-snap`.

## Version 1.0.5

10/12/2026

### Added

- Scripts to disable and restore Marco's conflicting keypad shortcuts.

## Version 1.0.4

10/6/2026

### Fixed

- Removed the GTK frame-extents adjustment that reduced the requested snap size.

## Version 1.0.3

10/5/2026

### Fixed

- Account for advertised window-frame extents when snapping so windows align with the requested bounds.

## Version 1.0.2

10/3/2026

### Fixed

- Removed the duplicate direct X resize request so Marco applies the requested window geometry.

## Version 1.0.1

9/29/2026

### Added

- Keyboard snapping for the focused window with Super+numeric keypad shortcuts.
- Side-by-side multi-monitor snapping and work-area detection that keeps MATE panels visible.
- `--version` option for identifying the installed version.