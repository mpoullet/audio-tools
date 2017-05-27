#include <iostream>
#include <algorithm>
#include <vector>
#include <complex>

#include <sndfile.hh>

#define kiss_fft_scalar double
#include "kiss_fft.h"

int main()
{
    const int I = 2;
    const int D = 1;
    const int N = 256;
    const int M = I*N/D;
    const int L = N/8;

    kiss_fft_cfg fwd = kiss_fft_alloc(N, 0, NULL, NULL);
    kiss_fft_cfg inv = kiss_fft_alloc(M, 1, NULL, NULL);

    const std::string infilename = "sine_48000_pcm32.wav";
    const std::string outfilename = "out.wav";
    SndfileHandle  infile(infilename);
    SndfileHandle outfile(outfilename, SFM_WRITE, infile.format(), infile.channels(), infile.samplerate() * I/D);

    std::vector<double> buffer(N);

    std::vector<std::complex<double>> fwd_fft_in_buffer(N);
    std::vector<std::complex<double>> fwd_fft_out_buffer(N);

    /* prepend 2L zeros */
    for (int i=0; i < 2*L; ++i) {
        buffer[i] = 0;
    }

    while (sf_count_t readcount = infile.read(buffer.data() + 2*L, N - 2*L))
    {   
        /* Store original samples */
        outfile.write(buffer.data() + 2*L, std::min(static_cast<int>(readcount), N - 2*L));

        /* Create FFT input buffer: 1 block of N samples with 2*L overlap */
        for(int i=0; i < std::min(static_cast<int>(readcount) + 2*L, N); ++i) {
            fwd_fft_in_buffer[i] = std::complex<double>(buffer[i], 0.0);
            std::cout << i << ": " << fwd_fft_in_buffer[i] << "\n";
        }
        std::cout << "\n";

        /* Forward N points FFT */
        kiss_fft(fwd, (kiss_fft_cpx *)fwd_fft_in_buffer.data(), (kiss_fft_cpx *)(fwd_fft_out_buffer.data()));
        for(int i=0; i < N; ++i) {
            std::cout << "FFT: " << i << " " << fwd_fft_out_buffer[i] << "\n";
        }

        /* Create IFFT input buffer */

        /* IFFT */

        /* Shift vector to the left 2*L times */
        std::rotate(buffer.begin(), buffer.begin() + N - 2*L, buffer.end());
    };

    kiss_fft_free(fwd);
    kiss_fft_free(inv);

    return 0;
}
