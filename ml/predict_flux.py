import argparse
import pandas as pd
import numpy as np
import joblib

def main():
    p = argparse.ArgumentParser(description="Predict future flux curve from ML model")
    p.add_argument("--alpha_csv",   required=True,
                   help="path to alpha_depth_time.csv (extinction vs time & depth)")
    p.add_argument("--distance",    required=True, type=float,
                   help="link distance in meters")
    p.add_argument("--lambda_nm",   required=True, type=int,
                   help="laser wavelength in nanometers")
    p.add_argument("--out_csv",     required=True,
                   help="output CSV path for predicted flux curve")
    p.add_argument("--time_horizon", type=float, default=1.5,
                   help="total time span (s) for predicted flux curve (default: 1.5s)")
    args = p.parse_args()

    # Load the trained model
    model = joblib.load("models/flux_predictor.joblib")

    # Determine feature window size T from model input dims (minus distance & lambda)
    n_features = model.n_features_in_
    T = n_features - 2

    # Load and preprocess alpha
    df_alpha = pd.read_csv(args.alpha_csv)
    # average over depth for each time, then sort by time
    alpha_time = df_alpha.groupby("time")["alpha_m-1"].mean().reset_index()
    alpha_series = alpha_time.sort_values("time")["alpha_m-1"].values

    if len(alpha_series) < T:
        raise RuntimeError(f"Need at least {T} alpha samples, but got {len(alpha_series)}")

    # Grab last T samples as features
    window = alpha_series[-T:]

    # Build feature vector [alpha_0,...,alpha_{T-1}, distance, lambda]
    X = np.hstack([window, args.distance, args.lambda_nm]).reshape(1, -1)

    # Predict future flux curve (shape: 1 x M)
    y_pred = model.predict(X)[0]
    M = y_pred.shape[0]

    # Build time axis
    times = np.linspace(0, args.time_horizon, M)

    # Write prediction to CSV
    df_out = pd.DataFrame({
        "time":           times,
        "predicted_flux": y_pred
    })
    df_out.to_csv(args.out_csv, index=False)
    print(f"Saved predicted flux curve ({M} samples) to {args.out_csv}")

if __name__ == "__main__":
    main()
