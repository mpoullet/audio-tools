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
    std::vector<kiss_fft_scalar> input_buffer(N);
    if (input_file.read(input_buffer.data(), N) != N) {
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
    std::vector<std::complex<kiss_fft_scalar>> fft_output_buffer(fft_input_buffer.size());

    // IFFT input/output buffers
    std::vector<std::complex<kiss_fft_scalar>> ifft_input_buffer(M);
    std::vector<std::complex<kiss_fft_scalar>> ifft_output_buffer(ifft_input_buffer.size());

    // Create FFT input buffer
    std::transform(input_buffer.begin(), input_buffer.end(),
                   fft_input_buffer.begin(),
                   [](kiss_fft_scalar real) { return std::complex<kiss_fft_scalar>(real); });

    std::ofstream fft_input_buffer_file("fft_input_buffer.asc");
    std::copy(fft_input_buffer.begin(), fft_input_buffer.end(), std::ostream_iterator<std::complex<kiss_fft_scalar>>(fft_input_buffer_file, "\n"));

    // Forward N points FFT
    kiss_fft(fwd_cfg.get(), reinterpret_cast<kiss_fft_cpx*>(fft_input_buffer.data()), reinterpret_cast<kiss_fft_cpx*>(fft_output_buffer.data()));

    std::ofstream fft_output_buffer_file("fft_output_buffer.asc");
    std::copy(fft_output_buffer.begin(), fft_output_buffer.end(), std::ostream_iterator<std::complex<kiss_fft_scalar>>(fft_output_buffer_file, "\n"));

    // Create IFFT input buffer
    std::copy(fft_output_buffer.begin(),       fft_output_buffer.begin() + N/2, ifft_input_buffer.begin());
    std::copy(fft_output_buffer.begin() + N/2, fft_output_buffer.end(),         ifft_input_buffer.end() - N/2);
    std::transform(ifft_input_buffer.begin(), ifft_input_buffer.end(),
                   ifft_input_buffer.begin(), std::bind1st(std::multiplies<std::complex<kiss_fft_scalar>>(),  1.0*I/D ));

    std::ofstream ifft_input_buffer_file("ifft_input_buffer.asc");
    std::copy(ifft_input_buffer.begin(), ifft_input_buffer.end(), std::ostream_iterator<std::complex<kiss_fft_scalar>>(ifft_input_buffer_file, "\n"));

    // Backward M points IFFT
    kiss_fft(inv_cfg.get(), reinterpret_cast<kiss_fft_cpx*>(ifft_input_buffer.data()), reinterpret_cast<kiss_fft_cpx*>(ifft_output_buffer.data()));
    std::transform(ifft_output_buffer.begin(), ifft_output_buffer.end(),
                   ifft_output_buffer.begin(),
                   std::bind1st(std::multiplies<std::complex<kiss_fft_scalar>>(),  1.0/M ));

    std::ofstream ifft_output_buffer_file("ifft_output_buffer.asc");
    std::copy(ifft_output_buffer.begin(), ifft_output_buffer.end(), std::ostream_iterator<std::complex<kiss_fft_scalar>>(ifft_output_buffer_file, "\n"));

    // Store upsampled samples
    std::vector<kiss_fft_scalar> output_buffer(ifft_output_buffer.size());
    std::transform(ifft_output_buffer.begin(), ifft_output_buffer.end(),
            output_buffer.begin(),
            [](std::complex<kiss_fft_scalar> cpx) { return cpx.real(); });
    output_file.write(output_buffer.data(), output_buffer.size());

    kiss_fft_cleanup();
    return 0;
}
