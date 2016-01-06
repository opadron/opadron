#! /usr/bin/env bash

script_dir="$( readlink -e "$( dirname "$0" )" )"
xmodmap "$script_dir/caps-esc-x.txt"

dconf write                                      \
    /org/gnome/desktop/input-sources/xkb-options \
    "['caps:escape']" &> /dev/null

setxkbmap -option 'caps:escape'

