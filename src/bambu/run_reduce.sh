#!/bin/bash

set -ex

cp -r /home/ymherklotz/projects/hls_fuzzing/bambu/failed/3_reduce/main_gcc.c . || true
cp -r /home/ymherklotz/projects/hls_fuzzing/bambu/failed/3_reduce/main_bambu.c . || true

bambu main_bambu.c >bambu.log 2>&1
iverilog top.v /home/ymherklotz/projects/hls_fuzzing/bambu/failed/3_reduce/tb.v -o top
./top >out.iverilog.txt

gcc -Werror -Wall -Wextra \
    -Wno-unused-function -Wno-unused-variable -Wno-unused-parameter \
    -Wno-unused-but-set-variable -Wno-unused-but-set-parameter -Wno-sign-compare -Wno-unused-value \
    -Wno-type-limits -Wno-tautological-compare -Wno-bool-compare -fsanitize=undefined\
    main_gcc.c -o test
timeout 1s ./test >out.gcc.txt 2>out.gcc.err.txt
test ! -s out.gcc.err.txt

if [[ $(wc -l <out.iverilog.txt) -ne 1 ]]; then exit 1; fi
if [[ $(wc -l <out.gcc.txt) -ne 1 ]]; then exit 1; fi

grep -E "^checksum = [a-fA-F0-9]{8}$" out.gcc.txt >/dev/null
grep -E "^checksum = [a-fA-F0-9]{8}$" out.iverilog.txt >/dev/null

cat out.iverilog.txt | tr 'A-Z' 'a-z' >out.iverilog.lw.txt
cat out.gcc.txt | tr 'A-Z' 'a-z' >out.gcc.lw.txt

diff out.iverilog.lw.txt out.gcc.lw.txt && exit 1 || exit 0
