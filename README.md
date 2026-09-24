# MATE Window Snap

`mate-window-snap` is a deliberately small X11 utility for Ubuntu MATE 24.04. It
runs alongside Marco and provides keyboard snapping for the focused application
window, including dialogs. It uses RandR to discover active monitors and EWMH
work-area data so MATE panels remain visible.

Contributions are welcome within the project's Ubuntu MATE/X11 scope. Please
read [CONTRIBUTING.md](CONTRIBUTING.md) before opening an issue or pull request.
See [CODE_OF_CONDUCT.md](CODE_OF_CONDUCT.md) for community expectations.

## Compatibility

`mate-window-snap` is tested and supported on **Ubuntu MATE 24.04** using the
**MATE desktop** with the **Marco** window manager in an **X11** session. It
requires an X11 server with the RandR extension and a window manager that
supports the EWMH `_NET_ACTIVE_WINDOW`, `_NET_WORKAREA`, and
`_NET_MOVERESIZE_WINDOW` properties.

It supports one monitor or two monitors arranged side by side. The cross-monitor
shortcuts assume the monitors are ordered from left to right.

Wayland sessions, other desktop environments, other window managers, and other
Linux distributions are not currently supported or tested.

## Dependencies

Build dependencies on Ubuntu are `build-essential`, `pkg-config`, `libx11-dev`,
and `libxrandr-dev`. Git is needed to download the source repository.

## Build And Install

Run each command in order:

1. Go to your home directory (or another directory if you prefer):

```sh
cd ~
```

2. Install Git and the build dependencies:

```sh
sudo apt update
sudo apt install git build-essential pkg-config libx11-dev libxrandr-dev
```

3. Clone the project and enter its directory:

```sh
git clone https://github.com/Scott-Kaplan/mate-window-snap.git
cd mate-window-snap
```

All remaining build and installation commands in this section must be run from
the cloned `mate-window-snap` directory.

4. Build the executable:

```sh
make
```

5. Test:

```sh
mate-window-snap
```
... and then press one of the sequences found in the Key Bindings at the bottom of this page.  You should then see this terminal window snap to the respective location.

## Automatic Startup
These steps will automatically activate mate-window-snap when you login

1. This next command installs the executable into `/usr/local/bin` and its system-wide MATE
autostart entry into `/etc/xdg/autostart`:

```sh
sudo make install
```

2. Log out of the MATE desktop and log back in. The desktop session then starts
`mate-window-snap` automatically after every future MATE login.  You should see all key bindings work as soon as you login

## Uninstall

Start in a terminal opened in the directory where the repository was cloned.
For example, if it was cloned into the home directory:

```sh
cd ~/mate-window-snap
```

Remove the executable and system-wide autostart entry:

```sh
sudo make uninstall
```

The utility will no longer start at future MATE logins. Any instance already
running in the current session continues until it is stopped or the user logs
out.

The install target honors `PREFIX`, `SYSCONFDIR`, and `DESTDIR`, which makes it
suitable for a different system prefix or a distribution package.

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