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

% Plot N points FFT output block in frequency domain
f_block = fs*(-N/2:N/2-1)/N;
title_name = '|DFT Values|';
figure('Name', title_name, 'NumberTitle', 'off');
plot(f_block, abs(fftshift(fft_data)));
xlim([-500 500]);
xlabel('Frequency (Hz)');
ylabel('|DFT Values|');

% IFFT
N1 = I/D*N;
ifft_input=[I/D*fft_data(1:N/2)';zeros(N1-N,1);I/D*fft_data(N/2+1:N)'];
ifft_data=ifft(ifft_input);

% Plot N1 points output block in time domain
t_block = (1/(I/D*fs))*(1:N1);
title_name = 'Time Domain (first block resampled)';
figure('Name', title_name, 'NumberTitle', 'off');
plot(t_block, real(ifft_data));
ylim([-1 1]);
xlabel('Time (s)');
ylabel('Amplitude');
title(title_name);

%
% Overlap approach for long sequences
%
L = N/8;
N1 = I/D*N;
L1 = I/D*L;

% Prepare first block 2*L overlap
%input = [zeros(2*L,1)' data']'(1:N);
