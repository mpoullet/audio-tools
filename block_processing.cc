#include <iostream>
#include <algorithm>
#include <vector>

#include <sndfile.hh>

int main()
{
    const int N = 256;
    const int L = N/8;
    const std::string filename = "sine_48000_pcm32.wav";
    SndfileHandle  infile(filename);

    std::vector<double> buffer(N);

    /* prepend 2L zeros */
    for (int i=0; i < 2*L; ++i) {
        buffer[i] = 0;
    }

    while (sf_count_t readcount = infile.read(buffer.data() + 2*L, N-2*L))
    {   
        for (int i=0; i < readcount; ++i) {
            std::cout << i << " " << buffer[i] << "\n";
        }
        std::cout << "\n\n";
        std::rotate(buffer.begin(), buffer.begin() + N-2*L, buffer.end());
    };

    return 0;
}
