import numpy as np
import matplotlib.pyplot as plt
from scipy.fft import fft, fftfreq

N1 = 512
N = 64
K = 27
k = np.arange(N)
n = np.arange(start=-N/2, stop=N/2, step=1)

#Blackman window
wk = 0.42 - 0.5 * np.cos(2 * np.pi * k / (N - 1)) + 0.08 * np.cos(4 * np.pi * k / (N - 1))

#Appending zeros after 64 for better frequency visualization
hk = np.zeros(N1)

#Low Pass filter cofficients
hk[0:N] = (1/N)*(np.sin((np.pi)*n*K/N)/np.sin((np.pi)*n/N))
hk[32] = hk[31]

for i in range(N):
    hk[i] = hk[i]*wk[i]

yf = fft(hk)
xf = np.arange(start=0, stop=N1, step=1)
plt.plot(xf, np.abs(yf))
#plt.plot(k, wk)
plt.title("FIR Low Pass Filter with cutoff frequency - 10 Khz")
plt.xlabel("Sample Index")
plt.ylabel("Amplitude")
plt.show()
