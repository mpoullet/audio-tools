#include <iostream>
#include <cmath>
#include <array>

#include <sndfile.hh>

int main() {
    constexpr double pi = 3.14159265358979323846264338;
    constexpr double freq = 440.0;
    constexpr double duration = 1.0;
    constexpr int samplerate = 48000;
    constexpr double sample_duration = 1.0/samplerate;
    constexpr int sample_count = samplerate * duration;

    std::cout << "duration="        << duration << "\n";
    std::cout << "samplerate="      << samplerate << "\n";
    std::cout << "sample_count="    << sample_count << "\n";
    std::cout << "sample_duration=" << sample_duration << "\n";
    std::cout << "freq="            << freq;
    std::cout << std::endl;

    std::array<double, sample_count> buffer;
    for (int k = 0; k < sample_count; k++) {
        buffer[k] = sin(freq * 2 * k * pi / samplerate);
        //std::cout << k << ": " << sample_duration*k << ": " << buffer[k] << "\n";
    }

    SndfileHandle sndfilehandle_pcm32("sine32.wav", SFM_WRITE, (SF_FORMAT_WAV | SF_FORMAT_PCM_32), 1, samplerate);
    SndfileHandle sndfilehandle_pcm24("sine24.wav", SFM_WRITE, (SF_FORMAT_WAV | SF_FORMAT_PCM_24), 1, samplerate);
    SndfileHandle sndfilehandle_pcm16("sine16.wav", SFM_WRITE, (SF_FORMAT_WAV | SF_FORMAT_PCM_16), 1, samplerate);
    SndfileHandle sndfilehandle_pcmf ("sinef.wav",  SFM_WRITE, (SF_FORMAT_WAV | SF_FORMAT_FLOAT),  1, samplerate);
    SndfileHandle sndfilehandle_pcmd ("sined.wav",  SFM_WRITE, (SF_FORMAT_WAV | SF_FORMAT_DOUBLE), 1, samplerate);

    sndfilehandle_pcm32.write(buffer.data(), sample_count);
    sndfilehandle_pcm24.write(buffer.data(), sample_count);
    sndfilehandle_pcm16.write(buffer.data(), sample_count);
    sndfilehandle_pcmf.write (buffer.data(), sample_count);
    sndfilehandle_pcmd.write (buffer.data(), sample_count);

    return 0;
}
