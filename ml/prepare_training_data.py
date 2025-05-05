#prepare_training_data.py
"""
prepare_training_data.py

Gathers Meep flux‐vs‐time CSVs and the recent alpha window
into one wide training table: 
   [α_0 … α_{T-1}, λ, d, flux_0 … flux_{M-1}]
"""

import glob
import re
import pandas as pd

# --- CONFIGURATION ---
# Number of past α samples to use
T = 50  
# Number of future flux samples per example
M = 100  

# Paths
ALPHA_CSV = "foam/2_post/alpha_depth_time.csv"
MEEP_PATTERN = "meep/data/flux_vs_time_λ*_d*_t*.csv"
OUTPUT_CSV = "data/training.csv"


def load_alpha_window(alpha_csv, window_size):
    # Read α CSV and average over depth for each time step
    df = pd.read_csv(alpha_csv)
    # Assumes columns: time, depth, alpha
    alpha_time = df.groupby("time")["alpha"].mean().reset_index()
    # Take the last `window_size` entries
    recent = alpha_time.tail(window_size)
    return recent["alpha"].values  # shape (T,)


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
        flux = df["scaled_flux"].values  # shape (M,)

        # Build one row: α_0…α_{T-1}, λ, d, flux_0…flux_{M-1}
        row = {}
        # past α features
        for i, a in enumerate(alpha_window):
            row[f"alpha_{i}"] = a
        # link parameters
        row["lambda_nm"] = lam
        row["distance_m"] = dist
        # future flux labels
        for j, f in enumerate(flux[:M]):
            row[f"flux_{j}"] = f

        rows.append(row)

    # 2) Create DataFrame and save
    train_df = pd.DataFrame(rows)
    train_df.to_csv(OUTPUT_CSV, index=False)
    print(f"Saved training data with {len(rows)} examples to {OUTPUT_CSV}")


if __name__ == "__main__":
    main()
