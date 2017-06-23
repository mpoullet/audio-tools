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
#include "kiss_fftr.h"

namespace {
    const auto precision = []() -> std::streamsize {
            // https://www.working-software.com/cpp-floats-as-decimal
            if (std::is_same<kiss_fft_scalar, float>::value) return 9;
            if (std::is_same<kiss_fft_scalar, double>::value) return 17;
            return std::cout.precision();
        }();

    template <typename T>
    void write_data(const std::vector<T>& v, const std::string& filename)
    {
        std::ofstream file(filename);
        file.precision(precision);
        std::copy(std::begin(v), std::end(v),
                  std::ostream_iterator<T>(file, "\n"));
    }
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
        return -1;
    }

    // Input data
    std::vector<kiss_fft_scalar> input_buffer(N);
    if (input_file.read(input_buffer.data(), N) != N) {
        std::cerr << "Error reading " << N << " samples from " << input_filename << ".\n";
        return -1;
    }

    // Kiss FFT complex configurations
    const auto fwd_cfg = std::unique_ptr<std::remove_pointer<kiss_fft_cfg>::type, decltype(kiss_fft_free) *> {
        kiss_fft_alloc(N, 0, nullptr, nullptr),
        kiss_fft_free
    };

    const auto inv_cfg = std::unique_ptr<std::remove_pointer<kiss_fft_cfg>::type, decltype(kiss_fft_free) *> {
        kiss_fft_alloc(M, 1, nullptr, nullptr),
        kiss_fft_free
    };

    // Kiss FFT real configurarions
    const auto fwd_cfg_r = std::unique_ptr<std::remove_pointer<kiss_fftr_cfg>::type, decltype(kiss_fft_free) *> {
        kiss_fftr_alloc(N, 0, nullptr, nullptr),
        kiss_fft_free
    };

    const auto inv_cfg_r = std::unique_ptr<std::remove_pointer<kiss_fftr_cfg>::type, decltype(kiss_fft_free) *> {
        kiss_fftr_alloc(M, 1, nullptr, nullptr),
        kiss_fft_free
    };

    if (fwd_cfg   == nullptr ||
        inv_cfg   == nullptr ||
        fwd_cfg_r == nullptr ||
        inv_cfg_r == nullptr)
    {
        std::cerr << "Error allocating Kiss FFT configurations.\n";
        return -1;
    }

    // Create FFT input buffer
    std::vector<std::complex<kiss_fft_scalar>> fft_input_buffer(input_buffer.size());

    write_data(fft_input_buffer, "fft_input_buffer.asc");

    // Forward N points complex FFT
    std::transform(std::begin(input_buffer), std::end(input_buffer),
                   std::begin(fft_input_buffer),
                   [](const kiss_fft_scalar& real) {
                       return std::complex<kiss_fft_scalar>(real);
                   });
    std::vector<std::complex<kiss_fft_scalar>> fft_output_buffer(fft_input_buffer.size());
    kiss_fft(fwd_cfg.get(),
             reinterpret_cast<kiss_fft_cpx*>(fft_input_buffer.data()),
             reinterpret_cast<kiss_fft_cpx*>(fft_output_buffer.data()));

    write_data(fft_output_buffer, "fft_output_buffer.asc");

    // Forward N points real FFT
    std::vector<std::complex<kiss_fft_scalar>> fftr_output_buffer(input_buffer.size()/2 + 1);
    kiss_fftr(fwd_cfg_r.get(),
              reinterpret_cast<kiss_fft_scalar*>(input_buffer.data()),
              reinterpret_cast<kiss_fft_cpx*>(fftr_output_buffer.data()));

    write_data(fftr_output_buffer, "fftr_output_buffer.asc");

    // Method 1 (complex FFT)

    // Create IFFT input buffer, C_i = 0
    std::vector<std::complex<kiss_fft_scalar>> ifft_input_buffer_zeros(M);
    std::copy(std::begin(fft_output_buffer), std::begin(fft_output_buffer) + N/2,
              std::begin(ifft_input_buffer_zeros));
    std::copy(std::begin(fft_output_buffer) + N/2 + 1, std::end(fft_output_buffer),
              std::end(ifft_input_buffer_zeros) - N/2 + 1);
    std::transform(std::begin(ifft_input_buffer_zeros), std::end(ifft_input_buffer_zeros),
                   std::begin(ifft_input_buffer_zeros),
                   std::bind1st(std::multiplies<std::complex<kiss_fft_scalar>>(), 1.0 * I/D));

    write_data(ifft_input_buffer_zeros, "ifft_input_buffer_zeros.asc");

    // Backward M points complex IFFT
    std::vector<std::complex<kiss_fft_scalar>> ifft_output_buffer_zeros(ifft_input_buffer_zeros.size());
    kiss_fft(inv_cfg.get(),
             reinterpret_cast<kiss_fft_cpx*>(ifft_input_buffer_zeros.data()),
             reinterpret_cast<kiss_fft_cpx*>(ifft_output_buffer_zeros.data()));
    std::transform(std::begin(ifft_output_buffer_zeros), std::end(ifft_output_buffer_zeros),
                   std::begin(ifft_output_buffer_zeros),
                   std::bind1st(std::multiplies<std::complex<kiss_fft_scalar>>(), 1.0 / M));

    write_data(ifft_output_buffer_zeros, "ifft_output_buffer_zeros.asc");

    // Store results
    std::vector<kiss_fft_scalar> output_buffer_zeros(ifft_output_buffer_zeros.size());
    std::transform(std::begin(ifft_output_buffer_zeros), std::end(ifft_output_buffer_zeros),
                   std::begin(output_buffer_zeros),
                   [](const std::complex<kiss_fft_scalar>& cpx) -> kiss_fft_scalar {
                       kiss_fft_scalar max_val = 0.99999990;
                       return std::max(-max_val, std::min(cpx.real(), max_val));
                   });

    write_data(output_buffer_zeros, "upsampling_algorithm_short_sequence_out_zeros.asc");

    const std::string output_filename_zeros = "upsampling_algorithm_short_sequence_out_zeros.wav";
    SndfileHandle output_file_zeros(output_filename_zeros, SFM_WRITE,
                                    input_file.format(), input_file.channels(), input_file.samplerate() * I/D);
    output_file_zeros.write(output_buffer_zeros.data(), output_buffer_zeros.size());

    // Method 1 (real FFT)

    // Create IFFT input buffer, C_i = 0
    std::vector<std::complex<kiss_fft_scalar>> ifftr_input_buffer_zeros(M/2 + 1);
    std::copy(std::begin(fftr_output_buffer), std::end(fftr_output_buffer) - 1,
              std::begin(ifftr_input_buffer_zeros));
    std::transform(std::begin(ifftr_input_buffer_zeros), std::end(ifftr_input_buffer_zeros),
                   std::begin(ifftr_input_buffer_zeros),
                   std::bind1st(std::multiplies<std::complex<kiss_fft_scalar>>(), 1.0 * I/D));

    write_data(ifftr_input_buffer_zeros, "ifftr_input_buffer_zeros.asc");

    // Backward M points real IFFT
    std::vector<kiss_fft_scalar> ifftr_output_buffer_zeros(M);
    kiss_fftri(inv_cfg_r.get(),
               reinterpret_cast<kiss_fft_cpx*>(ifftr_input_buffer_zeros.data()),
               reinterpret_cast<kiss_fft_scalar*>(ifftr_output_buffer_zeros.data()));
    std::transform(std::begin(ifftr_output_buffer_zeros), std::end(ifftr_output_buffer_zeros),
                   std::begin(ifftr_output_buffer_zeros),
                   std::bind1st(std::multiplies<kiss_fft_scalar>(), 1.0 / M));

    write_data(ifftr_output_buffer_zeros, "ifftr_output_buffer_zeros.asc");

    std::transform(std::begin(ifftr_output_buffer_zeros), std::end(ifftr_output_buffer_zeros),
                   std::begin(ifftr_output_buffer_zeros),
                   [](const kiss_fft_scalar& scalar) -> kiss_fft_scalar {
                       kiss_fft_scalar max_val = 0.99999990;
                       return std::max(-max_val, std::min(scalar, max_val));
                   });

    write_data(output_buffer_zeros, "upsampling_algorithm_short_sequence_out_zeros_real_fft.asc");

    const std::string output_filename_zeros_real_fft = "upsampling_algorithm_short_sequence_out_zeros_real_fft.wav";
    SndfileHandle output_file_zeros_real_fft(output_filename_zeros_real_fft, SFM_WRITE,
                                    input_file.format(), input_file.channels(), input_file.samplerate() * I/D);
    output_file_zeros_real_fft.write(ifftr_output_buffer_zeros.data(), ifftr_output_buffer_zeros.size());

    // Method 2 (complex FFT)

    // Create IFFT input buffer, C_i = X(N/2)
    const auto C_i = static_cast<kiss_fft_scalar>(1.0 * D/I) * std::complex<kiss_fft_scalar>(fft_output_buffer[N/2]);
    std::vector<std::complex<kiss_fft_scalar>> ifft_input_buffer_nonzeros(M, C_i);
    std::copy(std::begin(fft_output_buffer), std::begin(fft_output_buffer) + N/2,
              std::begin(ifft_input_buffer_nonzeros));
    std::copy(std::begin(fft_output_buffer) + N/2 + 1, std::end(fft_output_buffer),
              std::end(ifft_input_buffer_nonzeros) - N/2 + 1);
    std::transform(std::begin(ifft_input_buffer_nonzeros), std::end(ifft_input_buffer_nonzeros),
                   std::begin(ifft_input_buffer_nonzeros),
                   std::bind1st(std::multiplies<std::complex<kiss_fft_scalar>>(), 1.0 * I/D));

    write_data(ifft_input_buffer_nonzeros, "ifft_input_buffer_nonzeros.asc");

    // Backward M points IFFT
    std::vector<std::complex<kiss_fft_scalar>> ifft_output_buffer_nonzeros(ifft_input_buffer_nonzeros.size());
    kiss_fft(inv_cfg.get(),
             reinterpret_cast<kiss_fft_cpx*>(ifft_input_buffer_nonzeros.data()),
             reinterpret_cast<kiss_fft_cpx*>(ifft_output_buffer_nonzeros.data()));
    std::transform(std::begin(ifft_output_buffer_nonzeros), std::end(ifft_output_buffer_nonzeros),
                   std::begin(ifft_output_buffer_nonzeros),
                   std::bind1st(std::multiplies<std::complex<kiss_fft_scalar>>(), 1.0 / M));

    write_data(ifft_output_buffer_nonzeros, "ifft_output_buffer_nonzeros.asc");

    // Store results
    std::vector<kiss_fft_scalar> output_buffer_nonzeros(ifft_output_buffer_nonzeros.size());
    std::transform(std::begin(ifft_output_buffer_nonzeros), std::end(ifft_output_buffer_nonzeros),
                   std::begin(output_buffer_nonzeros),
                   [](const std::complex<kiss_fft_scalar>& cpx) -> kiss_fft_scalar {
                       kiss_fft_scalar max_val = 0.99999990;
                       return std::max(-max_val, std::min(cpx.real(), max_val));
                   });

    write_data(output_buffer_nonzeros, "upsampling_algorithm_short_sequence_out_nonzeros.asc");

    const std::string output_filename_nonzeros = "upsampling_algorithm_short_sequence_out_nonzeros.wav";
    SndfileHandle output_file_nonzeros(output_filename_nonzeros, SFM_WRITE,
                                       input_file.format(), input_file.channels(), input_file.samplerate() * I/D);
    output_file_nonzeros.write(output_buffer_nonzeros.data(), output_buffer_nonzeros.size());

    // Method 2 (real FFT)

    // Create IFFT input buffer, C_i = X(N/2)
    const auto C_i_real_fft = static_cast<kiss_fft_scalar>(1.0 * D/I) * std::complex<kiss_fft_scalar>(fftr_output_buffer[N/2]);
    std::vector<std::complex<kiss_fft_scalar>> ifftr_input_buffer_nonzeros(M/2 + 1, C_i_real_fft);
    std::copy(std::begin(fftr_output_buffer), std::end(fftr_output_buffer) - 1,
              std::begin(ifftr_input_buffer_nonzeros));
    std::transform(std::begin(ifftr_input_buffer_nonzeros), std::end(ifftr_input_buffer_nonzeros),
                   std::begin(ifftr_input_buffer_nonzeros),
                   std::bind1st(std::multiplies<std::complex<kiss_fft_scalar>>(), 1.0 * I/D));

    write_data(ifftr_input_buffer_nonzeros, "ifftr_input_buffer_nonzeros.asc");

    // Backward M points real IFFT
    std::vector<kiss_fft_scalar> ifftr_output_buffer_nonzeros(M);
    kiss_fftri(inv_cfg_r.get(),
               reinterpret_cast<kiss_fft_cpx*>(ifftr_input_buffer_nonzeros.data()),
               reinterpret_cast<kiss_fft_scalar*>(ifftr_output_buffer_nonzeros.data()));
    std::transform(std::begin(ifftr_output_buffer_nonzeros), std::end(ifftr_output_buffer_nonzeros),
                   std::begin(ifftr_output_buffer_nonzeros),
                   std::bind1st(std::multiplies<kiss_fft_scalar>(), 1.0 / M));

    write_data(ifftr_output_buffer_nonzeros, "ifftr_output_buffer_nonzeros.asc");

    std::transform(std::begin(ifftr_output_buffer_nonzeros), std::end(ifftr_output_buffer_nonzeros),
                   std::begin(ifftr_output_buffer_nonzeros),
                   [](const kiss_fft_scalar& scalar) -> kiss_fft_scalar {
                       kiss_fft_scalar max_val = 0.99999990;
                       return std::max(-max_val, std::min(scalar, max_val));
                   });

    write_data(output_buffer_nonzeros, "upsampling_algorithm_short_sequence_out_nonzeros_real_fft.asc");

    const std::string output_filename_nonzeros_real_fft = "upsampling_algorithm_short_sequence_out_nonzeros_real_fft.wav";
    SndfileHandle output_file_nonzeros_real_fft(output_filename_nonzeros_real_fft, SFM_WRITE,
                                                input_file.format(), input_file.channels(), input_file.samplerate() * I/D);
    output_file_nonzeros_real_fft.write(ifftr_output_buffer_nonzeros.data(), ifftr_output_buffer_nonzeros.size());

    // Kiss FFT cleanup
    kiss_fft_cleanup();
    return 0;
}
