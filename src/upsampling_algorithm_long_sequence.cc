#include <iostream>
#include <algorithm>
#include <vector>

#include <sndfile.hh>

#define kiss_fft_scalar float
#include "kiss_fft.h"

int main(int argc, char *argv[])
{
    const int I = 2;
    const int D = 1;
    const int N = 256;
    const int M = I*N/D;
    const int L = N/8;

    kiss_fft_cfg fwd = kiss_fft_alloc(N, 0, NULL, NULL);
    kiss_fft_cfg inv = kiss_fft_alloc(M, 1, NULL, NULL);

    if (argc != 2) return -1;

    const std::string infilename = argv[argc-1];
    const std::string outfilename = "orig.wav";
    const std::string resfilename = "res.wav";
    SndfileHandle  infile(infilename);
    SndfileHandle outfile(outfilename, SFM_WRITE, infile.format(), infile.channels(), infile.samplerate());
    SndfileHandle resfile(resfilename, SFM_WRITE, infile.format(), infile.channels(), infile.samplerate() * I/D);

    std::vector<kiss_fft_scalar> buffer(N);

    std::vector<kiss_fft_cpx> fwd_fft_in_buffer(N);
    std::vector<kiss_fft_cpx> fwd_fft_out_buffer(N);

    std::vector<kiss_fft_cpx> bwd_fft_in_buffer(M);
    std::vector<kiss_fft_cpx> bwd_fft_out_buffer(M);

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
            fwd_fft_in_buffer[i].r = buffer[i];
            fwd_fft_in_buffer[i].i = 0.0;
            std::cout << i << ": (" << fwd_fft_in_buffer[i].r << "," << fwd_fft_in_buffer[i].i << ")\n";
        }
        std::cout << "\n";

        /* Forward N points FFT */
        kiss_fft(fwd, fwd_fft_in_buffer.data(), fwd_fft_out_buffer.data());
        for(int i=0; i < N; ++i) {
            std::cout << " FFT: " << i << " (" << fwd_fft_out_buffer[i].r << "," << fwd_fft_out_buffer[i].i << ")\n";
        }

        /* Create IFFT input buffer */
        for(int i=0; i < N/2; ++i) {
            bwd_fft_in_buffer[i].r = static_cast<kiss_fft_scalar>(I/D) * fwd_fft_out_buffer[i].r;
            bwd_fft_in_buffer[i].i = static_cast<kiss_fft_scalar>(I/D) * fwd_fft_out_buffer[i].i;
        }
        for(int i=N/2; i <= M - N/2; ++i) {
            bwd_fft_in_buffer[i].r = static_cast<kiss_fft_scalar>(I/D) * 0.0;
            bwd_fft_in_buffer[i].i = static_cast<kiss_fft_scalar>(I/D) * 0.0;
        }
        for(int i=M - N/2 + 1; i < M; ++i) {
            bwd_fft_in_buffer[i].r = static_cast<kiss_fft_scalar>(I/D) * fwd_fft_out_buffer[i + M/2].r;
            bwd_fft_in_buffer[i].i = static_cast<kiss_fft_scalar>(I/D) * fwd_fft_out_buffer[i + M/2].i;
        }

        /* Backward M points IFFT */
        kiss_fft(inv, bwd_fft_in_buffer.data(), bwd_fft_out_buffer.data());
        for(int i=0; i < M; ++i) {
            std::cout << "IFFT: " << i << " (" << bwd_fft_out_buffer[i].r << "," << bwd_fft_out_buffer[i].i << ")\n";
        }

        /* Discard first and last I/D*L points and store the rest of the real vector */
        std::vector<double> res(M - 2*I/D*L);
        int j = I/D*L;
        for(int i=0; i < M - 2*I/D*L; ++i) {
            res[i] = bwd_fft_out_buffer[j++].r;
        }
        resfile.write(res.data(), res.size());

        /* Shift vector to the left 2*L times */
        std::rotate(buffer.begin(), buffer.begin() + N - 2*L, buffer.end());
    };

    kiss_fft_free(fwd);
    kiss_fft_free(inv);

    return 0;
}
