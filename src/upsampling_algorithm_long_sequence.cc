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
    const int L = N/8;

    if (argc != 2) {
        std::cerr << "Usage: upsampling_algorithm_long_sequence <filename>\n";
        return -1;
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

    const std::string input_filename = argv[argc-1];
    SndfileHandle input_file(input_filename);
    if (input_file.channels() != 1) {
        std::cerr << "Only files with one audio channel are supported.\n";
    }

    const std::string debug_input_filename = "upsampling_algorithm_long_sequence_debug_input.wav";
    SndfileHandle debug_input_file(input_file);

    const std::string output_filename = "upsampling_algorithm_long_sequence_out.wav";
    SndfileHandle output_file(output_filename,
                              SFM_WRITE,
                              input_file.format(),
                              input_file.channels(),
                              input_file.samplerate() * I/D);

    // Input data
    std::vector<kiss_fft_scalar> buffer(N);

    // FFT input/output buffers
    std::vector<std::complex<kiss_fft_scalar>> fft_input_buffer(N);
    std::vector<std::complex<kiss_fft_scalar>> fft_output_buffer(N);

    // IFFT input/output buffers
    std::vector<std::complex<kiss_fft_scalar>> ifft_input_buffer(M);
    std::vector<std::complex<kiss_fft_scalar>> ifft_output_buffer(M);

    /* prepend 2L zeros */
    for (int i=0; i < 2*L; ++i) {
        buffer[i] = 0;
    }

    int cnt=0;
    while (sf_count_t readcount = input_file.read(buffer.data() + 2*L, N - 2*L))
    {   
        /* Store original samples */
        debug_input_file.write(buffer.data() + 2*L, std::min(static_cast<int>(readcount), N - 2*L));

        /* Create FFT input buffer: 1 block of N samples with 2*L overlap */
        for(int i=0; i < std::min(static_cast<int>(readcount) + 2*L, N); ++i) {
            fft_input_buffer[i] = std::complex<kiss_fft_scalar>(buffer[i]);
        }
        write_complex_data(fft_input_buffer, "fft_input_buffer_" + std::to_string(cnt) + ".asc");

        /* Forward N points FFT */
        kiss_fft(fwd_cfg.get(),
                reinterpret_cast<kiss_fft_cpx*>(fft_input_buffer.data()),
                reinterpret_cast<kiss_fft_cpx*>(fft_output_buffer.data()));
        write_complex_data(fft_output_buffer, "fft_output_buffer_" + std::to_string(cnt) + ".asc");

        /* Create IFFT input buffer */
        for(int i=0; i < N/2; ++i) {
            ifft_input_buffer[i] = static_cast<kiss_fft_scalar>(I/D) * fft_output_buffer[i];
        }
        for(int i=N/2; i <= M - N/2; ++i) {
            ifft_input_buffer[i] = static_cast<kiss_fft_scalar>(I/D) * fft_output_buffer[N/2];
        }
        for(int i=M - N/2 + 1; i < M; ++i) {
            ifft_input_buffer[i] = static_cast<kiss_fft_scalar>(I/D) * fft_output_buffer[i + M/2];
        }
        write_complex_data(ifft_input_buffer, "ifft_input_buffer_" + std::to_string(cnt) + ".asc");

        /* Backward M points IFFT */
        kiss_fft(inv_cfg.get(),
                 reinterpret_cast<kiss_fft_cpx*>(ifft_input_buffer.data()),
                 reinterpret_cast<kiss_fft_cpx*>(ifft_output_buffer.data()));
        std::transform(std::begin(ifft_output_buffer), std::end(ifft_output_buffer),
                       std::begin(ifft_output_buffer),
                       std::bind1st(std::multiplies<std::complex<kiss_fft_scalar>>(), 1.0/M));

        write_complex_data(ifft_output_buffer, "ifft_output_buffer_" + std::to_string(cnt) + ".asc");

        /* Discard first and last I/D*L points and store the rest of the real vector */
        std::vector<kiss_fft_scalar> res(M - 2*I/D*L);
        int j = I/D*L;
        for(int i=0; i < M - 2*I/D*L; ++i) {
            res[i] = ifft_output_buffer[j++].real();
        }
        output_file.write(res.data(), res.size());

        /* Shift vector to the left 2*L times */
        std::rotate(buffer.begin(), buffer.begin() + N - 2*L, buffer.end());

        cnt++;
    };

    kiss_fft_cleanup();
    return 0;
}
