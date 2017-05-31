% Sampling Rate Conversion in the Frequency Domain [DSP Tips and Tricks]
% Guoan Bi and Sanjit K. Mitra
% IEEE Signal Processing Magazine ( Volume: 28, Issue: 3, May 2011 )
% Page(s): 140 - 144
% DOI: 10.1109/MSP.2011.940413

clear;
close all;

% Parameters
I = 2;
D = 1;
N = 256;

% WAV file input
[data, fs, nbits] = wavread("sine_48000_pcm32_long.wav");
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
% Overlap approach for long sequences
%
L = N/8;
L1 = I/D*L;

% Prepare first block 2*L overlap
input = [zeros(2*L,1)' data']'(1:N);
