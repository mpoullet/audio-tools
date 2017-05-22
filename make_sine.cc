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

#include <cstdio>
#include <cmath>
#include <array>

#include <sndfile.hh>

int main() {
    constexpr double pi = 3.14159265358979323846264338;
    constexpr double freq = 440.0;
    constexpr double duration = 5;//1/freq;
    constexpr int channels = 1;
    constexpr int samplerate = 48000;
    constexpr double sample_duration = 1.0/samplerate;
    constexpr int sample_count = samplerate * duration;

    printf("duration=%f\n", duration);
    printf("channels=%d\n", channels);
    printf("samplerate=%d\n", samplerate);
    printf("sample_count=%d\n", sample_count);
    printf("sample_duration=%f\n", sample_duration);
    printf("freq=%f\n", freq);

    SndfileHandle sndfilehandle_pcm32("sine32.wav", SFM_WRITE, (SF_FORMAT_WAV | SF_FORMAT_PCM_32), channels, samplerate);
    SndfileHandle sndfilehandle_pcm24("sine24.wav", SFM_WRITE, (SF_FORMAT_WAV | SF_FORMAT_PCM_24), channels, samplerate);
    SndfileHandle sndfilehandle_pcm16("sine16.wav", SFM_WRITE, (SF_FORMAT_WAV | SF_FORMAT_PCM_16), channels, samplerate);
    SndfileHandle sndfilehandle_pcmf ("sinef.wav",  SFM_WRITE, (SF_FORMAT_WAV | SF_FORMAT_FLOAT),  channels, samplerate);
    SndfileHandle sndfilehandle_pcmd ("sined.wav",  SFM_WRITE, (SF_FORMAT_WAV | SF_FORMAT_DOUBLE), channels, samplerate);
    std::array<double, channels * sample_count> buffer;

    for (int k = 0; k < sample_count; k++) {
        buffer[k] = sin(freq * 2 * k * pi / samplerate);
        printf("%d: %f: %f\n", k, sample_duration*k, buffer[k]);
    }

    sndfilehandle_pcm32.write(buffer.data(), sndfilehandle_pcm32.channels() * sample_count);
    sndfilehandle_pcm24.write(buffer.data(), sndfilehandle_pcm24.channels() * sample_count);
    sndfilehandle_pcm16.write(buffer.data(), sndfilehandle_pcm16.channels() * sample_count);
    sndfilehandle_pcmf.write (buffer.data(), sndfilehandle_pcmf.channels()  * sample_count);
    sndfilehandle_pcmd.write (buffer.data(), sndfilehandle_pcmd.channels()  * sample_count);

    return 0;
} /* main */
