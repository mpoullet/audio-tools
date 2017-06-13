# Makefile to build Kiss FFT examples

# https://stackoverflow.com/a/16346321
create_bin_dir:=$(shell mkdir -p bin)

# https://stackoverflow.com/q/24005166
MAKEFLAGS += --silent

CC=gcc
CXX=g++
CPPFLAGS=-I ../kiss_fft130/tools/ -I ../kiss_fft130/ -Wall -Wextra -Werror -pedantic 
CFLAGS=-std=c11
CXXFLAGS=-std=c++14
LIBS=-lm

# the macros $@ and $^ are the left and right sides of the :
%.o: %.c
	$(CC) -c -o $@ $< $(CPPFLAGS) $(CFLAGS)

%.o: %.cc
	$(CXX) -c -o $@ $< $(CPPFLAGS) $(CXXFLAGS)

all: complex_valued_fwd_fft_floats real_valued_fwd_fft_floats kissfft_cpp kissfft_cpp_raii

complex_valued_fwd_fft_floats: complex_valued_fwd_fft_floats.o ../kiss_fft130/kiss_fft.o
	$(CC) -o bin/$@ $^ $(CPPFLAGS) $(CFLAGS) $(LIBS)

real_valued_fwd_fft_floats: real_valued_fwd_fft_floats.o ../kiss_fft130/kiss_fft.o ../kiss_fft130/tools/kiss_fftr.o
	$(CC) -o bin/$@ $^ $(CPPFLAGS) $(CFLAGS) $(LIBS)

kissfft_cpp: kissfft_cpp.o ../kiss_fft130/kiss_fft.o
	$(CXX) -o bin/$@ $^ $(CPPFLAGS) $(CXXFLAGS) $(LIBS)

kissfft_cpp_raii: kissfft_cpp_raii.o ../kiss_fft130/kiss_fft.o
	$(CXX) -o bin/$@ $^ $(CPPFLAGS) $(CXXFLAGS) $(LIBS)

.PHONY: clean

clean:
	rm -rf *.o bin/*
