#! /usr/bin/env bash

(
    xrandr --auto

    IFS=':'
    for output in $OUTPUTS ; do
        xrandr --output "$output" --off
    done

    xrandr_command=xrandr
    last_mode="--primary"
    last_output=""
    for output in $OUTPUTS ; do
        xrandr_command="$xrandr_command --output \"$output\""
        xrandr_command="$xrandr_command --auto $last_mode $last_output"
        xrandr_command="$xrandr_command --scale 0.9999x0.9999"
        last_mode="--right-of"
        last_output="$output"
    done

    eval "$xrandr_command"
)

