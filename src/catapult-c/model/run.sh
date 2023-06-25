#!/bin/sh

set -xe

sed -i -E -e 's:#include "csmith\.h":#include "/home/ymherklotz/projects/csmith/runtime/csmith.h":' \
          -e 's:return 0;:return crc32_context ^ 0xFFFFFFFFUL;:' \
          -e 's:int main \(void\):int result (void):' \
          -e 's:goto [^ ]+;:;:' \
          -e '/^int result \(void\)/i #pragma hls_design top' test.cpp
echo "int main() { result(); }" >> test.cpp
gcc -Wno-narrowing test.cpp -o test_gcc
./test_gcc >out.gold.txt
catapult -shell -file run.tcl
cp Catapult/result.v1/concat_sim_rtl.v result.v
#verilator --cc --exe --build -j 7 -Wno-fatal --exe sim_main.cpp result.v
#./obj_dir/Vresult >out.sim.txt
iverilog -o out.ver result.v tb.v
./out.ver >out.sim.txt
diff out.gold.txt out.sim.txt
