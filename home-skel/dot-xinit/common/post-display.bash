#! /usr/bin/env bash

~/sys-setup/x-map-caps2esc.bash
xrdb -load ~/.Xresources

sleep 1s

term-dark  &
term-light &
google-chrome &

xcompmgr -CcF     \
         -I -.015 \
         -O -.03  \
         -D 1 &> /dev/null &

xsetroot -cursor_name left_ptr

exec xmonad

