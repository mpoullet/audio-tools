#include <iostream>
#include <fstream>
#include <vector>
#include <memory>
#include <algorithm>
#include <functional>
#include <complex>

#include <sndfile.hh>

#define kiss_fft_scalar float
#include "kiss_fft.h"

// https://stackoverflow.com/a/2284805/496459
template <typename Stream>
void reopen(Stream& pStream, const char * pFile,
            std::ios_base::openmode pMode = std::ios_base::out)
{
    pStream.close();
    pStream.clear();
    pStream.open(pFile, pMode);
}

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
        std::cerr << "Error allocating Kiss FFT configurations.\n";
    }

    // FFT input/output buffers
    std::vector<std::complex<kiss_fft_scalar>> fwd_fft_in_buffer(N);
    std::vector<std::complex<kiss_fft_scalar>> fwd_fft_out_buffer(N);

    // IFFT input/output buffers
    std::vector<std::complex<kiss_fft_scalar>> bwd_fft_in_buffer(M);
    std::vector<std::complex<kiss_fft_scalar>> bwd_fft_out_buffer(M);

    // Create FFT input buffer
    std::ofstream ofs("fft_input_buffer.asc");
    for (int i=0; i < N; ++i) {
        fwd_fft_in_buffer[i] = std::complex<kiss_fft_scalar>(buffer[i], 0.0);
        ofs << i << " " << fwd_fft_in_buffer[i].real() << " " << fwd_fft_in_buffer[i].imag() << "\n";
    }

    // Forward N points FFT
    reopen(ofs, "fft_output_buffer.asc");
    kiss_fft(fwd_cfg.get(), (kiss_fft_cpx*)(fwd_fft_in_buffer.data()), (kiss_fft_cpx*)(fwd_fft_out_buffer.data()));
    for (int i=0; i < N; ++i) {
        ofs << i << " " << fwd_fft_out_buffer[i].real() << " " << fwd_fft_out_buffer[i].imag() << "\n";
    }

    // Create IFFT input buffer
    reopen(ofs, "ifft_input_buffer.asc");
    for (int i=0; i < N/2; ++i) {
        bwd_fft_in_buffer[i] = static_cast<kiss_fft_scalar>(I/D) * fwd_fft_out_buffer[i];
    }
    for (int i=N/2; i < M - N/2; ++i) {
        bwd_fft_in_buffer[i] = static_cast<kiss_fft_scalar>(I/D) * std::complex<kiss_fft_scalar>(0.0, 0.0);
    }
    for (int i=M - N/2; i < M; ++i) {
        bwd_fft_in_buffer[i] = static_cast<kiss_fft_scalar>(I/D) * fwd_fft_out_buffer[i - N];
    }
    for (int i=0; i < M; ++i) {
        ofs << i << " " << bwd_fft_in_buffer[i].real() << " " << bwd_fft_in_buffer[i].imag() << "\n";
    }

    // Backward M points IFFT
    reopen(ofs, "ifft_output_buffer.asc");
    kiss_fft(inv_cfg.get(), (kiss_fft_cpx*)(bwd_fft_in_buffer.data()), (kiss_fft_cpx*)(bwd_fft_out_buffer.data()));
    /*
    std::transform(bwd_fft_out_buffer.begin(), bwd_fft_out_buffer.end(),
                   bwd_fft_out_buffer.begin(), std::bind1st(std::multiplies<kiss_fft_scalar>(), 1.0/M));
    */
    for(int i=0; i < M; ++i) {
        bwd_fft_out_buffer[i] = static_cast<kiss_fft_scalar>(1.0/M) * bwd_fft_out_buffer[i];
        ofs << i << " " << bwd_fft_out_buffer[i].real() << " " << bwd_fft_out_buffer[i].imag() << "\n";
    }

    kiss_fft_cleanup();
    return 0;
}
