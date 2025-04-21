#!/usr/bin/env python3
"""
trace_parser.py

Reads your modulation output (SNR ↔ BER) and writes a PER‐vs‐SNR table for NS‑3.

Usage:
    cd ns3
    python3 trace_parser.py \
      --modulation_csv ../modulation/rate_vs_distance.csv \
      --output per_snr_table.csv \
      --packet_sizes 128,256,512
"""

import argparse
import pandas as pd
import numpy as np
import os

def main():
    p = argparse.ArgumentParser()
    p.add_argument(
        "--modulation_csv", type=str,
        default="rate_vs_distance.csv",
        help="Path to modulation/rate_vs_distance_*.csv"
    )
    p.add_argument(
        "--output", type=str, default="per_snr_table.csv",
        help="Output PER vs SNR table for NS‑3"
    )
    p.add_argument(
        "--packet_sizes", type=str, default="128,256,512",
        help="Comma‑separated list of packet sizes (bytes) to compute PER for"
    )
    args = p.parse_args()

    # parse packet sizes
    pkt_sizes = [int(x) for x in args.packet_sizes.split(",")]

    # load your modulation output
    df = pd.read_csv(args.modulation_csv)

    # ensure we have the columns we need
    if "SNR_dB" not in df.columns or "BER" not in df.columns:
        raise RuntimeError(f"{args.modulation_csv} must contain 'SNR_dB' and 'BER' columns")

    # build the PER‐vs‐SNR table
    snr_bins = np.sort(df["SNR_dB"].unique())
    out = {"SNR_dB": snr_bins}

    for ps in pkt_sizes:
        per_list = []
        for snr in snr_bins:
            # average BER at this SNR bin (if you have multiple distances/schemes)
            ber_vals = df.loc[df["SNR_dB"] == snr, "BER"].values
            if len(ber_vals) == 0:
                mean_ber = 1.0
            else:
                mean_ber = np.mean(ber_vals)
            # PER for this packet size
            per = 1.0 - (1.0 - mean_ber) ** (ps * 8)
            per_list.append(per)
        out[f"PER_{ps}B"] = per_list

    df_out = pd.DataFrame(out)

    # write it
    df_out.to_csv(args.output, index=False)
    print(f"→ Wrote PER vs SNR table to {args.output}")

if __name__ == "__main__":
    main()
