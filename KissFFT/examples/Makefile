# Makefile to build Kiss FFT examples

# https://stackoverflow.com/a/16346321
create_bin_dir:=$(shell mkdir -p bin)

CC=gcc
CXX=g++
CPPFLAGS=-I ../kiss_fft130/tools/ -I ../kiss_fft130/ -Wall -Wextra -Werror -pedantic 
CFLAGS=-std=c11
CXXFLAGS=-std=c++14
LIBS=-lm

all: complex_valued_fwd_fft_floats real_valued_fwd_fft_floats kissfft_cpp kissfft_cpp_raii
.PHONY: all clean

complex_valued_fwd_fft_floats: complex_valued_fwd_fft_floats.o ../kiss_fft130/kiss_fft.o
	@echo "LD $@"
	$(CC) -o bin/$@ $^ $(CPPFLAGS) $(CFLAGS) $(LIBS)

real_valued_fwd_fft_floats: real_valued_fwd_fft_floats.o ../kiss_fft130/kiss_fft.o ../kiss_fft130/tools/kiss_fftr.o
	@echo "LD $@"
	$(CC) -o bin/$@ $^ $(CPPFLAGS) $(CFLAGS) $(LIBS)

kissfft_cpp: kissfft_cpp.o ../kiss_fft130/kiss_fft.o
	@echo "LD $@"
	$(CXX) -o bin/$@ $^ $(CPPFLAGS) $(CXXFLAGS) $(LIBS)

kissfft_cpp_raii: kissfft_cpp_raii.o ../kiss_fft130/kiss_fft.o
	@echo "LD $@"
	$(CXX) -o bin/$@ $^ $(CPPFLAGS) $(CXXFLAGS) $(LIBS)

clean:
	rm -rf *.o bin/*

# the macros $@ and $^ are the left and right sides of the :
%.o: %.c
	@echo "CC $<"
	$(CC) -c -o $@ $< $(CPPFLAGS) $(CFLAGS)

%.o: %.cc
	@echo "CXX $<"
	$(CXX) -c -o $@ $< $(CPPFLAGS) $(CXXFLAGS)

# http://make.mad-scientist.net/managing-recipe-echoing/
$(V).SILENT:
