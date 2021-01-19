o_sig = dlmread("data.txt");

Fs = 44.1e3;
L = length(o_sig);
y = fft(o_sig);
P2 = abs(y/L);
P1 = P2(1:L/2+1);
P1(2:end-1) = 2*P1(2:end-1);
f = Fs*(0:(L/2))/L;
plot(f,P1);