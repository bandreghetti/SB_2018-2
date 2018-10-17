#!/bin/bash

declare -a COMPONENTS=("montador")

CCFLAGS="-O2 -std=c++14"

for comp in "${COMPONENTS[@]}"
do
    g++ ${comp}.cpp -o ${comp} ${CCFLAGS} -g
done
