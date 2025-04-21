# Underwater Laser Networking: Architecture & Plan

This document outlines the end‑to‑end workflow for simulating, analyzing, and emulating underwater laser communication links, from CFD to network simulation. It reflects the latest pipeline enhancements, including beam‑divergence, ambient noise, and data‑driven link‑budget modeling.

## Sections A–F

### A. Flow & Sediment Dynamics (OpenFOAM)

1. **Simulation**: 3D fluid flow and sediment transport around the submarine geometry.  
2. **Turbidity Output**: `foam/0` and time‑stamped subfolders contain `turbidity` fields: `C(x,y,z,t)`.

### B. Post‑Processing to Optical Properties

1. **Compute Extinction & Scattering**:  
   - Script: `foam/2_post/alpha_depth_time.py` reads turbidity and cell centroids, applies extinction coefficient, and writes `foam/2_post/alpha_depth_time.csv`.  
2. **3D Fields (future)**: Export HDF5 fields per wavelength: `alpha_<λ>.h5`, `scatter_<λ>.h5` for volume‑scattering modeling.

### C. FDTD Optical Simulations (Meep)

1. **Pilot Run**: `meep/src/run_pilot.py`  
   - **Inputs**:  
     - `--lambda_nm` (nm)  
     - `--distance` (m)  
     - `--time` (s)  
     - `--w0` (launch beam radius, m)  
     - `--rx_radius` (receiver aperture radius, m)  
     - `--eta_rx` (detector efficiency)  
     - `--alpha_csv` (path to `alpha_depth_time.csv`)  
   - **What it does**:  
     - Loads ⟨α⟩ from CFD data, embeds underwater absorption as conductivity  
     - Computes diffraction‑limited divergence, geometric coupling, η_rx  
     - Records raw Meep flux vs time, scales to _effective_ flux  
   - **Outputs** in `meep/data/`:  
     - `flux_vs_time_λ{λ}_d{d}_t{t}.csv` (time, effective flux)  
     - Plot: `flux_vs_time_λ{λ}_d{d}_t{t}.png`  
     - Ez field dumps → PNGs → GIF `Ez_evolution_λ{λ}.gif`  
2. **Distance Sweep**: `meep/src/run_sweep_d.py`  
3. **Wavelength Sweep**: `meep/src/run_sweep_lambda.py`  
4. **Ambient Noise Calibration**: `meep/src/compute_bg_photons.py`  
   - **Inputs**:  
     - `--jerlov_csv meep/data/jerlov/Jerlov_Type_1B_radiance.csv` (downwelling radiance)  
     - `--params meep/channel_params.json`  
     - `--bit_rate` (bits/s)  
   - **What it does**: integrates ambient radiance over 2π sr & A_rx to compute `N_bg` (photons/bit)  
   - **Output**: updates `meep/channel_params.json` with `"N_bg"`  
5. **Channel Parameters**: `meep/channel_params.json`  
   {
     "A_rx": 3e-5,
     "I_dark": 1e-9,
     "NEP": 1e-12,
     "B": 1e7,
     "RIN": 1e-13,
     "FEC_rate": 0.8,
     "N_bg": <computed>
   }
D. Link‑Level Modulation Analysis (Photon‑Budget)
Rate Calculator: modulation/rate_vs_distance.py

Inputs:

--csv_dir meep/data

--P_TX_mW (mW)

--eta_rx

--params meep/channel_params.json

--ber_dir modulation/ber_curves (CSV tables: SNR_dB,BER)

What it does:

Loads Meep “unit‑flux” CSVs, scales by P_TX → P_RX

Computes photons/bit, shot‑ & ambient‑noise SNR (includes N_dark, NEP, RIN, N_bg)

Interpolates real BER vs SNR lookup tables

Applies FEC overhead

Outputs:

modulation/rate_vs_distance.csv (detailed link budget + rates)

modulation/rate_vs_distance_plot.png

BER Curves: drop vendor/literature CSVs into modulation/ber_curves/ (e.g. ook.csv, 4pam.csv, ppm.csv, 16qam.csv).

E. Network‑Level Emulation (NS‑3)
Trace Parser: ns3/trace_parser.py converts rate_vs_distance.csv → NS‑3 PER tables.

Error Model: ns3/scratch/underwater_laser.cc implements dynamic PER based on instantaneous P_RX(d,t).

Mesh Example: ns3/examples/laser_mesh.cc sets up multi‑node scenarios with OLSR/AODV.

F. Integration & Automation
Environment: Conda / env/environment.yml or Docker for OpenFOAM, Meep, Python, NS‑3.

End‑to‑End Pipeline: use a Makefile or Snakemake to chain:

# 1) CFD → turbidity → α(z,t)
python foam/2_post/compute_alpha.py --case foam

# 2) Ambient noise
python meep/src/compute_bg_photons.py \
  --jerlov_csv meep/data/jerlov/Jerlov_Type_1B_radiance.csv \
  --params meep/channel_params.json \
  --bit_rate 20e6

# 3) Sanity‑check pilot
python meep/src/run_pilot.py \
  --lambda_nm 532 --distance 2.0 --time 1.5 \
  --w0 0.001 --rx_radius 0.005 --eta_rx 0.5

# 4) Sweeps
python meep/src/run_sweep_d.py \
  --lambda_nm 532 --d_start 1 --d_end 50 --d_step 1 --time 1.5
python meep/src/run_sweep_lambda.py \
  --distance 2 --lambda_start 450 --lambda_end 550 --lambda_step 25 --time 1.5

# 5) Link‑level rates
cd modulation
python rate_vs_distance.py \
  --csv_dir ../meep/data \
  --P_TX_mW 10 \
  --eta_rx 0.5 \
  --params ../meep/channel_params.json \
  --ber_dir ber_curves

# 6) NS‑3 emulation
cd ../ns3
./waf --run scratch/underwater_laser
All components now reflect a data‑driven, physics‑based link‑budget methodology, ready to plug into NS‑3 or lab validation.