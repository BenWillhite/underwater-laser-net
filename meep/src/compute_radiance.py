#!/usr/bin/env python3
import os, numpy as np, pandas as pd, matplotlib.pyplot as plt, pvlib

# 1) Load ASTM G173‑03 global spectrum
am15   = pvlib.spectrum.get_reference_spectra(standard="ASTM G173-03")
E0_nm  = am15.index.values       # wavelengths [nm]
E0     = am15["global"].values   # W/m²/nm

# locate this script’s directory
script_dir = os.path.dirname(__file__)

# 2) Load your Jerlov Kd (0–10 m) data as floats, using the file’s header
jd = os.path.abspath(os.path.join(
     script_dir, "..", "data", "jerlov",
     "Jerlov_Type_1B_Kd_0-10m.csv"))
dfkd = pd.read_csv(jd)                  # reads the first line as header

# rename whatever the original columns were to our canonical names:
dfkd = dfkd.rename(columns={
    dfkd.columns[0]: "wavelength_nm",
    dfkd.columns[1]: "Kd_m-1"
})

# now ensure they’re truly floats
dfkd["wavelength_nm"] = dfkd["wavelength_nm"].astype(float)
dfkd["Kd_m-1"]        = dfkd["Kd_m-1"].astype(float)

# 3) Pick a depth, compute transmittance
z = 1.0  # 1 m below surface
dfkd["T"] = np.exp(-dfkd["Kd_m-1"] * z)

# 4) Interpolate E0 onto those wavelengths
E0_interp = np.interp(dfkd["wavelength_nm"], E0_nm, E0)

# 5) Compute down‑welling radiance (approximate as E/π → radiance)
dfkd["Lamb_W_m2_nm_sr"] = E0_interp * dfkd["T"] / np.pi

# 6) Save a “radiance” CSV for compute_bg_photons.py
out = os.path.abspath(os.path.join(
      script_dir, "..", "data", "jerlov",
      "Jerlov_Type_1B_radiance.csv"))
dfkd[["wavelength_nm","Lamb_W_m2_nm_sr"]].to_csv(out, index=False)

print(f"→ wrote radiance CSV to {out}")
