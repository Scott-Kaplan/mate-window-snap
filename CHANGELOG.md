# Changelog

Below are the changes that have been made to `mate-window-snap`.

## Version 1.0.12

10/12/2026

### Fixed

- Fit cross-monitor moves inside the destination's usable area so Marco no longer pushes the window back.
- Removed the direct X11 move fallback added in 1.0.10.

## Version 1.0.11

10/12/2026

### Changed

- Debug mode also appends to `/tmp/mate-window-snap.log` and logs ignored key events.

## Version 1.0.10

10/12/2026

### Fixed

- Apply a direct X11 move/resize fallback for cross-monitor transfers.

## Version 1.0.9

10/12/2026

### Fixed

- Keep cross-monitor resize requests within the destination monitor while preserving window size.

## Version 1.0.8

10/12/2026

### Added

- Opt-in terminal diagnostics for key handling and monitor-crossing decisions.

## Version 1.0.7

10/12/2026

### Fixed

- Expand snap requests by client-side frame extents so the visible window fills the target.

## Version 1.0.6

10/12/2026

### Changed

- Snap windows to full monitor bounds instead of excluding panel work areas.

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