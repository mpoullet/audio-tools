#!/bin/sh

set -e
set -u

g++     -std=c++14 -Wall -Wextra -Werror -pedantic -o make_sine make_sine.cc -lsndfile -lm
clang++ -std=c++14 -Wall -Wextra -Werror -pedantic -o make_sine make_sine.cc -lsndfile -lm

gcc   -std=c11 -o complex_valued_fwd_fft_floats -Wall -Wextra -Werror -pedantic -I kiss_fft130/                      complex_valued_fwd_fft_floats.c kiss_fft130/kiss_fft.c                               -lm
clang -std=c11 -o complex_valued_fwd_fft_floats -Wall -Wextra -Werror -pedantic -I kiss_fft130/                      complex_valued_fwd_fft_floats.c kiss_fft130/kiss_fft.c                               -lm
gcc   -std=c11 -o real_valued_fwd_fft_floats    -Wall -Wextra -Werror -pedantic -I kiss_fft130/tools/ -I kiss_fft130 real_valued_fwd_fft_floats.c    kiss_fft130/kiss_fft.c kiss_fft130/tools/kiss_fftr.c -lm
clang -std=c11 -o real_valued_fwd_fft_floats    -Wall -Wextra -Werror -pedantic -I kiss_fft130/tools/ -I kiss_fft130 real_valued_fwd_fft_floats.c    kiss_fft130/kiss_fft.c kiss_fft130/tools/kiss_fftr.c -lm

cppcheck --enable=all *.cc *.c
