#!/bin/bash

#set -x
set -e
set -u

param1=${1:-}

YELLOW='\033[1;33m'
NOCOLOR='\033[0m'

echo -e "${YELLOW}Building with gcc/g++...${NOCOLOR}"
CC=gcc   CXX=g++     make -C KissFFT/examples clean all
CC=clang CXX=clang++ make -C KissFFT/examples clean all

echo -e "${YELLOW}Building with clang/clang++...${NOCOLOR}"
CC=gcc   CXX=g++     make -C src clean all
CC=clang CXX=clang++ make -C src clean all

echo -e "${YELLOW}Checking C/C++ source code...${NOCOLOR}"
find ./KissFFT/examples/ ./src \( -name "*.c" -o -name "*.cc" \) -exec cppcheck \
    --enable=all \
    --suppress=missingIncludeSystem \
    --check-config \
    -I KissFFT/kiss_fft130/ \
    -I KissFFT/kiss_fft130/tools/ {} +

if [ "$param1" == "--run" ]; then
    echo -e "${YELLOW}Running...${NOCOLOR}"
    #bin/make_sine
    bin/command_line --frequency 440 --sample-rate 48000 --audio-format pcm32 --prefix-name sine --duration 0.00909090 --verbose
    bin/block_processing
fi
