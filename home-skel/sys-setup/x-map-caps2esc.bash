#! /usr/bin/env bash

dconf write                                      \
    /org/gnome/desktop/input-sources/xkb-options \
    "['caps:escape']" &> /dev/null

if [ "$?" '!=' '0' ] ; then
    script_dir="$( readlink -e "$( dirname "$0" )" )"
    xmodmap "$script_dir/caps-esc-x.txt"
fi

