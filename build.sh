#!/usr/bin/sh

clang -O2 -fno-builtin --target=wasm32 --no-standard-libraries -Wl,--no-entry -Wl,--export-all -Wl,--export=__head_base -Wl,--allow-undefined -o ./wasm/wudoku.wasm src/sudoku.c

