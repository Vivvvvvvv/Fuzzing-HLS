#!/bin/bash

for i in $(seq 1 10000); do
    echo -n "RUN $i: "
    cp -r model run_$i
    cp testcases/$i/outG.txt run_$i/.
    cp testcases/$i/test.c run_$i/test.cpp
    cd run_$i
    ./run.sh >run_$i.log 2>&1
    if [[ "$?" -eq "0" ]]; then
    echo "PASSED"
    cd ..
    rm -rf run_$i
    else
    echo "FAILED"
    cd ..
    cp -r run_$i failed/failed_$i
    rm -rf run_$i
    fi
done
