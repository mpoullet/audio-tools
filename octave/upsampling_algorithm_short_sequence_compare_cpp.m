fft_input_buffer = read_complex_file('../src/fft_input_buffer.asc');
cmp_fft_input = [fft_input_data, fft_input_buffer];

fft_output_buffer = read_complex_file('../src/fft_output_buffer.asc');
cmp_fft_output = [fft_output_data, fft_output_buffer];

ifft_input_buffer_zeros = read_complex_file('../src/ifft_input_buffer_zeros.asc');
cmp_ifft_input_zeros = [ifft_input_data_zeros, ifft_input_buffer_zeros];

ifft_output_buffer_zeros = read_complex_file('../src/ifft_output_buffer_zeros.asc');
cmp_ifft_output_zeros = [real(ifft_output_data_zeros), real(ifft_output_buffer_zeros)];

ifft_input_buffer_nonzeros = read_complex_file('../src/ifft_input_buffer_nonzeros.asc');
cmp_ifft_input_nonzeros = [ifft_input_data_nonzeros, ifft_input_buffer_nonzeros];

ifft_output_buffer_nonzeros = read_complex_file('../src/ifft_output_buffer_nonzeros.asc');
cmp_ifft_output_nonzeros = [real(ifft_output_data_nonzeros), real(ifft_output_buffer_nonzeros)];

% Plot C++ generated sinewaves

% WAV file input
[data, fs, nbits] = wavread('../src/out_zeros.wav');
sample_counts = length(data);

% Plot sound file in time domain
t_all = (1/fs)*(1:sample_counts);
title_name = 'C++: Time domain (first block resampled) 96kHz | C_i = 0';
figure('Name', title_name, 'NumberTitle', 'off');
plot(t_all, data);
xlabel('Time (s)');
ylabel('Amplitude');
ylim([-1 1]);
title(title_name);

% WAV file input
[data, fs, nbits] = wavread('../src/out_nonzeros.wav');
sample_counts = length(data);

% Plot sound file in time domain
t_all = (1/fs)*(1:sample_counts);
title_name = 'C++: Time domain (first block resampled) 96kHz | C_i = X(N/2)';
figure('Name', title_name, 'NumberTitle', 'off');
plot(t_all, data);
xlabel('Time (s)');
ylabel('Amplitude');
ylim([-1 1]);
title(title_name);
