# MATE Window Snap

`mate-window-snap` is a deliberately small X11 utility for Ubuntu MATE. It
runs alongside Marco and provides keyboard snapping for the focused application
window, including dialogs. It uses RandR to discover active monitors and EWMH
window data to snap against each monitor's usable area. Snapped windows stay
clear of MATE panels.

Contributions are welcome within the project's Ubuntu MATE/X11 scope. Please
read [CONTRIBUTING.md](CONTRIBUTING.md) before opening an issue or pull request.
See [CODE_OF_CONDUCT.md](CODE_OF_CONDUCT.md) for community expectations.

## Compatibility

`mate-window-snap` is tested and supported on **Ubuntu MATE** using the
**MATE desktop** with the **Marco** window manager in an **X11** session. It
requires an X11 server with the RandR extension and a window manager that
supports the EWMH `_NET_ACTIVE_WINDOW`, `_NET_CLIENT_LIST`,
`_NET_WM_STRUT_PARTIAL`, and `_NET_MOVERESIZE_WINDOW` properties.

It supports one monitor or two monitors arranged side by side.

Wayland sessions, other desktop environments, other window managers, and other
Linux distributions are not currently supported nor tested.

<!-- ## Optional for developers

To print snapping diagnostics to the terminal and append them to
`/tmp/mate-window-snap.log` while testing, run:

```sh
MATE_WINDOW_SNAP_DEBUG=1 ./mate-window-snap
``` -->

<!-- ## Dependencies

Build dependencies on Ubuntu are `build-essential`, `pkg-config`, `libx11-dev`,
and `libxrandr-dev`. Git is needed to download the source repository. -->

## Get the Project, Install Dependencies, Build and Run it

Install the project's dependencies
```sh
sudo apt install build-essential pkg-config libx11-dev libxrandr-dev
```
Move to the directory where you want to keep the project & clone it
```sh
mkdir -p ~/temp/github
cd ~/temp/github
git clone https://github.com/Scott-Kaplan/mate-window-snap.git
```
Disable existing Marco shortcuts
```sh
# Marco's default keypad shortcuts conflict with mate-window-snap.
# This command allows mate-window-snap to be able to receive the
# Key Binding shortcuts and act on them.
./disable-marco-keybindings.sh
# this command restores them: ./restore-marco-keybindings.sh
```
Build & run it
```sh
cd mate-window-snap
make
./mate-window-snap
```
... and then press one or more Key Binding shortcuts found at the bottom of this page.  You should then see whatever window you have selected snap to the respective location.

## List version
```sh
cd ~/temp/github/mate-window-snap
./mate-window-snap --version
```
Once it is installed, you can run `mate-window-snap --version` from any directory.

## Install for Automatic Startup
These steps will automatically activate mate-window-snap when you login:

1. This command installs the executable into `/usr/local/bin` and its system-wide MATE
autostart entry into `/etc/xdg/autostart`:

```sh
cd ~/temp/github/mate-window-snap
sudo make install
```

2. Log out of the MATE desktop and log back in. All Key Binding shortcuts should work.

## Uninstall Automatic Startup

These steps will remove mate-window-snap from your system's startup.

```sh
cd ~/temp/github/mate-window-snap
sudo make uninstall
```
The final step is just to log out or restart your computer and the Key Binding shortcuts below will be inactive.

## Key Bindings

All shortcuts require the Super key and a numeric keypad key with NumLock on.

| Shortcut | Result |
| --- | --- |
| Super+KP7 | Top-left quarter of the current monitor |
| Super+KP4 | Left half; from the left half of a right monitor, moves to the right edge of the monitor on the left while retaining the current size |
| Super+KP1 | Bottom-left quarter of the current monitor |
| Super+KP5 | Usable area of the current monitor |
| Super+KP9 | Top-right quarter of the current monitor |
| Super+KP6 | Right half; from the right half of a left monitor, moves to the left edge of the monitor on the right while retaining the current size |
| Super+KP3 | Bottom-right quarter of the current monitor |