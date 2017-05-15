#!/bin/sh
gcc -std=c11 -Wall -Wextra -Werror -pedantic -o make_sine make_sine.c -lsndfile -lm && ./make_sine
