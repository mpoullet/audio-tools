/*
** Copyright (C) 1999-2012 Erik de Castro Lopo <erikd@mega-nerd.com>
**
** All rights reserved.
**
** Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**
**     * Redistributions of source code must retain the above copyright
**       notice, this list of conditions and the following disclaimer.
**     * Redistributions in binary form must reproduce the above copyright
**       notice, this list of conditions and the following disclaimer in
**       the documentation and/or other materials provided with the
**       distribution.
**     * Neither the author nor the names of any contributors may be used
**       to endorse or promote products derived from this software without
**       specific prior written permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
** TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
** PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
** CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
** EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
** PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
** OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
** WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
** OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
** ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <memory>

#include <sndfile.hh>

int main() {
    constexpr const char * filename = "sine.wav";
    constexpr double pi = 3.14159265358979323846264338;
    constexpr double freq = 440.0;
    constexpr double duration = 5;//1/freq;
    constexpr int channels = 1;
    constexpr int samplerate = 48000;
    constexpr double sample_duration = 1.0/samplerate;
    constexpr int sample_count = samplerate * duration;
    // 0x2F000000 => 788529152
    constexpr double amplitude = 1.0 * 0x2F000000;

    printf("duration=%f\n", duration);
    printf("channels=%d\n", channels);
    printf("samplerate=%d\n", samplerate);
    printf("sample_count=%d\n", sample_count);
    printf("sample_duration=%f\n", sample_duration);
    printf("amplitude=%f\n", amplitude);
    printf("freq=%f\n", freq);

    SndfileHandle sndfilehandle(filename, SFM_WRITE, (SF_FORMAT_WAV | SF_FORMAT_PCM_32), channels, samplerate);
    std::unique_ptr<int, void (*)(void*)> buffer(static_cast<int *>(std::malloc(2 * sample_count * sizeof(int))), &std::free);

    if (sndfilehandle.channels() == 1) {
        for (int k = 0; k < sample_count; k++) {
            (buffer.get())[k] = amplitude * sin(freq * 2 * k * pi / samplerate);
            printf("%d: %f: %d\n", k, sample_duration*k, (buffer.get())[k]);
        }
    } else {
        printf("Error : make_sine can only generate mono files.\n");
        return 1;
    };

    sndfilehandle.write(buffer.get(), sndfilehandle.channels() * sample_count);

    return 0;
} /* main */
