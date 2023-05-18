#!/bin/bash

echo "cleaning workstation..."
rm -rf work
mkdir -p failed
max=5000

for i in $(seq 1 $max); do
    printf "\rrun %4d/%4d: ..." $i $max
    cp -r "tests/$i" work
    cp template.v work/tb.v
    cp run_single.sh work/run_single.sh

    cd work

    ./run_single.sh
    result=$?

    if [[ "$result" -eq "0" ]]; then
        cd ..
        rm -rf work
        printf "\rrun %4d/%4d: pass" $i $max
    else
        cd ..
        mv work "failed/$i"
        printf "\rrun %4d/%4d: failed\n" $i $max
    fi
done

echo ""
