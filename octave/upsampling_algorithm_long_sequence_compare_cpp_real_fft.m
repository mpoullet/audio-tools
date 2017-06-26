filename_prefix = '../src/fft_input_buffer_';
fft_input_buffer = zeros(256, 10);
for i=1:10
    filename = [filename_prefix num2str(i-1) '.asc'];
    fft_input_buffer(:,i) = load(filename);
endfor

filename_prefix = '../src/fft_output_buffer_';
fft_output_buffer = zeros(256, 10);
for i=1:10
    filename = [filename_prefix num2str(i-1) '.asc'];
    fft_output_buffer(1:256/2+1,i) = read_complex_file(filename);
endfor

filename_prefix = '../src/ifft_input_buffer_';
ifft_input_buffer = zeros(512, 10);
for i=1:10
    filename = [filename_prefix num2str(i-1) '.asc'];
    ifft_input_buffer(1:512/2+1,i) = read_complex_file(filename);
endfor

filename_prefix = '../src/ifft_output_buffer_';
ifft_output_buffer = zeros(512, 10);
for i=1:10
    filename = [filename_prefix num2str(i-1) '.asc'];
    ifft_output_buffer(:,i) = load(filename);
endfor

% Plot C++ generated sinewave
plot_wav_file('../src/upsampling_algorithm_long_sequence_out_real_fft.wav' , 'C++: Time domain 96kHz (real FFT) | C_i = X(N/2)');
