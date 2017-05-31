clear;
close all;

% http://www.gaussianwaves.com/2014/07/how-to-plot-fft-using-matlab-fft-of-basic-signals-sine-and-cosine-waves/
% http://calculator.vhex.net/post/calculator-result/1d-discrete-fourier-transform
% http://www.phys.nsu.ru/cherk/fft.pdf
% http://csserver.evansville.edu/~richardson/courses/Tutorials/audio/AudioProcessing.pdf
% http://mirlab.org/jang/books/audioSignalProcessing/matlab4waveRead.asp?title=4-2%20Reading%20Wave%20Files

% Parameters
I = 2;
D = 1;
N = 256;

% WAV file input
[data, fs, nbits] = wavread("sine_48000_pcm32_short.wav");
sample_counts = length(data);

% Plot sound file in time domain
t_all = (1/fs)*(1:sample_counts);
title_name = 'Time domain (all samples) 48kHz';
figure('Name', title_name, 'NumberTitle', 'off');
plot(t_all, data);
xlabel('Time (s)');
ylabel('Amplitude');
title(title_name);

%
% Short sequence without overlap
%

% Plot N points input block in time domain
input = data(1:N)';
t_block = (1/fs)*(1:N);
title_name = 'Time domain (first block) 48kHz';
figure('Name', title_name, 'NumberTitle', 'off');
plot(t_block, input);
xlabel('Time (s)');
ylabel('Amplitude');
title(title_name);

% FFT
fft_data = fft(input);

% Plot N points FFT output block in frequency domain
f_block = fs*(-N/2:N/2-1)/N;
title_name = '|DFT values|';
figure('Name', title_name, 'NumberTitle', 'off');
plot(f_block, abs(fftshift(fft_data)));
xlim([-500 500]);
xlabel('Frequency (Hz)');
ylabel('|DFT Values|');

% IFFT
N1 = I/D*N;

% Method 1

% C_i = 0
ifft_input_zeros=[I/D*fft_data(1:N/2)';zeros(N1-N,1);I/D*fft_data(N/2+1:N)'];
ifft_data_zeros=ifft(ifft_input_zeros);

% Plot N1 points output block in time domain
[orig_data, orig_fs, orig_nbits] = wavread("sine_96000_pcm32_short.wav");
t_block = (1/(I/D*fs))*(1:N1);
title_name = 'Time domain (first block resampled) 96kHz | C_i=0';
figure('Name', title_name, 'NumberTitle', 'off');
plot(t_block, [real(ifft_data_zeros) orig_data(1:N1)]);
ylim([-1 1]);
xlabel('Time (s)');
ylabel('Amplitude');
title(title_name);

% Method 2

% C_i = X(N/2)
filling(1:N1-N) = fft_data(N/2);
ifft_input_nonzeros=[I/D*fft_data(1:N/2)';filling';I/D*fft_data(N/2+1:N)'];
ifft_data_nonzeros=ifft(ifft_input_nonzeros);

% Plot N1 points output block in time domain
[orig_data, orig_fs, orig_nbits] = wavread("sine_96000_pcm32_short.wav");
t_block = (1/(I/D*fs))*(1:N1);
title_name = 'Time domain (first block resampled) 96kHz | C_i=X(N/2)';
figure('Name', title_name, 'NumberTitle', 'off');
plot(t_block, [real(ifft_data_nonzeros) orig_data(1:N1)]);
ylim([-1 1]);
xlabel('Time (s)');
ylabel('Amplitude');
title(title_name);
