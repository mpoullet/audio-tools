#!/bin/sh
g++ -std=c++14 -Wall -Wextra -Werror -pedantic -o make_sine make_sine.cc -lsndfile -lm && ./make_sine
