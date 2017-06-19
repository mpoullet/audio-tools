#include <iostream>
#include <fstream>
#include <vector>
#include <memory>
#include <algorithm>
#include <functional>
#include <complex>
#include <iterator>

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

    const std::string input_filename = argv[argc-1];
    SndfileHandle input_file(input_filename);
    if (input_file.channels() != 1) {
        std::cerr << "Only files with one audio channel are supported.\n";
    }

    const std::string output_filename = "out.wav";
    SndfileHandle output_file(output_filename, SFM_WRITE, input_file.format(), input_file.channels(), input_file.samplerate() * I/D);

    // Input data
    std::vector<kiss_fft_scalar> buffer(N);
    if (input_file.read(buffer.data(), N) != N) {
        std::cerr << "Error reading " << N << " samples from " << input_filename << ".\n";
    }

    // Kiss FFT configurations
    auto fwd_cfg = std::unique_ptr<std::remove_pointer<kiss_fft_cfg>::type, decltype(kiss_fft_free) *> {
        kiss_fft_alloc(N, 0, nullptr, nullptr),
        kiss_fft_free
    };

    auto inv_cfg = std::unique_ptr<std::remove_pointer<kiss_fft_cfg>::type, decltype(kiss_fft_free) *> {
        kiss_fft_alloc(M, 1, nullptr, nullptr),
        kiss_fft_free
    };

    if (fwd_cfg == nullptr || inv_cfg == nullptr) {
        std::cerr << "Error allocating Kiss FFT configurations.\n";
    }

    // FFT input/output buffers
    std::vector<std::complex<kiss_fft_scalar>> fft_input_buffer(N);
    std::vector<std::complex<kiss_fft_scalar>> fft_output_buffer(N);

    // IFFT input/output buffers
    std::vector<std::complex<kiss_fft_scalar>> ifft_input_buffer(M);
    std::vector<std::complex<kiss_fft_scalar>> ifft_output_buffer(M);

    // Create FFT input buffer
    for (int i=0; i < N; ++i) {
        fft_input_buffer[i] = std::complex<kiss_fft_scalar>(buffer[i]);
    }

    std::ofstream fft_input_buffer_file("fft_input_buffer.asc");
    std::copy(fft_input_buffer.begin(), fft_input_buffer.end(), std::ostream_iterator<std::complex<kiss_fft_scalar>>(fft_input_buffer_file, "\n"));

    // Forward N points FFT
    kiss_fft(fwd_cfg.get(), reinterpret_cast<kiss_fft_cpx*>(fft_input_buffer.data()), reinterpret_cast<kiss_fft_cpx*>(fft_output_buffer.data()));

    std::ofstream fft_output_buffer_file("fft_output_buffer.asc");
    std::copy(fft_output_buffer.begin(), fft_output_buffer.end(), std::ostream_iterator<std::complex<kiss_fft_scalar>>(fft_output_buffer_file, "\n"));

    // Create IFFT input buffer
    for (int i=0; i < N/2; ++i) {
        ifft_input_buffer[i] = static_cast<kiss_fft_scalar>(I/D) * fft_output_buffer[i];
    }
    for (int i=N/2; i < M - N/2; ++i) {
        ifft_input_buffer[i] = static_cast<kiss_fft_scalar>(I/D) * std::complex<kiss_fft_scalar>(0.0, 0.0);
    }
    for (int i=M - N/2; i < M; ++i) {
        ifft_input_buffer[i] = static_cast<kiss_fft_scalar>(I/D) * fft_output_buffer[i - N];
    }

    std::ofstream ifft_input_buffer_file("ifft_input_buffer.asc");
    std::copy(ifft_input_buffer.begin(), ifft_input_buffer.end(), std::ostream_iterator<std::complex<kiss_fft_scalar>>(ifft_input_buffer_file, "\n"));

    // Backward M points IFFT
    kiss_fft(inv_cfg.get(), reinterpret_cast<kiss_fft_cpx*>(ifft_input_buffer.data()), reinterpret_cast<kiss_fft_cpx*>(ifft_output_buffer.data()));
    std::transform(ifft_output_buffer.begin(), ifft_output_buffer.end(),
                   ifft_output_buffer.begin(), std::bind1st(std::multiplies<std::complex<float>>(),  1.0/M ));

    std::ofstream ifft_output_buffer_file("ifft_output_buffer.asc");
    std::copy(ifft_output_buffer.begin(), ifft_output_buffer.end(), std::ostream_iterator<std::complex<kiss_fft_scalar>>(ifft_output_buffer_file, "\n"));

    kiss_fft_cleanup();
    return 0;
}
