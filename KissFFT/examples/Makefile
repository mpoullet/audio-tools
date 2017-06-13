CC=gcc
CPPFLAGS=-I ../kiss_fft130/tools/ -I ../kiss_fft130/ -Wall -Wextra -Werror -pedantic 
CFLAGS=-std=c11
LIBS=-lm

%.o: %.c
	$(CC) -c -o $@ $< $(CPPFLAGS) $(CFLAGS)

# the macros $@ and $^ are the left and right sides of the :

all: complex_valued_fwd_fft_floats real_valued_fwd_fft_floats

complex_valued_fwd_fft_floats: complex_valued_fwd_fft_floats.o ../kiss_fft130/kiss_fft.o
	$(CC) -o $@ $^ $(CPPFLAGS) $(CFLAGS) $(LIBS)

real_valued_fwd_fft_floats: real_valued_fwd_fft_floats.o ../kiss_fft130/kiss_fft.o ../kiss_fft130/tools/kiss_fftr.o
	$(CC) -o $@ $^ $(CPPFLAGS) $(CFLAGS) $(LIBS)


.PHONY: clean

clean:
	rm -f *.o complex_valued_fwd_fft_floats real_valued_fwd_fft_floats
