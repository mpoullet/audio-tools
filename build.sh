#!/bin/sh
clang++-3.9 -std=c++14 -Wall -Wextra -Werror -pedantic -o make_sine_clang make_sine.cc -lsndfile -lm && ./make_sine_clang
g++         -std=c++14 -Wall -Wextra -Werror -pedantic -o make_sine_g++   make_sine.cc -lsndfile -lm && ./make_sine_g++
cppcheck --enable=all make_sine.cc
