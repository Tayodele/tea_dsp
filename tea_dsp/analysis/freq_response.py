#%%
import scipy.signal as signal
import matplotlib.pyplot as plot
import numpy as np

#%%
fs = int(44.1e3) #Hz

sig_len = 2 #seconds
f_start = 100
f_stop = 10000
test_freqs = np.arange(f_start,f_stop,500)

test_sig = np.zeros(int(sig_len*fs))

for n, point in enumerate(test_sig):
  for freq in test_freqs:
    omega = 2 * np.pi * n
    delta = freq/fs
    point += np.sin(omega*delta)
  test_sig[n] = point
#%%
def plot_signal(sig):
  freq, pxx = signal.welch(sig, fs=fs, window='hann')
  fig = plot.figure()
  ax = fig.gca()
  resp_amp = 10 * np.log10(sig)
  ax.plot(resp_amp[:500])
  fig = plot.figure()
  ax = fig.gca()
  resp_amp = 10 * np.log10(abs(pxx))
  ax.plot(freq,resp_amp)
# %%
plot_signal(test_sig)

# %%

def plot_response_and_get_slope(coeffs,cutoff,calc_slope=False):
  w, h = signal.freqz(coeffs, fs=fs)
  fig = plot.figure()
  ax = fig.gca()
  figw = plot.figure()
  ax1 = figw.gca()
  resp_amp = 10 * np.log10(abs(h))
  ax.grid()
  ax1.grid()
  xvals = w
  yvals = resp_amp
  ax.set_xscale('log')
  # ax1.set_xscale('log')
  resp = ax.plot(w, resp_amp)
  ax1.plot(xvals[:15],yvals[:15])
  # slope calc
  if calc_slope:
    print(f"Cutoff {cutoff} Hz")
    approx1 = np.where(xvals>=cutoff)[0]
    ydiff = np.diff(yvals)
    try:
      approx2 = np.where(ydiff > 0)[0][0] - 1
    except IndexError:
      approx2 = yvals.shape[0]-1
    y1 = yvals[approx2]
    x1 = xvals[approx2]
    y0 = yvals[approx1[0]]
    x0 = xvals[approx1[0]]
    slope_approx = -1 * (y0-y1/(x0-x1)) #dB/Hz
    print(f"{y0} dB to {y1} dB at {x0} Hz to {x1} Hz")
    print(f"Slope Approx. dB/Hz", slope_approx)
    slope_approx = ((y0-y1)/np.log2(x1-x0)) #dB/octave
    print(f"Slope Approx. dB/octave", slope_approx)

# %%
# Simple Lowpass
cutoff = 10
ntaps = 512
coeffs = signal.firwin(ntaps,cutoff,window='blackman',fs=fs)
sig_filtered = signal.convolve(test_sig, coeffs,mode="same") 
plot_response_and_get_slope(coeffs,cutoff)
print("Num Taps", ntaps)
# %%
cutoff = 10
bands = np.array([0,cutoff,fs/2])
gains = np.array([1.,0,0.])
ntaps = 5
coeffs = signal.firwin2(ntaps,freqs,gains,fs=fs)
sig_filtered = signal.convolve(test_sig, coeffs,mode="same") 
plot_response_and_get_slope(coeffs,cutoff,True)
print("Num Taps", ntaps)
# %%
cutoff = 100
trans_width = 400
bands = np.array([0,cutoff,cutoff+trans_width,fs/2])
gains = np.array([1.,0])
ntaps = 512
coeffs = signal.remez(ntaps,bands,gains,fs=fs)
sig_filtered = signal.convolve(test_sig, coeffs,mode="same") 
plot_response_and_get_slope(coeffs,cutoff,True)
print("Num Taps", ntaps)
# %%
plot_signal(sig_filtered)
# %%
