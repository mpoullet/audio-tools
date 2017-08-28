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
#include "kiss_fftr.h"

namespace {
    const auto precision = []() -> std::streamsize {
        // https://www.working-software.com/cpp-floats-as-decimal
        if (std::is_same<kiss_fft_scalar, float>::value)
            return 9;
        if (std::is_same<kiss_fft_scalar, double>::value)
            return 17;
        return std::cout.precision();
    }();

    const auto format = []() {
            if (std::is_same<kiss_fft_scalar, float>::value)
                return SF_FORMAT_WAV | SF_FORMAT_FLOAT;
            if (std::is_same<kiss_fft_scalar, double>::value)
                return SF_FORMAT_WAV | SF_FORMAT_DOUBLE;
            return SF_FORMAT_WAV | SF_FORMAT_PCM_16;
    }();
}

int main(int argc, char *argv[])
{
    const auto I { 2 };
    const auto D { 1 };
    const auto N { 256 };
    const auto M { I/D*N };
    const auto L { N/8 };

    if (argc != 2) {
        std::cerr << "Usage: upsampling_algorithm_long_sequence <filename>\n";
        return -1;
    }

    const std::string input_filename { argv[argc-1] };
    SndfileHandle input_file(input_filename);
    if (input_file.channels() != 1) {
        std::cerr << "Only files with one audio channel are supported.\n";
        return -1;
    }

    // Kiss FFT configurations
    const auto fwd_cfg = std::unique_ptr<std::remove_pointer_t<kiss_fftr_cfg>, decltype(kiss_fft_free) *> {
        kiss_fftr_alloc(N, 0, nullptr, nullptr),
        kiss_fft_free
    };

    const auto inv_cfg = std::unique_ptr<std::remove_pointer_t<kiss_fftr_cfg>, decltype(kiss_fft_free) *> {
        kiss_fftr_alloc(M, 1, nullptr, nullptr),
        kiss_fft_free
    };

    if (fwd_cfg == nullptr || inv_cfg == nullptr) {
        std::cerr << "Error allocating Kiss FFT configurations.\n";
        return -1;
    }

    const auto output_filename = input_filename.substr(0, input_filename.rfind(".")) + "_upsampled2x.wav";
    SndfileHandle output_file(output_filename, SFM_WRITE,
                              format, input_file.channels(), input_file.samplerate() * I/D);

    // Buffers
    std::vector<kiss_fft_scalar>               fft_input_buffer(N);
    std::vector<std::complex<kiss_fft_scalar>> tmp_buffer(M/2 + 1);
    std::vector<kiss_fft_scalar>               ifft_output_buffer(M);

    size_t cnt=0;
    while (sf_count_t readcount = input_file.read(fft_input_buffer.data() + 2*L, N - 2*L))
    {
        if (readcount < N - 2*L) {
            std::fill(std::begin(fft_input_buffer) + 2*L + readcount, std::end(fft_input_buffer), 0);
        }

        // Forward N points real FFT
        kiss_fftr(fwd_cfg.get(),
                  reinterpret_cast<kiss_fft_scalar*>(fft_input_buffer.data()),
                  reinterpret_cast<kiss_fft_cpx*>(tmp_buffer.data()));

        // Create IFFT input buffer
        const auto C_i = static_cast<kiss_fft_scalar>(1.0 * D/I) * std::complex<kiss_fft_scalar>(tmp_buffer[N/2]);
        std::fill(std::begin(tmp_buffer) + N/2 + 1, std::end(tmp_buffer), C_i);

        // Backward M points IFFT
        kiss_fftri(inv_cfg.get(),
                   reinterpret_cast<kiss_fft_cpx*>(tmp_buffer.data()),
                   reinterpret_cast<kiss_fft_scalar*>(ifft_output_buffer.data()));

        // Discard first and last I/D*L points (normal case)
        // Discard padding zeros at the beginning / end (special cases)
        const auto nb_elements_to_discard_front = (cnt == 0)            ? I/D*2*L                     : I/D*L;
        const auto nb_elements_to_discard_back  = (readcount < N - 2*L) ? I/D*(N - (2*L + readcount)) : I/D*L;
        std::vector<kiss_fft_scalar> output_buffer(std::begin(ifft_output_buffer) + nb_elements_to_discard_front,
                                                     std::end(ifft_output_buffer) - nb_elements_to_discard_back);

        // 1 - 1.0/M : make the ifft/fft pair unitary
        // 2 - I/D : scale factor due to resampling
        // 3 - clipping to [-1;1]
        std::transform(std::begin(output_buffer), std::end(output_buffer),
                       std::begin(output_buffer),
                       [](const kiss_fft_scalar& scalar) {
                           kiss_fft_scalar max_val = 1.0;
                           kiss_fft_scalar val = scalar * 1.0/M * I/D;
                           return std::max(-max_val, std::min(val, max_val));
                       });

        // Store upsampled samples
        output_file.write(output_buffer.data(), output_buffer.size());

        // Shift vector to the left 2*L times
        std::rotate(fft_input_buffer.begin(), fft_input_buffer.begin() + N - 2*L, fft_input_buffer.end());

        cnt++;
    };

    kiss_fft_cleanup();
    return 0;
}
