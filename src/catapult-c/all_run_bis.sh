#!/bin/bash

i=$1

cp -r model run_$i
#cp testcases/$i/outG.txt run_$i/.
cp testcases/$i/test.c run_$i/test.cpp
cd run_$i
./run.sh >run_$i.log 2>&1
if [[ "$?" -eq "0" ]]; then
    echo -n "RUN $i: "
    echo "PASSED"
    cd ..
    rm -rf run_$i
else
    echo -n "RUN $i: "
    echo "FAILED"
    cd ..
    cp -r run_$i failed/failed_$i
    rm -rf run_$i
fi
