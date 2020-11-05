#!/bin/bash

set -e
gcc -g -shared -fPIC mem-trace.c -o libMemTrace.so -ldl
g++ -std=c++14 -g main.cc -o main -L. -lMemTrace -Wl,-rpath .

LD_PRELOAD=./libMemTrace.so ./main
# gdb -ex "set environment LD_PRELOAD ./libMemTrace.so" ./main
