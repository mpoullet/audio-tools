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

#include <sndfile.h>

int main() {
    const double pi = 3.14159265358979323846264338;
    const double freq = 440.0;
    const double duration = 1/freq;
    const int channels = 1;
    const int samplerate = 48000;
    const double sample_duration = 1.0/samplerate;
    const int sample_count = samplerate * duration;
    // 0x2F000000 => 788529152
    const double amplitude = 1.0 * 0x2F000000;

    printf("duration=%f\n", duration);
    printf("channels=%d\n", channels);
    printf("samplerate=%d\n", samplerate);
    printf("sample_count=%d\n", sample_count);
    printf("sample_duration=%f\n", sample_duration);
    printf("amplitude=%f\n", amplitude);
    printf("freq=%f\n", freq);

    SNDFILE *file;
    SF_INFO sfinfo;
    int *buffer;

    if (!(buffer = static_cast<int *>(malloc(2 * sample_count * sizeof *buffer)))) {
        printf("Error : Malloc failed.\n");
        return 1;
    };

    memset(&sfinfo, 0, sizeof(sfinfo));

    sfinfo.samplerate = samplerate;
    sfinfo.frames = sample_count;
    sfinfo.channels = channels;
    sfinfo.format = (SF_FORMAT_WAV | SF_FORMAT_PCM_32);

    if (!(file = sf_open("sine.wav", SFM_WRITE, &sfinfo))) {
        printf("Error : Not able to open output file.\n");
        free(buffer);
        return 1;
    };

    if (sfinfo.channels == 1) {
        for (int k = 0; k < sample_count; k++) {
            buffer[k] = amplitude * sin(freq * 2 * k * pi / samplerate);
            printf("%d: %f: %d\n", k, sample_duration*k, buffer[k]);
        }
    } else {
        printf("Error : make_sine can only generate mono files.\n");
	sf_close(file);
	free(buffer);
        return 1;
    };

    if (sf_write_int(file, buffer, sfinfo.channels * sample_count) !=
        sfinfo.channels * sample_count)
        puts(sf_strerror(file));

    sf_close(file);
    free(buffer);
    return 0;
} /* main */
