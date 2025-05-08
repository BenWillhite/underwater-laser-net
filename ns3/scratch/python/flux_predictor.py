#!/usr/bin/env python3
import argparse
import pandas as pd
import joblib
import os
import sys

def main():
    # 1) parse args
    p = argparse.ArgumentParser()
    p.add_argument("--alpha_csv",  required=True,
                   help="CSV with columns: time, z_mid, alpha_m-1")
    p.add_argument("--distance",   type=float, required=True,
                   help="Link distance in meters")
    p.add_argument("--lambda_nm",  type=float, required=True,
                   help="Wavelength in nanometers")
    p.add_argument("--out_csv",    required=True,
                   help="Path for output CSV: distance_m,rate_Mbps")
    args = p.parse_args()

    # 2) load model
    script_dir = os.path.dirname(__file__)
    model_path = os.path.join(script_dir, "../models/flux_predictor.joblib")
    rf = joblib.load(model_path)

    # 3) load & collapse alpha-depth-time CSV
    alpha_df = pd.read_csv(args.alpha_csv)
    tg = alpha_df.groupby("time")["alpha_m-1"].mean().reset_index()
    tg = tg.sort_values("time")
    alpha_series = tg["alpha_m-1"].tolist()

    # 4) truncate to the number of features your RF expects
    expected_N = rf.n_features_in_       # e.g. 52
    n_alpha    = expected_N - 2          # e.g. 50
    if len(alpha_series) < n_alpha:
        print(f"ERROR: only {len(alpha_series)} α samples but model expects {n_alpha}", file=sys.stderr)
        sys.exit(1)
    alpha_trunc = alpha_series[:n_alpha]

    # 5) assemble feature vector
    features = alpha_trunc + [args.lambda_nm, args.distance]
    X = [features]  # shape = (1, expected_N)

    # 6) predict full flux‐vs‐time curve
    y_pred = rf.predict(X)  # shape = (1, M)
    flux_curve = y_pred[0]  # a 1D array of length M

    # 7) collapse to a single scalar (e.g. average flux)
    rate_mbps = float(flux_curve.mean())

    # 8) write out CSV
    out_df = pd.DataFrame({
        "distance_m": [args.distance],
        "rate_Mbps":  [rate_mbps]
    })
    out_df.to_csv(args.out_csv, index=False)

if __name__ == "__main__":
    main()
