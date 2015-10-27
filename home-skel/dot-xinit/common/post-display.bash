#! /usr/bin/env bash

~/sys-setup/x-map-caps2esc.bash
xrdb -load ~/.Xresources

sleep 1s

term-dark  &
term-light &

xsetroot -cursor_name left_ptr

exec xmonad

killall google-chrome &> /dev/null

