#!/bin/sh

set -xe

cp /mnt/orca/projects/fuzzing-hls/src/catapult-c/failed/catapult_failed_00086/sim_main.cpp . || true
cp /mnt/orca/projects/fuzzing-hls/src/catapult-c/failed/catapult_failed_00086/tb.v . || true
cp /mnt/orca/projects/fuzzing-hls/src/catapult-c/failed/catapult_failed_00086/run.tcl . || true
cp /mnt/orca/projects/fuzzing-hls/src/catapult-c/failed/catapult_failed_00086/main.cpp . || true

gcc -Wall -Werror -Wno-unused-function -Wno-unused-variable -Wno-write-strings -Wno-narrowing -Wno-unused-but-set-variable -Wno-unused-value -Wno-unknown-pragmas main.cpp -o test_gcc

./test_gcc >out.gold.txt

catapult -shell -file run.tcl

cp Catapult/result.v1/concat_sim_rtl.v result.v

#verilator --cc --exe --build -j 7 -Wno-fatal --exe sim_main.cpp result.v
#./obj_dir/Vresult >out.sim.txt

iverilog -o out.ver result.v tb.v

./out.ver >out.sim.txt

grep -E '^checksum = [0-9a-f]*$' out.sim.txt
grep -E '^checksum = [0-9a-f]*$' out.gold.txt

WC1=$(wc -l out.sim.txt | sed 's/\([0-9]*\).*/\1/')
WC2=$(wc -l out.gold.txt | sed 's/\([0-9]*\).*/\1/')

[[ "$WC1" -eq "1" ]]
[[ "$WC2" -eq "1" ]]

if diff out.gold.txt out.sim.txt; then
    exit 1
else
    exit 0
fi
