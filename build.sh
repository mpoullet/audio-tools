#!/bin/sh

set -e
set -u

# make_sine
g++     -std=c++14 -Wall -Wextra -Werror -pedantic -o make_sine make_sine.cc -lsndfile -lm
clang++ -std=c++14 -Wall -Wextra -Werror -pedantic -o make_sine make_sine.cc -lsndfile -lm

# commmand_line
g++     -std=c++14 -Wall -Wextra -Werror -pedantic -o command_line command_line.cc -lboost_program_options -lboost_system -lboost_filesystem -lsndfile -lm
clang++ -std=c++14 -Wall -Wextra -Werror -pedantic -o command_line command_line.cc -lboost_program_options -lboost_system -lboost_filesystem -lsndfile -lm

# block_processing and test_fft

## GNU
## Step 1: build Kiss FFT object files
gcc -std=c11 -Wall -Wextra -Werror -pedantic -I kiss_fft130/                      -c kiss_fft130/kiss_fft.c
gcc -std=c11 -Wall -Wextra -Werror -pedantic -I kiss_fft130/ -I kiss_fft130/tools -c kiss_fft130/tools/kiss_fftr.c
# Step 2: build block_processing
g++ -std=c++14 -Wall -Wextra -Werror -pedantic -I kiss_fft130/ -I kiss_fft130/tools/ -o block_processing kiss_fft.o kiss_fftr.o block_processing.cc -lsndfile
# Step 3: build test_fft
g++ -std=c++14 -Wall -Wextra -Werror -pedantic -I kiss_fft130/ -I kiss_fft130/tools/ -o test_fft kiss_fft.o kiss_fftr.o test_fft.cc -lm

## Clang
## Step 1: build Kiss FFT object files
clang -std=c11 -Wall -Wextra -Werror -pedantic -I kiss_fft130/                      -c kiss_fft130/kiss_fft.c
clang -std=c11 -Wall -Wextra -Werror -pedantic -I kiss_fft130/ -I kiss_fft130/tools -c kiss_fft130/tools/kiss_fftr.c
# Step 2: build block_processing
clang++ -std=c++14 -Wall -Wextra -Werror -pedantic -I kiss_fft130/ -I kiss_fft130/tools/ -o block_processing kiss_fft.o kiss_fftr.o block_processing.cc -lsndfile
# Step 3: build test_fft
clang++ -std=c++14 -Wall -Wextra -Werror -pedantic -I kiss_fft130/ -I kiss_fft130/tools/ -o test_fft kiss_fft.o kiss_fftr.o test_fft.cc -lm

# cppcheck
cppcheck --enable=all --suppress=missingIncludeSystem *.cc *.c

echo "Running..."
#./make_sine
./command_line --frequency 440 --sample-rate 48000 --audio-format pcm32 --prefix-name sine --duration 0.00909090 --verbose
./block_processing
