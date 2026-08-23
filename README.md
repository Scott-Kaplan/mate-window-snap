# MATE Snap WM

`mate-snap-wm` is a deliberately small X11 utility for Ubuntu MATE 24.04. It
runs alongside Marco and provides keyboard snapping for the focused application
window, including dialogs. It uses RandR to discover active monitors and EWMH
work-area data so MATE panels remain visible.

## Dependencies

Build dependencies on Ubuntu are `build-essential`, `pkg-config`, `libx11-dev`,
and `libxrandr-dev`.

## Build

```sh
make
```

The install target honors `PREFIX` and `DESTDIR`, which makes it suitable for a
local prefix or a distribution package. It installs the executable and a MATE
autostart entry, so it starts automatically at login.

```sh
make install PREFIX=/usr/local
```

Log out and back in after installation to start the utility. It leaves Marco as
MATE's window manager and communicates window moves through EWMH.

## Key Bindings

All shortcuts require the Super key and a numeric keypad key with NumLock on.

| Shortcut | Result |
| --- | --- |
| Super+KP7 | Top-left quarter of the current monitor |
| Super+KP4 | Left half; from the left half of a right monitor, moves to the right edge of the monitor on the left while retaining the current size |
| Super+KP1 | Bottom-left quarter of the current monitor |
| Super+KP5 | Usable work area of the current monitor |
| Super+KP9 | Top-right quarter of the current monitor |
| Super+KP6 | Right half; from the right half of a left monitor, moves to the left edge of the monitor on the right while retaining the current size |
| Super+KP3 | Bottom-right quarter of the current monitor |

The monitor list is sorted left to right. With one monitor, repeated KP4 or KP6
remains on that monitor. The implementation assumes the monitors are arranged
side by side, as requested.

## Scope

This first version intentionally has no mouse move/resize, workspace controls,
client rules, gaps, or configuration parser. The window manager retains control
of decorations, focus, and normal window behavior.