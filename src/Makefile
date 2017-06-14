# Makefile to build Kiss FFT examples

# https://stackoverflow.com/a/16346321
create_bin_dir := $(shell mkdir -p bin)

CC ?= gcc
CXX ?= g++
CPPFLAGS := -I ./ -I ../KissFFT/kiss_fft130/tools/ -I ../KissFFT/kiss_fft130/ -Wall -Wextra -Werror -pedantic
CFLAGS := -std=c11
CXXFLAGS := -std=c++14
LIBS := -lm -lsndfile -lboost_program_options -lboost_system -lboost_filesystem

all: make_sine command_line block_processing
.PHONY: all clean

# Automatic variables: https://www.gnu.org/software/make/manual/html_node/Automatic-Variables.html
# $@ is the file name of the target of the rule.
# $^ is the names of all the prerequisites, whith spaces between them.
# $< is the name of the first prerequisite.
make_sine: make_sine.o
	@echo "LD $@"
	$(CXX) -o bin/$@ $^ $(CPPFLAGS) $(CXXFLAGS) $(LIBS)

command_line: command_line.o
	@echo "LD $@"
	$(CXX) -o bin/$@ $^ $(CPPFLAGS) $(CXXFLAGS) $(LIBS)

block_processing: block_processing.o ../KissFFT/kiss_fft130/kiss_fft.o ../KissFFT/kiss_fft130/tools/kiss_fftr.o
	@echo "LD $@"
	$(CXX) -o bin/$@ $^ $(CPPFLAGS) $(CXXFLAGS) $(LIBS)

clean:
	rm -rf *.o bin/*

%.o: %.c
	@echo "CC $<"
	$(CC) -c -o $@ $< $(CPPFLAGS) $(CFLAGS)

%.o: %.cc
	@echo "CXX $<"
	$(CXX) -c -o $@ $< $(CPPFLAGS) $(CXXFLAGS)

# http://make.mad-scientist.net/managing-recipe-echoing/
$(V).SILENT:
