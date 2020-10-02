#!/bin/sh

set -xe

sed -i -E -e 's:#include "csmith\.h":#include "/home/vagrant/projects/csmith/runtime/csmith.h":' \
          -e 's:return 0;:return crc32_context ^ 0xFFFFFFFFUL;:' \
          -e 's:int main \(void\):int result (void):' \
          -e 's:goto [^ ]+;:;:' \
          -e '/^int result \(void\)/i #pragma hls_design top' test.cpp
echo "int main() { result(); }" >> test.cpp
gcc test.cpp -o test_gcc
./test_gcc >out.gold.txt
catapult -shell -file run.tcl
cp Catapult/result.v1/concat_sim_rtl.v result.v
verilator -Wno-fatal --cc result.v
make -j4
./sim_main >out.sim.txt
diff out.gold.txt out.sim.txt
