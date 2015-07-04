#! /usr/bin/env bash

xrandr --output DP2-1 --auto --primary        --scale 1.0001x1.0001 \
       --output DP2-2 --auto --right-of DP2-1 --scale 1.0001x1.0001 \
       --output eDP1 --off

source "$HOME/.x/common/reset-wallpaper.bash"

