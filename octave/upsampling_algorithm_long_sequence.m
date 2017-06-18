% Sampling Rate Conversion in the Frequency Domain [DSP Tips and Tricks]
% Guoan Bi and Sanjit K. Mitra
% IEEE Signal Processing Magazine ( Volume: 28, Issue: 3, May 2011 )
% Page(s): 140 - 144
% DOI: 10.1109/MSP.2011.940413

clc;
clear;
close all;

% We need the buffer function:
% MATLAB => Signal Processing Toolbox (https://www.mathworks.com/help/signal/ref/buffer.html)
% Octave => signal package

if isOctave
% (outside octave) sudo apt-get install liboctave-dev
% (inside octave)  pkg install -forge control
% (inside octave)  pkg install -forge signal
pkg load signal
end

% Parameters
I = 2;
D = 1;
N = 256;

% WAV file input
[data, fs, nbits] = wavread('sine_48000_pcm32_long.wav');
sample_counts = length(data);

% Plot sound file in time domain
t_all = (1/fs)*(1:sample_counts);
title_name = 'Time Domain (all samples) 48kHz';
figure('Name', title_name, 'NumberTitle', 'off');
plot(t_all, data);
xlabel('Time (s)');
ylabel('Amplitude');
title(title_name);

%
% Overlap approach for long sequences
%
N1 = I/D*N;

L = N/8;
L1 = I/D*L;

% Prepare input blocks with 2*L overlap
fft_input = buffer(data, N, 2*L);
% N points FFT
fft_result = fft(fft_input);

% Method 1

% C_i = 0
filling_zeros = zeros(N1-N, size(fft_input, 2));
ifft_input = [I/D*fft_result(1:N/2,:);filling_zeros;I/D*fft_result(N/2+1:N,:)];
% N1 points IFFT
ifft_result=real(ifft(ifft_input));
% Remove both L1 points ends
ifft_result_cut_overlap = ifft_result(L1+1:N1-L1,:);
% Resampled data
resampled_data = ifft_result_cut_overlap(:);

% Plot resampled sound file in time domain
[orig_data, orig_fs, orig_nbits] = wavread('sine_96000_pcm32_long.wav');
t_all_resampled = (1/(I/D*fs))*(1:length(orig_data));
title_name = 'Time Domain (resampled) 96kHz | C_i=0';
figure('Name', title_name, 'NumberTitle', 'off');
plot(t_all_resampled, [ resampled_data(1:length(orig_data)) orig_data(1:length(orig_data)) ]);
xlabel('Time (s)');
ylabel('Amplitude');
ylim([-1 1]);
title(title_name);

% Method 2

% C_i = X(N/2)
filling_nonzeros = bsxfun(@times, ones(N1-N, size(fft_input, 2)), fft_result(N/2,:));
ifft_input = [I/D*fft_result(1:N/2,:);filling_nonzeros;I/D*fft_result(N/2+1:N,:)];
% N1 points IFFT
ifft_result=real(ifft(ifft_input));
% Remove both L1 points ends
ifft_result_cut_overlap = ifft_result(L1+1:N1-L1,:);
% Resampled data
resampled_data = ifft_result_cut_overlap(:);

% Plot resampled sound file in time domain
[orig_data, orig_fs, orig_nbits] = wavread('sine_96000_pcm32_long.wav');
t_all_resampled = (1/(I/D*fs))*(1:length(orig_data));
title_name = 'Time Domain (resampled) 96kHz | C_i=X(N/2)';
figure('Name', title_name, 'NumberTitle', 'off');
plot(t_all_resampled, [ resampled_data(1:length(orig_data)) orig_data(1:length(orig_data)) ]);
xlabel('Time (s)');
ylabel('Amplitude');
ylim([-1 1]);
title(title_name);


% Print all samples of a vector along with their index
%for k=1:length(resampled_data)
%    sprintf('%d: %s',k, num2str(resampled_data(k)))
%end
