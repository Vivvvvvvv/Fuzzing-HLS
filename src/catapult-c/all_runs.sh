#!/bin/bash

mkdir -p failed

seq 1 10000 | xargs -P8 -n1 ./all_run_bis.sh
