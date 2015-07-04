#! /usr/bin/env bash

xrandr --output DP2-1 --auto --primary        --scale 0.9999x0.9999 \
       --output DP2-2 --auto --right-of DP2-1 --scale 0.9999x0.9999 \
       --output eDP1 --off

source "$HOME/.x/common/reset-wallpaper.bash"

