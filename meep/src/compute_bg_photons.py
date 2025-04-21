#!/usr/bin/env python3
"""
compute_bg_photons.py

Integrate ambient radiance over receiver FOV, aperture & bit‐period
to estimate background photons per bit.

**YOU MUST** supply a CSV of real spectral radiance!
If you only have Jerlov transmittance, you need to multiply a known
solar‐irradiance spectrum by transmittance(λ) to get radiance[λ].

Usage:
    python compute_bg_photons.py \
      --jerlov_csv ../meep/data/jerlov/Jerlov_Type_1B.csv \
      --params    ../meep/channel_params.json \
      --bit_rate  20e6
"""

import os
import argparse
import json
import numpy as np
import pandas as pd

def main():
    p = argparse.ArgumentParser()
    p.add_argument("--jerlov_csv", type=str,
                   default=os.path.join(
                     os.path.dirname(__file__),
                     "../meep/data/jerlov/Jerlov_Type_1B.csv"
                   ),
                   help="CSV of downwelling RADIANCE [nm,w/m2/nm/sr].")
    p.add_argument("--params",    type=str,
                   default=os.path.join(
                     os.path.dirname(__file__),
                     "../meep/channel_params.json"
                   ),
                   help="JSON with receiver parameters (A_rx, etc.)")
    p.add_argument("--bit_rate",  type=float, default=20e6,
                   help="Bit rate [bits/s]")
    args = p.parse_args()

    # load channel parameters
    with open(args.params) as f:
        cp = json.load(f)
    A_rx = cp["A_rx"]

    # load your Jerlov CSV
    df = pd.read_csv(args.jerlov_csv)

    # your file currently has 'wavelength_nm' & 'transmission_pct'
    # but compute_bg_photons *expects* radiance Lamb=[W/m2/nm/sr].
    # 👉 If you *only* have transmission data, you must *also*
    #    load a solar‐irradiance spectrum, multiply it by
    #    transmission_pct/100, to get Lamb(λ).  

    # For now, we’ll assume your CSV was actually radiance:
    if "Lamb_W_m2_nm_sr" in df.columns:
        lamb_nm = df["wavelength_nm"].values
        Lamb    = df["Lamb_W_m2_nm_sr"].values
    else:
        raise RuntimeError(
            f"{args.jerlov_csv} does not contain a 'Lamb_W_m2_nm_sr' column;\n"
            "you need a true radiance spectrum, not transmittance!\n"
            "Either provide a different CSV or preprocess a solar spectrum × T(λ)."
        )

    # integrate Lamb(λ) over Δλ, full downward hemisphere (2π sr), and your aperture
    Ω   = 2*np.pi
    λ_m = lamb_nm * 1e-9
    Δλ  = np.gradient(λ_m)               # in meters
    P_bg = np.sum(Lamb * A_rx * Ω * Δλ)  # total background optical power [W]

    # convert to photons/sec at mean wavelength
    h    = 6.626e-34
    c    = 3e8
    λ_avg= λ_m.mean()
    E_ph = h*c/λ_avg
    N_bg_s = P_bg / E_ph

    # per‐bit
    N_bg = N_bg_s / args.bit_rate
    print(f"Background photons/bit = {N_bg:.3e}")

    # store back into your channel params for downstream scripts
    cp["N_bg"] = N_bg
    with open(args.params, "w") as f:
        json.dump(cp, f, indent=2)
    print(f"→ updated {args.params} with N_bg")

if __name__=="__main__":
    main()
