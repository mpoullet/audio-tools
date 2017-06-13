// Taken from: http://stackoverflow.com/a/14537855

#include <iostream>
#include <complex>
#include <numeric>
#include <memory>
#include <type_traits>
#include <array>

#include <cmath>
#include <cstdio>
#include <cstdlib>

#ifndef M_PI
#define M_PI 3.14159265358979324
#endif

#define N 16

#define kiss_fft_scalar float
#include "kiss_fft.h"

namespace std {
    template<>
    class default_delete<std::remove_pointer<kiss_fft_cfg>::type>
    {
        public:
            void operator()(std::remove_pointer<kiss_fft_cfg>::type *p) { std::free(p); }
    };
}

int main()
{
    std::array<kiss_fft_cpx, N> in;
    std::array<kiss_fft_cpx, N> out;

    printf("Ones (complex)\n");
    for (size_t i=0; i < N; ++i) {
        in[i].r = 1;
        in[i].i = 0.0;
    }

    // RAII without default_delete
    auto cfg1 = std::unique_ptr<std::remove_pointer<kiss_fft_cfg>::type, decltype(std::free) *> {
        kiss_fft_alloc(N, 0, NULL, NULL),
        std::free
    };

    // RAII with default delete
    auto cfg2 = std::unique_ptr<std::remove_pointer<kiss_fft_cfg>::type> {
        kiss_fft_alloc(N, 0, NULL, NULL)
    };

    if (cfg1) {
        kiss_fft(cfg1.get(), in.data(), out.data());

        for (size_t i = 0; i < N; i++) {
            printf(" in[%2zu] = %+f , %+f    "
             "out[%2zu] = %+f , %+f\n",
             i, in[i].r, in[i].i, i, out[i].r, out[i].i);
        }
    } else {
        printf("not enough memory\n");
        return -1;
    }

    return 0;
}
