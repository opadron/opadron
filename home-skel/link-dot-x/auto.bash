#! /usr/bin/env bash

(
    connected_output_list=($( xrandr | grep ' connected ' | awk '{ print $1 }' ))
    all_output_list=($( xrandr | awk \
        '$1!~/[0-9]+x[0-9]+/ && $1!~/Screen/{ print $1 }' ))
    n="${#connected_output_list[@]}"

    if [ -n "$SELECTIONS" ] ; then
        selections=($SELECTIONS)
    else
        selections=($( seq 0 $(( n-1 )) ))
    fi

    n="${#selections[@]}"

    for ((i=0; i<n; ++i)) ; do
        index="${selections[$i]}"
        if [ -z "$OUTPUTS" ] ; then
            OUTPUTS="${connected_output_list[$index]}"
        else
            OUTPUTS="$OUTPUTS:${connected_output_list[$index]}"
        fi
    done

    n2="${#all_output_list[@]}"
    for ((i=0; i<n2; ++i)) ; do
        xrandr --output "${all_output_list[$i]}" --off
    done

    export OUTPUTS
    source "$HOME/.x/common/auto-screen.bash"
)

source "$HOME/.x/common/reset-wallpaper.bash"

