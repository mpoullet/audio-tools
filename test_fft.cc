// Taken from: http://stackoverflow.com/a/14537855

#include <iostream>
#include <complex>
#include <numeric>
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

int main()
{
    std::array<kiss_fft_cpx, N> in;
    std::array<kiss_fft_cpx, N> out;

    for (size_t i=0; i < N; ++i) {
        in[i].r = i;
        in[i].i = 0.0;
        //std::cout << in[i].r << "," << in[i].i << "\n";
    }

    kiss_fft_cfg cfg;
    if ((cfg = kiss_fft_alloc(N, 0, NULL, NULL)) != NULL) {
        kiss_fft(cfg, in.data(), out.data());
        free(cfg);

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
