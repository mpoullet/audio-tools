#!/bin/sh

#set -x
set -e
set -u

echo "Building..."
CC=gcc CXX=g++       make -C KissFFT/examples clean all
CC=clang CXX=clang++ make -C KissFFT/examples clean all

CC=gcc CXX=g++       make -C src clean all
CC=clang CXX=clang++ make -C src clean all

echo "Running..."
#./make_sine
bin/command_line --frequency 440 --sample-rate 48000 --audio-format pcm32 --prefix-name sine --duration 0.00909090 --verbose
bin/block_processing
