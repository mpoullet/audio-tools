% Compare Octave and C++ values
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
plot_wav_file('../src/upsampling_algorithm_short_sequence_out_zeros.wav', 'C++: Time domain (first block resampled) (complex FFT) 96kHz | C_i = 0');
plot_wav_file('../src/upsampling_algorithm_short_sequence_out_zeros_real_fft.wav', 'C++: Time domain (first block resampled) (real FFT) 96kHz | C_i = 0');
plot_wav_file('../src/upsampling_algorithm_short_sequence_out_nonzeros.wav', 'C++: Time domain (first block resampled) 96kHz (complex FFT) | C_i = X(N/2)');
plot_wav_file('../src/upsampling_algorithm_short_sequence_out_nonzeros_real_fft.wav', 'C++: Time domain (first block resampled) 96kHz (real FFT) | C_i = X(N/2)');
