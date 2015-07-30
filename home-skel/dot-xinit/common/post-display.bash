#! /usr/bin/env bash

~/sys-setup/x-map-caps2esc.bash
xrdb -load ~/.Xresources

sleep 1s

term-dark  &
term-light &

xcompmgr -CcfF      \
         -r'0.010'  \
         -I'-0.015' \
         -O'-0.030' \
         -D'1'      &> /dev/null &

xsetroot -cursor_name left_ptr

exec xmonad

killall google-chrome &> /dev/null

