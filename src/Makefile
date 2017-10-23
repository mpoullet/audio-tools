# Makefile to build Kiss FFT examples

# https://stackoverflow.com/a/16346321
BINDIR := ../bin
create_bin_dir := $(shell mkdir -p $(BINDIR))

KISSFFT_DIR := ../KissFFT/kiss_fft130

CPPFLAGS := -I ./include -I $(KISSFFT_DIR)/tools -I $(KISSFFT_DIR) -Wall -Wextra -Werror -pedantic
CFLAGS := -std=c11 -O2
CXXFLAGS := -std=c++17
LIBS := -lm -lsndfile -lboost_program_options -lboost_system -lboost_filesystem

all: generate_A440 \
	 generate_sine_wave \
	 upsampling_algorithm_short_sequence \
	 upsampling_algorithm_long_sequence \
	 upsampling_algorithm_long_sequence_real_fft \
	 upsampling2x

.PHONY: all clean

# Automatic variables: https://www.gnu.org/software/make/manual/html_node/Automatic-Variables.html
# $@ is the file name of the target of the rule.
# $^ is the names of all the prerequisites, whith spaces between them.
# $< is the name of the first prerequisite.
generate_A440: generate_A440.o
	@echo "LD $@"
	$(CXX) -o $(BINDIR)/$@ $^ $(CPPFLAGS) $(CXXFLAGS) $(LIBS)

generate_sine_wave: generate_sine_wave.o
	@echo "LD $@"
	$(CXX) -o $(BINDIR)/$@ $^ $(CPPFLAGS) $(CXXFLAGS) $(LIBS)

upsampling_algorithm_short_sequence: upsampling_algorithm_short_sequence.o $(KISSFFT_DIR)/kiss_fft.o $(KISSFFT_DIR)/tools/kiss_fftr.o
	@echo "LD $@"
	$(CXX) -o $(BINDIR)/$@ $^ $(CPPFLAGS) $(CXXFLAGS) $(LIBS)

upsampling_algorithm_long_sequence: upsampling_algorithm_long_sequence.o $(KISSFFT_DIR)/kiss_fft.o $(KISSFFT_DIR)/tools/kiss_fftr.o
	@echo "LD $@"
	$(CXX) -o $(BINDIR)/$@ $^ $(CPPFLAGS) $(CXXFLAGS) $(LIBS)

upsampling_algorithm_long_sequence_real_fft: upsampling_algorithm_long_sequence_real_fft.o $(KISSFFT_DIR)/kiss_fft.o $(KISSFFT_DIR)/tools/kiss_fftr.o
	@echo "LD $@"
	$(CXX) -o $(BINDIR)/$@ $^ $(CPPFLAGS) $(CXXFLAGS) $(LIBS)

upsampling2x: upsampling2x.o $(KISSFFT_DIR)/kiss_fft.o $(KISSFFT_DIR)/tools/kiss_fftr.o
	@echo "LD $@"
	$(CXX) -o $(BINDIR)/$@ $^ $(CPPFLAGS) $(CXXFLAGS) $(LIBS)

clean:
	rm -rf *.o *.asc $(BINDIR)/* $(KISSFFT_DIR)/kiss_fft.o $(KISSFFT_DIR)/tools/kiss_fftr.o

%.o: %.c
	@echo "CC $<"
	$(CC) -c -o $@ $< $(CPPFLAGS) $(CFLAGS)

%.o: %.cc
	@echo "CXX $<"
	$(CXX) -c -o $@ $< $(CPPFLAGS) $(CXXFLAGS)

# http://make.mad-scientist.net/managing-recipe-echoing/
$(V).SILENT:
