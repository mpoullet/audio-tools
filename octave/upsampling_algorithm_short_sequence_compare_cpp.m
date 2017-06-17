fft_input_buffer_values = load('../src/fft_input_buffer.asc');
fft_input_buffer = complex(fft_input_buffer_values(:,2), fft_input_buffer_values(:,3));
cmp_fft_input = [fft_input_data, fft_input_buffer];

fft_output_buffer_values = load('../src/fft_output_buffer.asc');
fft_output_buffer = complex(fft_output_buffer_values(:,2), fft_output_buffer_values(:,3));
cmp_fft_output = [fft_output_data, fft_output_buffer];

ifft_input_buffer_values = load('../src/ifft_input_buffer.asc');
ifft_input_buffer = complex(ifft_input_buffer_values(:,2), ifft_input_buffer_values(:,3));
cmp_ifft_input = [ifft_input_data_zeros, ifft_input_buffer];

ifft_output_buffer_values = load('../src/ifft_output_buffer.asc');
ifft_output_buffer = complex(ifft_output_buffer_values(:,2), ifft_output_buffer_values(:,3));
cmp_ifft_output = [ifft_output_data_zeros, ifft_output_buffer];