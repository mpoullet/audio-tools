fft_input_buffer = read_complex_file('../src/fft_input_buffer.asc');
cmp_fft_input = [fft_input_data, fft_input_buffer];

fft_output_buffer = read_complex_file('../src/fft_output_buffer.asc');
cmp_fft_output = [fft_output_data, fft_output_buffer];

ifft_input_buffer = read_complex_file('../src/ifft_input_buffer.asc');
cmp_ifft_input = [ifft_input_data_zeros, ifft_input_buffer];

ifft_output_buffer = read_complex_file('../src/ifft_output_buffer.asc');
cmp_ifft_output = [ifft_output_data_zeros, ifft_output_buffer];
