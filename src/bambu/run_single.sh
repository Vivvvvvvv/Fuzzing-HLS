#!/bin/bash

sed -i -e 's:"csmith\.h":"/home/ymherklotz/projects/csmith/runtime/csmith.h":' test.c
sed -i -e 's:return 0;:return crc32_context ^ 0xFFFFFFFFUL;:' test.c
bambu test.c >bambu.log 2>&1
iverilog top.v tb.v -o top
./top >out.iverilog.txt

gcc test.c -o test
./test >out.gcc.txt

cat out.iverilog.txt | tr 'A-Z' 'a-z' >out.iverilog.lw.txt
cat out.gcc.txt | tr 'A-Z' 'a-z' >out.gcc.lw.txt

diff out.iverilog.lw.txt out.gcc.lw.txt
