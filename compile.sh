#!/bin/bash
cd src
clang++  -std=c++11 -I/home/yul13/bin/include -L/home/yul13/bin/lib fastgenematch.h fastgenematch.cpp utils.h MurmurHash3.h MurmurHash3.cpp fastgenematch_proc.h main_proc.cpp  -lkyotocabinet -lz -lstdc++ -lrt -lpthread -lm -lc -O3
mv a.out ../fgc_proc.exe
clang++  -std=c++11 -I/home/yul13/bin/include -L/home/yul13/bin/lib fastgenematch.h fastgenematch.cpp utils.h MurmurHash3.h MurmurHash3.cpp main.cpp  -lkyotocabinet -lz -lstdc++ -lrt -lpthread -lm -lc -O3
mv a.out ../fgc.exe
cd ..
