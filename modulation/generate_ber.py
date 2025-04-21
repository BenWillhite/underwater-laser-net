#!/usr/bin/env python3
import os
import numpy as np
import pandas as pd
from scipy.special import erfc

# output directory
outdir = os.path.join(os.path.dirname(__file__), "ber_curves")
os.makedirs(outdir, exist_ok=True)

# 1) define SNR grid in dB and linear
snr_db = np.linspace(0, 50, 501)
snr    = 10 ** (snr_db/10)

# 2) OOK (binary on–off keying)
ber_ook = 0.5 * erfc(np.sqrt(snr/2))
pd.DataFrame({"SNR_dB": snr_db, "BER": ber_ook}) \
  .to_csv(os.path.join(outdir, "ook.csv"), index=False)

# 3) 4‑PAM
M = 4
factor_pam = (2*(M-1)) / (M * np.log2(M))
arg_pam    = np.sqrt(3 * np.log2(M)/(M**2 - 1) * snr)
ber_4pam   = factor_pam * erfc(arg_pam)
pd.DataFrame({"SNR_dB": snr_db, "BER": ber_4pam}) \
  .to_csv(os.path.join(outdir, "m-pam.csv"), index=False)

# 4) 4‑PPM (coherent orthogonal M‑ary PPM, M=4)
M_ppm = 4
# BER ≈ (M/(2·log2(M))) · erfc( sqrt( (snr·log2(M)) / 2 ) )
ber_ppm = (M_ppm/(2*np.log2(M_ppm))) * erfc(np.sqrt((snr*np.log2(M_ppm))/2))
pd.DataFrame({"SNR_dB": snr_db, "BER": ber_ppm}) \
  .to_csv(os.path.join(outdir, "ppm.csv"), index=False)

# 5) OFDM+QAM approximated by 16‑QAM BER
M = 16
factor_qam = (4*(1 - 1/np.sqrt(M))) / np.log2(M)
arg_qam    = np.sqrt(3 * np.log2(M)/(2*(M-1)) * snr)
ber_16qam  = factor_qam * erfc(arg_qam)
pd.DataFrame({"SNR_dB": snr_db, "BER": ber_16qam}) \
  .to_csv(os.path.join(outdir, "ofdm+qam.csv"), index=False)

print("✔️ BER curves generated in", outdir)
