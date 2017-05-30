clear;
close all;

% http://www.gaussianwaves.com/2014/07/how-to-plot-fft-using-matlab-fft-of-basic-signals-sine-and-cosine-waves/

% Parameters
I = 2;
D = 1;
N = 256;

% WAV file input
[data, fs, nbits] = wavread("sine_48000_pcm32.wav");
sample_counts = length(data);

% Plot sound file in time domain
t_all = (1/fs)*(1:sample_counts);
title_name = 'Time Domain (all samples)';
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
title_name = 'Time Domain (first block)';
figure('Name', title_name, 'NumberTitle', 'off');
plot(t_block, input);
xlabel('Time (s)');
ylabel('Amplitude');
title(title_name);

% FFT
fft_data = fft(input);
f_block = fs*(-N/2:N/2-1)/N;

% Plot N points FFT output block in frequency domain
title_name = '|DFT Values|';
figure('Name', title_name, 'NumberTitle', 'off');
plot(f_block, abs(fftshift(fft_data)));
xlim([0 1000]);
xlabel('Frequency (Hz)');
ylabel('|DFT Values|');

%
% Overlap approach for long sequences
%
L = N/8;
N1 = I/D*N;
L1 = I/D*L;

% Prepare first block 2*L overlap
%input = [zeros(2*L,1)' data']'(1:N);
