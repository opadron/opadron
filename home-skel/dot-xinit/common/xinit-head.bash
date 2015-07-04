#! /usr/bin/env bash

if [ "$XINIT_HEAD_LOADED" '!=' '1' ] ; then
    export XINIT_HEAD_LOADED=1
    startx "$0" $*
    exit 0
fi

