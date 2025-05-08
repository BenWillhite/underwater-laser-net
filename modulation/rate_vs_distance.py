#!/usr/bin/env python3
"""
rate_vs_distance.py

Loads Meep “unit‑flux” CSVs, scales to real P_RX given P_TX, converts to
photons/bit + full noise‐floor SNR, then computes achievable rate R(d)
using real BER vs SNR curves + FEC — swept over multiple target bit‑rates.

Usage:
    python rate_vs_distance.py \
      --csv_dir ../meep/data \
      --P_TX_mW 10.0 \
      --eta_rx 0.5 \
      --params ../meep/channel_params.json \
      --ber_dir modulation/ber_curves
"""

import os
import json
import glob
import re
import argparse

import numpy as np
import pandas as pd
import matplotlib.pyplot as plt

def modulation_rate(snr_db, scheme):
    """Map shot-noise SNR to raw spectral efficiency bits/s/Hz."""
    snr_lin = 10**(snr_db/10)
    mapping = {
      "OOK":      1.0,
      "4PAM":     2.0,
      "PPM":      0.5,
      "OFDM+QAM": 5.0
    }
    return mapping.get(scheme, 0.0) * np.log2(1 + snr_lin)

def run_for_bit_rate(args, bit_rate):
    # load channel params
    with open(args.params) as f:
        cp = json.load(f)
    I_dark   = cp["I_dark"]
    NEP      = cp["NEP"]
    B        = cp["B"]
    RIN      = cp["RIN"]
    FEC_rate = cp["FEC_rate"]
    N_bg     = cp.get("N_bg", 0.0)

    # load BER curves
    ber_curves = {}
    for fn in os.listdir(args.ber_dir):
        name = os.path.splitext(fn)[0].upper()
        df   = pd.read_csv(os.path.join(args.ber_dir, fn))
        ber_curves[name] = df

    # constants
    h = 6.626e-34
    c = 3e8

    records = []
    for cf in sorted(glob.glob(f"{args.csv_dir}/flux_vs_time_λ*_d*_t*.csv")):
        m = re.search(r'λ(\d+)_d([\d\.]+)_t', cf)
        if not m: continue
        lam_nm = float(m.group(1))
        d_m    = float(m.group(2))

        flux_df   = pd.read_csv(cf)
        flux_unit = flux_df["flux"].iloc[-1]

        # 2a) scale to real watts
        P_RX = flux_unit * (args.P_TX_mW / 1e3)

        # 2b) photons-per-bit
        λ_m    = lam_nm*1e-9
        E_ph   = h*c/λ_m
        T_bit  = 1.0/bit_rate
        N_ph   = (P_RX * T_bit)/E_ph
        N_det  = args.eta_rx * N_ph

        # 2c) full noise-floor SNR
        N_dark = I_dark * T_bit / E_ph
        N_th   = (NEP**2 * B) * T_bit / E_ph**2
        N_rin  = RIN * N_ph
        total_noise = N_dark + N_th + N_rin + N_bg
        snr_linear = N_det / np.sqrt(N_det + total_noise)
        snr_db = 10 * np.log10(snr_linear + 1e-20)

        # loop schemes & BER lookup
        for scheme, df_ber in ber_curves.items():
            ber = np.interp(snr_db, df_ber["SNR_dB"], df_ber["BER"])
            if ber <= 1e-6:
                raw_se = modulation_rate(snr_db, scheme)      # bits/s/Hz
                rate   = FEC_rate * raw_se * (B / 1e6)         # now uses full bandwidth B [Hz]
            else:
                rate = 0.0


            records.append((
                lam_nm, d_m, scheme, snr_db, rate,
                flux_unit, P_RX, N_ph, N_det,
                N_dark, N_th, N_rin, N_bg, ber
            ))

    cols = [
      "lambda_nm","distance_m","mod_scheme","SNR_dB","rate_Mbps",
      "unit_flux","P_RX_W","photons_per_bit","detected_electrons_per_bit",
      "N_dark","N_th","N_rin","N_bg","BER"
    ]
    df_out = pd.DataFrame(records, columns=cols)

    # output CSV per bit‐rate
    out_csv = f"modulation/rate_vs_distance_{int(bit_rate/1e6)}Mbps.csv"
    df_out.to_csv(out_csv, index=False)
    print(f"→ wrote {out_csv}")

    # plotting
    lam_ref = 532.0
    df_ref  = df_out[df_out["lambda_nm"]==lam_ref]

    os.makedirs("modulation/plots", exist_ok=True)
    plt.figure()
    for sch, grp in df_ref.groupby("mod_scheme"):
        grp = grp.sort_values("distance_m")
        plt.plot(grp["distance_m"], grp["rate_Mbps"], 'o-', label=sch)
    plt.xlabel("Distance (m)")
    plt.ylabel("Rate (Mbps)")
    plt.title(f"Rate vs Distance @ λ={int(lam_ref)} nm, "
              f"R_b={int(bit_rate/1e6)} Mbps")
    plt.grid(True)
    plt.legend()
    out_png = f"modulation/plots/rate_vs_distance_{int(bit_rate/1e6)}Mbps.png"
    plt.tight_layout()
    plt.savefig(out_png)
    plt.close()
    print(f"→ plotted {out_png}")

def main():
    p = argparse.ArgumentParser()
    p.add_argument("--csv_dir", type=str, required=True,
                   help="Directory of flux_vs_time_λ*_d*_t*.csv")
    p.add_argument("--P_TX_mW", type=float, default=10.0,
                   help="Transmit power [mW]")
    p.add_argument("--eta_rx",  type=float, default=0.5,
                   help="Quantum efficiency")
    p.add_argument("--params",  type=str, required=True,
                   help="Path to channel_params.json")
    p.add_argument("--ber_dir", type=str, required=True,
                   help="Directory of BER vs SNR CSVs")
    args = p.parse_args()

    # sweep these bit‐rates (in bits/s)
    bit_rates = [10e6, 20e6, 30e6, 40e6, 50e6]

    for Rb in bit_rates:
        run_for_bit_rate(args, Rb)

if __name__=="__main__":
    main()
