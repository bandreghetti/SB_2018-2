#!/bin/bash

mkdir -p build

cd build
cmake ..
make && \
mv montador.out ../ && \
mv ligador.out ../
