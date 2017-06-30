#!/bin/bash

#set -x
set -e
set -u

param1=${1:-}

YELLOW='\033[1;33m'
NOCOLOR='\033[0m'

echo -e "${YELLOW}Building with gcc/g++...${NOCOLOR}"
make -C KissFFT/examples clean all CC=gcc   CXX=g++
make -C KissFFT/examples clean all CC=clang CXX=clang++

echo -e "${YELLOW}Building with clang/clang++...${NOCOLOR}"
make -C src clean all CC=gcc   CXX=g++
make -C src clean all CC=clang CXX=clang++

echo -e "${YELLOW}Checking C/C++ source code...${NOCOLOR}"
find ./KissFFT/examples/ ./src \( -name "*.c" -o -name "*.cc" \) -exec cppcheck \
    --enable=all \
    --suppress=missingIncludeSystem \
    --check-config \
    -I src/include \
    -I KissFFT/kiss_fft130/ \
    -I KissFFT/kiss_fft130/tools/ {} +

if [ "$param1" == "--run" ]; then
    echo -e "${YELLOW}Running generate_A440${NOCOLOR}"
    bin/generate_A440
    echo -e "${YELLOW}Running generate_sine_wave${NOCOLOR}"
    bin/generate_sine_wave --frequency 440 --sample-rate 48000 --audio-format pcm32 --prefix-name sine --duration 0.00909090 --verbose
    echo -e "${YELLOW}Running upsampling_algorithm_short_sequence${NOCOLOR}"
    bin/upsampling_algorithm_short_sequence octave/sine_48000_pcm32_short.wav
    echo -e "${YELLOW}Running upsampling_algorithm_long_sequence${NOCOLOR}"
    bin/upsampling_algorithm_long_sequence octave/sine_48000_pcm32_long.wav
    echo -e "${YELLOW}Running upsampling_algorithm_long_sequence_real_fft${NOCOLOR}"
    bin/upsampling_algorithm_long_sequence_real_fft octave/sine_48000_pcm32_long.wav
    echo -e "${YELLOW}Running upsampling2x${NOCOLOR}"
    bin/upsampling2x octave/sine_48000_pcm32_long.wav
fi
