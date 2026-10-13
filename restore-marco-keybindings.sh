#!/bin/sh

# if ran disable-macro-keybindings.sh, need to run this script because that one and this one supposedly persists through power ups.  Would only need to run this script if not using mate-window-snap.

set -eu

schema=org.mate.Marco.window-keybindings

gsettings set "$schema" move-to-corner-ne '<Mod4>KP_9'
gsettings set "$schema" move-to-corner-nw '<Mod4>KP_7'
gsettings set "$schema" move-to-corner-se '<Mod4>KP_3'
gsettings set "$schema" move-to-corner-sw '<Mod4>KP_1'
gsettings set "$schema" move-to-side-e '<Mod4>KP_6'
gsettings set "$schema" move-to-side-w '<Mod4>KP_4'
gsettings set "$schema" tile-to-corner-ne '<Mod4>KP_9'
gsettings set "$schema" tile-to-corner-nw '<Mod4>KP_7'
gsettings set "$schema" tile-to-corner-se '<Mod4>KP_3'
gsettings set "$schema" tile-to-corner-sw '<Mod4>KP_1'
gsettings set "$schema" tile-to-side-e '<Mod4>KP_6'
gsettings set "$schema" tile-to-side-w '<Mod4>KP_4'

printf '%s\n' 'Marco keypad bindings restored.'