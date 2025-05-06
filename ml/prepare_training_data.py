#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import glob
import re
import pandas as pd

# --- CONFIGURATION ---
# Number of past α samples to use
T = 50
# Number of future flux samples per example
M = 100

# Paths
ALPHA_CSV = "../foam/2_post/alpha_depth_time.csv"
MEEP_PATTERN = "../meep/data/flux_vs_time_λ*_d*_t*.csv"
OUTPUT_CSV = "training.csv"


def load_alpha_window(alpha_csv, window_size):
    # Read α CSV and average over depth for each time step
    df = pd.read_csv(alpha_csv)
    # Assumes columns: time, depth, alpha
    alpha_time = df.groupby("time")["alpha_m-1"].mean().reset_index()
    # Take the last `window_size` entries
    recent = alpha_time.tail(window_size)
    return recent["alpha_m-1"].values  # shape (T,)


def parse_meep_filename(path):
    # Extract λ and d from name like flux_vs_time_λ532_d2.00_t1.50.csv
    m = re.search(r"λ(\d+)_d([\d\.]+)_t", path)
    lam = int(m.group(1))
    dist = float(m.group(2))
    return lam, dist


def main():
    # 1) Load the α window
    alpha_window = load_alpha_window(ALPHA_CSV, T)

    rows = []
    for csv_path in glob.glob(MEEP_PATTERN):
        lam, dist = parse_meep_filename(csv_path)
        df = pd.read_csv(csv_path)

        # --- automatically find the flux column (anything but "time") ---
        flux_cols = [c for c in df.columns if c.lower() != "time"]
        if len(flux_cols) != 1:
            raise RuntimeError(
                f"Expected 1 flux column in {csv_path}, found {flux_cols}")
        flux = df[flux_cols[0]].values  # now picks up your actual header name

        # Build one row with alpha features, link params, then flux labels
        row = {}
        for i, a in enumerate(alpha_window):
            row[f"alpha_{i}"] = a
        row["lambda_nm"] = lam
        row["distance_m"] = dist
        for j, f in enumerate(flux[:M]):
            row[f"flux_{j}"] = f

        rows.append(row)

    # 2) Create DataFrame and save
    train_df = pd.DataFrame(rows)
    train_df.to_csv(OUTPUT_CSV, index=False)
    print(f"Saved training data with {len(rows)} examples to {OUTPUT_CSV}")


if __name__ == "__main__":
    main()
