#include <algorithm>
#include <cmath>
#include <iostream>
#include <numeric>
#include <vector>

#include <sndfile.hh>

int main ()
{
    const auto pi{3.14159265358979323846264338};
    const auto freq{440.0};
    const auto duration{1.0};
    const auto samplerate{48000};
    const auto sample_duration{1.0 / samplerate};
    const int sample_count = samplerate * std::round (duration);

    std::cout << "duration=" << duration << "\n";
    std::cout << "samplerate=" << samplerate << "\n";
    std::cout << "sample_count=" << sample_count << "\n";
    std::cout << "sample_duration=" << sample_duration << "\n";
    std::cout << "freq=" << freq;
    std::cout << std::endl;

    std::vector<double> buffer (sample_count);
    std::iota (std::begin (buffer), std::end (buffer), 0);
    std::transform (std::begin (buffer), std::end (buffer), std::begin (buffer),
                    [&](auto k) { return sin (freq * 2 * k * pi / samplerate); });

    std::string filename_prefix = "sine_" + std::to_string (freq) + "_" + std::to_string (samplerate);
    SndfileHandle sndfilehandle_pcm32 (filename_prefix + "_pcm32.wav",
                                       SFM_WRITE,
                                       (SF_FORMAT_WAV | SF_FORMAT_PCM_32), 1, samplerate);
    SndfileHandle sndfilehandle_pcm24 (filename_prefix + "_pcm24.wav",
                                       SFM_WRITE,
                                       (SF_FORMAT_WAV | SF_FORMAT_PCM_24), 1, samplerate);
    SndfileHandle sndfilehandle_pcm16 (filename_prefix + "_pcm16.wav",
                                       SFM_WRITE,
                                       (SF_FORMAT_WAV | SF_FORMAT_PCM_16), 1, samplerate);
    SndfileHandle sndfilehandle_pcmf (filename_prefix + "_float.wav",
                                      SFM_WRITE,
                                      (SF_FORMAT_WAV | SF_FORMAT_FLOAT), 1, samplerate);
    SndfileHandle sndfilehandle_pcmd (filename_prefix + "_double.wav",
                                      SFM_WRITE,
                                      (SF_FORMAT_WAV | SF_FORMAT_DOUBLE), 1, samplerate);

    sndfilehandle_pcm32.write (buffer.data (), buffer.size ());
    sndfilehandle_pcm24.write (buffer.data (), buffer.size ());
    sndfilehandle_pcm16.write (buffer.data (), buffer.size ());
    sndfilehandle_pcmf.write (buffer.data (), buffer.size ());
    sndfilehandle_pcmd.write (buffer.data (), buffer.size ());
}
