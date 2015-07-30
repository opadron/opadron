#! /usr/bin/env bash

xrandr --auto

xrandr --output eDP1  --off \
       --output DP2-2 --off \
       --output DP2-3 --off

xrandr --output eDP1  --auto --primary        --scale 0.9999x0.9999
xrandr --output DP2-3 --auto --right-of eDP1  --scale 0.9999x0.9999
xrandr --output DP2-2 --auto --right-of DP2-3 --scale 0.9999x0.9999

source "$HOME/.x/common/reset-wallpaper.bash"

