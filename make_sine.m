% Sine wave frequency:
Fc = 440;                     % hertz
% Time specifications:
Fs = 48000;                   % samples per second
dt = 1/Fs;                    % seconds per sample
StopTime = 1/Fc;              % seconds
t = (0:dt:StopTime-dt)';      % seconds

% Generate sine wave
x = sin(2*pi*Fc*t);

% Plot the signal versus time:
figure;
plot(t,x);
xlabel('time (in seconds)');
title('Signal versus Time');
zoom xon;
