# Makefile to build Kiss FFT examples

# https://stackoverflow.com/a/16346321
create_bin_dir := $(shell mkdir -p ../bin)

BINDIR := ../bin

CC ?= gcc
CXX ?= g++
CPPFLAGS := -I ./ -I ../KissFFT/kiss_fft130/tools/ -I ../KissFFT/kiss_fft130/ -Wall -Wextra -Werror -pedantic
CFLAGS := -std=c11
CXXFLAGS := -std=c++14
LIBS := -lm -lsndfile -lboost_program_options -lboost_system -lboost_filesystem

all: make_sine command_line upsampling_algorithm_short_sequence upsampling_algorithm_long_sequence
.PHONY: all clean

# Automatic variables: https://www.gnu.org/software/make/manual/html_node/Automatic-Variables.html
# $@ is the file name of the target of the rule.
# $^ is the names of all the prerequisites, whith spaces between them.
# $< is the name of the first prerequisite.
make_sine: make_sine.o
	@echo "LD $@"
	$(CXX) -o $(BINDIR)/$@ $^ $(CPPFLAGS) $(CXXFLAGS) $(LIBS)

command_line: command_line.o
	@echo "LD $@"
	$(CXX) -o $(BINDIR)/$@ $^ $(CPPFLAGS) $(CXXFLAGS) $(LIBS)

upsampling_algorithm_short_sequence: upsampling_algorithm_short_sequence.o ../KissFFT/kiss_fft130/kiss_fft.o ../KissFFT/kiss_fft130/tools/kiss_fftr.o
	@echo "LD $@"
	$(CXX) -o $(BINDIR)/$@ $^ $(CPPFLAGS) $(CXXFLAGS) $(LIBS)

upsampling_algorithm_long_sequence: upsampling_algorithm_long_sequence.o ../KissFFT/kiss_fft130/kiss_fft.o ../KissFFT/kiss_fft130/tools/kiss_fftr.o
	@echo "LD $@"
	$(CXX) -o $(BINDIR)/$@ $^ $(CPPFLAGS) $(CXXFLAGS) $(LIBS)

clean:
	rm -rf *.o *.asc $(BINDIR)/*

%.o: %.c
	@echo "CC $<"
	$(CC) -c -o $@ $< $(CPPFLAGS) $(CFLAGS)

%.o: %.cc
	@echo "CXX $<"
	$(CXX) -c -o $@ $< $(CPPFLAGS) $(CXXFLAGS)

# http://make.mad-scientist.net/managing-recipe-echoing/
$(V).SILENT:
