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

void write_complex_data(const std::vector<std::complex<kiss_fft_scalar>>& v, const std::string& filename)
{
    const auto precision = []() -> std::streamsize {
        // https://www.working-software.com/cpp-floats-as-decimal
        if (std::is_same<kiss_fft_scalar, float>::value) return 9;
        if (std::is_same<kiss_fft_scalar, double>::value) return 17;
        return std::cout.precision();
    }();

    std::ofstream file(filename);
    file.precision(precision);
    std::copy(std::begin(v), std::end(v),
              std::ostream_iterator<std::complex<kiss_fft_scalar>>(file, "\n"));
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

    const std::string input_filename = argv[argc-1];
    SndfileHandle input_file(input_filename);
    if (input_file.channels() != 1) {
        std::cerr << "Only files with one audio channel are supported.\n";
    }

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

    // Create FFT input buffer
    std::vector<std::complex<kiss_fft_scalar>> fft_input_buffer(N);
    std::transform(std::begin(input_buffer), std::end(input_buffer),
                   std::begin(fft_input_buffer),
                   [](kiss_fft_scalar real) { return std::complex<kiss_fft_scalar>(real); });

    write_complex_data(fft_input_buffer, "fft_input_buffer.asc");

    // Forward N points FFT
    std::vector<std::complex<kiss_fft_scalar>> fft_output_buffer(fft_input_buffer.size());
    kiss_fft(fwd_cfg.get(),
             reinterpret_cast<kiss_fft_cpx*>(fft_input_buffer.data()),
             reinterpret_cast<kiss_fft_cpx*>(fft_output_buffer.data()));

    write_complex_data(fft_output_buffer, "fft_output_buffer.asc");

    // Method 1

    // Create IFFT input buffer, C_i = 0
    std::vector<std::complex<kiss_fft_scalar>> ifft_input_buffer_zeros(M);
    std::copy(std::begin(fft_output_buffer),       std::begin(fft_output_buffer) + N/2, std::begin(ifft_input_buffer_zeros));
    std::copy(std::begin(fft_output_buffer) + N/2, std::end(fft_output_buffer),         std::end(ifft_input_buffer_zeros) - N/2);
    std::transform(std::begin(ifft_input_buffer_zeros), std::end(ifft_input_buffer_zeros),
                   std::begin(ifft_input_buffer_zeros),
                   std::bind1st(std::multiplies<std::complex<kiss_fft_scalar>>(), 1.0*I/D));

    write_complex_data(ifft_input_buffer_zeros, "ifft_input_buffer_zeros.asc");

    // Backward M points IFFT
    std::vector<std::complex<kiss_fft_scalar>> ifft_output_buffer_zeros(ifft_input_buffer_zeros.size());
    kiss_fft(inv_cfg.get(),
             reinterpret_cast<kiss_fft_cpx*>(ifft_input_buffer_zeros.data()),
             reinterpret_cast<kiss_fft_cpx*>(ifft_output_buffer_zeros.data()));
    std::transform(std::begin(ifft_output_buffer_zeros), std::end(ifft_output_buffer_zeros),
                   std::begin(ifft_output_buffer_zeros),
                   std::bind1st(std::multiplies<std::complex<kiss_fft_scalar>>(), 1.0/M));

    write_complex_data(ifft_output_buffer_zeros, "ifft_output_buffer_zeros.asc");

    // Store results
    const std::string output_filename_zeros = "out_zeros.wav";
    SndfileHandle output_file_zeros(output_filename_zeros, SFM_WRITE, input_file.format(), input_file.channels(), input_file.samplerate() * I/D);
    std::vector<kiss_fft_scalar> output_buffer_zeros(ifft_output_buffer_zeros.size());
    std::transform(std::begin(ifft_output_buffer_zeros), std::end(ifft_output_buffer_zeros),
                   std::begin(output_buffer_zeros),
                   [](std::complex<kiss_fft_scalar> cpx) { return cpx.real(); });
    output_file_zeros.write(output_buffer_zeros.data(), output_buffer_zeros.size());

    // Method 2

    // Create IFFT input buffer, C_i = X(N/2)
    std::vector<std::complex<kiss_fft_scalar>> ifft_input_buffer_nonzeros(M, std::complex<kiss_fft_scalar>(fft_output_buffer[N/2]));
    std::copy(std::begin(fft_output_buffer),       std::begin(fft_output_buffer) + N/2, std::begin(ifft_input_buffer_nonzeros));
    std::copy(std::begin(fft_output_buffer) + N/2, std::end(fft_output_buffer),         std::end(ifft_input_buffer_nonzeros) - N/2);
    std::transform(std::begin(ifft_input_buffer_nonzeros), std::end(ifft_input_buffer_nonzeros),
                   std::begin(ifft_input_buffer_nonzeros),
                   std::bind1st(std::multiplies<std::complex<kiss_fft_scalar>>(), 1.0*I/D));

    write_complex_data(ifft_input_buffer_nonzeros, "ifft_input_buffer_nonzeros.asc");

    // Backward M points IFFT
    std::vector<std::complex<kiss_fft_scalar>> ifft_output_buffer_nonzeros(ifft_input_buffer_nonzeros.size());
    kiss_fft(inv_cfg.get(), reinterpret_cast<kiss_fft_cpx*>(ifft_input_buffer_nonzeros.data()), reinterpret_cast<kiss_fft_cpx*>(ifft_output_buffer_nonzeros.data()));
    std::transform(std::begin(ifft_output_buffer_nonzeros), std::end(ifft_output_buffer_nonzeros),
                   std::begin(ifft_output_buffer_nonzeros),
                   std::bind1st(std::multiplies<std::complex<kiss_fft_scalar>>(), 1.0/M));

    write_complex_data(ifft_output_buffer_nonzeros, "ifft_output_buffer_nonzeros.asc");

    // Store results
    const std::string output_filename_nonzeros = "out_nonzeros.wav";
    SndfileHandle output_file_nonzeros(output_filename_nonzeros, SFM_WRITE, input_file.format(), input_file.channels(), input_file.samplerate() * I/D);
    std::vector<kiss_fft_scalar> output_buffer_nonzeros(ifft_output_buffer_nonzeros.size());
    std::transform(std::begin(ifft_output_buffer_nonzeros), std::end(ifft_output_buffer_nonzeros),
                   std::begin(output_buffer_nonzeros),
                   [](std::complex<kiss_fft_scalar> cpx) { return cpx.real(); });
    output_file_nonzeros.write(output_buffer_nonzeros.data(), output_buffer_nonzeros.size());

    kiss_fft_cleanup();
    return 0;
}
