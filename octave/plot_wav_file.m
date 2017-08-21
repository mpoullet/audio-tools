function [x, y] = plot_wav_file(filename, title_name)
    % WAV file input
    [data, fs, nbits] = wavread(filename);
    sample_counts = length(data);

    % Plot sound file in time domain
    t_all = (1/fs)*(1:sample_counts);
    figure('Name', title_name, 'NumberTitle', 'off');
    plot(t_all, data);
    xlabel('Time (s)');
    ylabel('Amplitude');
    ylim([-1 1]);
    title(title_name);

    x = t_all;
    y = data;
end
