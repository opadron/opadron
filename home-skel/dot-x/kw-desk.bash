#! /usr/bin/env bash

xrandr --output eDP1  --auto --primary        --scale 0.9999x0.9999 \
       --output DP2-3 --auto --right-of eDP1  --scale 0.9999x0.9999 \
       --output DP2-2 --auto --right-of DP2-3 --scale 0.9999x0.9999

source "$HOME/.x/common/reset-wallpaper.bash"

