function complex_vector = read_complex_file(filename)
    file = fopen(filename);
    values = textscan(file, '(%f,%f)');
    complex_vector = complex(values{1,1},values{1,2});
end
