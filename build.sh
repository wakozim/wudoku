#!/usr/bin/sh

set -xe

mkdir -p ./build/
mkdir -p ./wasm/

CFLAGS="-O3 -Wall -Wextra -g -pedantic  `pkg-config --libs raylib`"
CLIBS="`pkg-config --libs raylib` -lm"

clang $CFLAGS -o ./build/wudoku ./src/main.c $CLIBS
clang -O3 -fno-builtin --target=wasm32 --no-standard-libraries -Wl,--no-entry -Wl,--export-all -Wl,--export=__head_base -Wl,--allow-undefined -o ./wasm/wudoku.wasm src/sudoku.c -DPLATFORM_WEB
