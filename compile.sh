#!/bin/bash
cd src
clang++ -std=c++11 fastgenematch.h fastgenematch.cpp utils.h MurmurHash3.h MurmurHash3.cpp main.cpp --static -O2
mv a.out ../fgc.exe
cd ..
