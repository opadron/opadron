#! /usr/bin/env bash

script_dir="$( readlink -e "$( dirname "$0" )" )"
xmodmap "$script_dir/caps-esc-x.txt"

