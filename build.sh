#!/bin/sh

set -e
set -u

# make_sine
g++     -std=c++14 -Wall -Wextra -Werror -pedantic -o make_sine make_sine.cc -lsndfile -lm
clang++ -std=c++14 -Wall -Wextra -Werror -pedantic -o make_sine make_sine.cc -lsndfile -lm

# Kiss FFT examples
gcc   -std=c11 -o complex_valued_fwd_fft_floats -Wall -Wextra -Werror -pedantic -I kiss_fft130/                      complex_valued_fwd_fft_floats.c kiss_fft130/kiss_fft.c                               -lm
clang -std=c11 -o complex_valued_fwd_fft_floats -Wall -Wextra -Werror -pedantic -I kiss_fft130/                      complex_valued_fwd_fft_floats.c kiss_fft130/kiss_fft.c                               -lm
gcc   -std=c11 -o real_valued_fwd_fft_floats    -Wall -Wextra -Werror -pedantic -I kiss_fft130/tools/ -I kiss_fft130 real_valued_fwd_fft_floats.c    kiss_fft130/kiss_fft.c kiss_fft130/tools/kiss_fftr.c -lm
clang -std=c11 -o real_valued_fwd_fft_floats    -Wall -Wextra -Werror -pedantic -I kiss_fft130/tools/ -I kiss_fft130 real_valued_fwd_fft_floats.c    kiss_fft130/kiss_fft.c kiss_fft130/tools/kiss_fftr.c -lm

cppcheck --enable=all --suppress=missingIncludeSystem *.cc *.c

# commmand_line
g++     -std=c++14 -Wall -Wextra -Werror -pedantic -o command_line command_line.cc -lboost_program_options -lboost_system -lboost_filesystem -lsndfile -lm
clang++ -std=c++14 -Wall -Wextra -Werror -pedantic -o command_line command_line.cc -lboost_program_options -lboost_system -lboost_filesystem -lsndfile -lm

# block_processing

# GNU
# Step 1: build Kiss FFT object files
gcc -std=c11 -Wall -Wextra -Werror -pedantic -I kiss_fft130/                      -c kiss_fft130/kiss_fft.c
gcc -std=c11 -Wall -Wextra -Werror -pedantic -I kiss_fft130/ -I kiss_fft130/tools -c kiss_fft130/tools/kiss_fftr.c
# Step 2: build block_processing
g++ -std=c++14 -Wall -Wextra -Werror -pedantic -I kiss_fft130/ -I kiss_fft130/tools/ -o block_processing kiss_fft.o kiss_fftr.o block_processing.cc -lsndfile

# Clang
# Step 1: build Kiss FFT object files
clang -std=c11 -Wall -Wextra -Werror -pedantic -I kiss_fft130/                      -c kiss_fft130/kiss_fft.c
clang -std=c11 -Wall -Wextra -Werror -pedantic -I kiss_fft130/ -I kiss_fft130/tools -c kiss_fft130/tools/kiss_fftr.c
# Step 2: build block_processing
clang++ -std=c++14 -Wall -Wextra -Werror -pedantic -I kiss_fft130/ -I kiss_fft130/tools/ -o block_processing kiss_fft.o kiss_fftr.o block_processing.cc -lsndfile

echo "Running..."
#./make_sine
./command_line --frequency 440 --sample-rate 48000 --audio-format pcm32 --prefix-name sine --duration 0.00909090 --verbose
./block_processing
