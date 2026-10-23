#!/bin/sh

# The setting changes made by this script persists through power ups

set -eu

schema=org.mate.Marco.window-keybindings

for key in \
    move-to-corner-ne move-to-corner-nw move-to-corner-se move-to-corner-sw \
    move-to-side-e move-to-side-w \
    tile-to-corner-ne tile-to-corner-nw tile-to-corner-se tile-to-corner-sw \
    tile-to-side-e tile-to-side-w
do
    gsettings set "$schema" "$key" disabled
done

printf '%s\n' 'Marco keypad bindings disabled.'