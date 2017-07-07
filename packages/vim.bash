#! /usr/bin/env bash

set -e

git submodule update --init --recursive vim

cd vim

make distclean || true

./configure --prefix=$HOME/.local/         \
            --with-features=huge           \
            --enable-luainterp=dynamic     \
            --enable-perlinterp=dynamic    \
            --enable-pythoninterp=dynamic  \
            --enable-python3interp=dynamic \
            --enable-tclinterp=dynamic     \
            --enable-rubyinterp=dynamic    \
            --enable-cscope                \
            --enable-terminal              \
            --enable-multibyte             \
            --disable-gui

make

make install

