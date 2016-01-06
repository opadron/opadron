#! /usr/bin/env bash

script_dir="$( readlink -e "$( dirname "$0" )" )"
gconftool-2 --load "$script_dir/gnome-terminal.conf.xml"

