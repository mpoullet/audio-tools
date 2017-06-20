# Makefile to build Kiss FFT examples

# https://stackoverflow.com/a/16346321
create_bin_dir := $(shell mkdir -p bin)

BINDIR := bin
KISSFFT_DIR := ../kiss_fft130

CPPFLAGS := -I $(KISSFFT_DIR)/tools/ -I $(KISSFFT_DIR)/ -Wall -Wextra -Werror -pedantic
CFLAGS := -std=c11
CXXFLAGS := -std=c++14
LIBS := -lm

all: complex_valued_fwd_fft_floats real_valued_fwd_fft_floats kissfft_cpp kissfft_cpp_raii
.PHONY: all clean

# Automatic variables: https://www.gnu.org/software/make/manual/html_node/Automatic-Variables.html
# $@ is the file name of the target of the rule.
# $^ is the names of all the prerequisites, whith spaces between them.
# $< is the name of the first prerequisite.
complex_valued_fwd_fft_floats: complex_valued_fwd_fft_floats.o $(KISSFFT_DIR)/kiss_fft.o
	@echo "LD $@"
	$(CC) -o $(BINDIR)/$@ $^ $(CPPFLAGS) $(CFLAGS) $(LIBS)

real_valued_fwd_fft_floats: real_valued_fwd_fft_floats.o $(KISSFFT_DIR)/kiss_fft.o $(KISSFFT_DIR)/tools/kiss_fftr.o
	@echo "LD $@"
	$(CC) -o $(BINDIR)/$@ $^ $(CPPFLAGS) $(CFLAGS) $(LIBS)

kissfft_cpp: kissfft_cpp.o $(KISSFFT_DIR)/kiss_fft.o
	@echo "LD $@"
	$(CXX) -o $(BINDIR)/$@ $^ $(CPPFLAGS) $(CXXFLAGS) $(LIBS)

kissfft_cpp_raii: kissfft_cpp_raii.o $(KISSFFT_DIR)/kiss_fft.o
	@echo "LD $@"
	$(CXX) -o $(BINDIR)/$@ $^ $(CPPFLAGS) $(CXXFLAGS) $(LIBS)

clean:
	rm -rf *.o $(BINDIR)/* $(KISSFFT_DIR)/kiss_fft.o $(KISSFFT_DIR)/tools/kiss_fftr.o

%.o: %.c
	@echo "CC $<"
	$(CC) -c -o $@ $< $(CPPFLAGS) $(CFLAGS)

%.o: %.cc
	@echo "CXX $<"
	$(CXX) -c -o $@ $< $(CPPFLAGS) $(CXXFLAGS)

# http://make.mad-scientist.net/managing-recipe-echoing/
$(V).SILENT:
