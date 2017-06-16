#include <iostream>
#include <vector>
#include <memory>
#include <algorithm>

#include <sndfile.hh>

#define kiss_fft_scalar float
#include "kiss_fft.h"

int main(int argc, char *argv[])
{
    const int I = 2;
    const int D = 1;
    const int N = 256;
    const int M = I/D*N;

    if (argc != 2) {
        std::cerr << "Usage upsampling_algorithm_short_sequence <filename>\n";
        return -1;
    }

    const std::string infilename = argv[argc-1];
    const std::string resfilename = "res.wav";
    SndfileHandle  infile(infilename);
    SndfileHandle resfile(resfilename, SFM_WRITE, infile.format(), infile.channels(), infile.samplerate() * I/D);

    // Input data
    std::vector<kiss_fft_scalar> buffer(N);
    if (infile.read(buffer.data(), N) != N) {
        std::cerr << "Error reading " << N << " samples from " << infilename << ".\n";
    }

    // Kiss FFT configurations
    auto fwd_cfg = std::unique_ptr<std::remove_pointer<kiss_fft_cfg>::type, decltype(kiss_fft_free) *> {
        kiss_fft_alloc(N, 0, NULL, NULL),
        kiss_fft_free
    };

    auto inv_cfg = std::unique_ptr<std::remove_pointer<kiss_fft_cfg>::type, decltype(kiss_fft_free) *> {
        kiss_fft_alloc(M, 1, NULL, NULL),
        kiss_fft_free
    };

    if (fwd_cfg == nullptr || inv_cfg == nullptr) {
        std::cerr << "Error allocating Kiss FFT configurations\n";
    }

    // FFT input/output buffers
    std::vector<kiss_fft_cpx> fwd_fft_in_buffer(N);
    std::vector<kiss_fft_cpx> fwd_fft_out_buffer(N);

    // IFFT input/output buffers
    std::vector<kiss_fft_cpx> bwd_fft_in_buffer(M);
    std::vector<kiss_fft_cpx> bwd_fft_out_buffer(M);

    // Create FFT input buffer
    for (int i=0; i < N; ++i) {
        fwd_fft_in_buffer[i].r = buffer[i];
        fwd_fft_in_buffer[i].i = 0.0;
        std::cout << i << ": (" << fwd_fft_in_buffer[i].r << "," << fwd_fft_in_buffer[i].i << ")\n";
    }
    std::cout << "\n";

    // Forward N points FFT
    kiss_fft(fwd_cfg.get(), fwd_fft_in_buffer.data(), fwd_fft_out_buffer.data());
    for (int i=0; i < N; ++i) {
        std::cout << " FFT: " << i << " (" << fwd_fft_out_buffer[i].r << "," << fwd_fft_out_buffer[i].i << ")\n";
    }

    // Create IFFT input buffer
    for (int i=0; i < N/2; ++i) {
        bwd_fft_in_buffer[i].r = static_cast<kiss_fft_scalar>(I/D) * fwd_fft_out_buffer[i].r;
        bwd_fft_in_buffer[i].i = static_cast<kiss_fft_scalar>(I/D) * fwd_fft_out_buffer[i].i;
    }
    for (int i=N/2; i < M - N/2; ++i) {
        bwd_fft_in_buffer[i].r = static_cast<kiss_fft_scalar>(I/D) * 0.0;
        bwd_fft_in_buffer[i].i = static_cast<kiss_fft_scalar>(I/D) * 0.0;
    }
    for (int i=M - N/2; i < M; ++i) {
        bwd_fft_in_buffer[i].r = static_cast<kiss_fft_scalar>(I/D) * fwd_fft_out_buffer[i - N].r;
        bwd_fft_in_buffer[i].i = static_cast<kiss_fft_scalar>(I/D) * fwd_fft_out_buffer[i - N].i;
    }
    for (int i=0; i < M; ++i) {
        std::cout << i << ": (" << bwd_fft_in_buffer[i].r << "," << bwd_fft_in_buffer[i].i << ")\n";
    }

    // Backward M points IFFT
    kiss_fft(inv_cfg.get(), bwd_fft_in_buffer.data(), bwd_fft_out_buffer.data());
    for(int i=0; i < M; ++i) {
        std::cout << "IFFT: " << i << " (" << bwd_fft_out_buffer[i].r << "," << bwd_fft_out_buffer[i].i << ")\n";
    }

    return 0;
}
