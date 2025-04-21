#!/usr/bin/env python3
"""
run_pilot.py

A baseline Meep FDTD simulation at a single wavelength (lambda_nm),
a single link distance (distance_m), and with underwater absorption
folded in via your CFD-derived α(z,t), plus realistic geometric
coupling and detector efficiency.

This version:
  - Logs the full flux vs. time history (effective power at detector)
  - Saves CSV + matplotlib plot in meep/data/
  - Dumps Ez-*.h5 via Meep into meep/data/, converts to PNGs
  - Builds a GIF animation of the field evolution

Usage:
    python run_pilot.py --lambda_nm 532 --distance 2.0 --time 0.5 \
        --w0 0.001 --rx_radius 0.005 --eta_rx 0.5
"""

import argparse
import os
import glob
import h5py
import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
import imageio
import meep as mp


def load_alpha_profile(alpha_csv, t):
    df = pd.read_csv(alpha_csv)
    times = np.unique(df['time'].astype(float).values)
    t_pick = min(times, key=lambda tt: abs(tt - t))
    sub = df[df['time'].astype(float) == t_pick]
    z = sub['z_mid'].astype(float).values
    alpha_vals = np.abs(sub['alpha_m-1'].astype(float).values)
    integral = np.trapz(alpha_vals, x=z)
    depth = z.max() - z.min()
    return integral / depth


def main():
    # determine project layout
    script_dir = os.path.dirname(os.path.realpath(__file__))
    repo_root  = os.path.abspath(os.path.join(script_dir, '..', '..'))
    default_csv = os.path.join(repo_root, 'foam', '2_post', 'alpha_depth_time.csv')
    data_dir    = os.path.join(repo_root, 'meep', 'data')
    os.makedirs(data_dir, exist_ok=True)

    # parse args
    parser = argparse.ArgumentParser()
    parser.add_argument("--lambda_nm",  type=float, default=532.0,
                        help="Wavelength in nm")
    parser.add_argument("--distance",   type=float, default=2.0,
                        help="Link distance in meters")
    parser.add_argument("--time",       type=float, default=None,
                        help="CFD snapshot time (s)")
    parser.add_argument("--alpha_csv",  type=str, default=default_csv,
                        help="Path to alpha_depth_time.csv")
    parser.add_argument("--w0",         type=float, default=0.001,
                        help="Launch beam radius w₀ [m]")
    parser.add_argument("--rx_radius",  type=float, default=0.005,
                        help="Receiver aperture radius [m]")
    parser.add_argument("--eta_rx",     type=float, default=0.5,
                        help="Detector quantum efficiency η_rx")
    args = parser.parse_args()

    # pick snapshot time if none given
    if args.time is None:
        df_all = pd.read_csv(args.alpha_csv)
        args.time = df_all['time'].astype(float).max()
        print(f"No --time given; defaulting to t = {args.time:.3f} s")

    # load path‑average extinction
    bar_alpha = load_alpha_profile(args.alpha_csv, args.time)
    print(f"Using ⟨α⟩ = {bar_alpha:.4e} m⁻¹ at t = {args.time:.3f} s")

    # Meep setup
    wavelength_um = args.lambda_nm / 1000.0
    dpml          = 1.0
    cell_size     = mp.Vector3(args.distance + 2*dpml, 0, 0)
    water         = mp.Medium(index=1.33, D_conductivity=bar_alpha)

    # geometric coupling & detector efficiency
    w0    = args.w0
    theta = (wavelength_um * 1e-6) / (np.pi * w0)    # λ [μm]→m / (π w0)
    w_d   = w0 + args.distance * np.tan(theta)
    A_beam = np.pi * w_d**2
    A_rx   = np.pi * args.rx_radius**2
    coupling = A_rx / A_beam
    print(f"Geometric coupling A_rx/A_beam = {coupling:.2e}, η_rx = {args.eta_rx:.2f}")

    # flux recorder (will store effective power)
    times = []
    flux_hist = []
    def record_flux(sim):
        pwr_raw = mp.get_fluxes(flux_obj)[0]
        pwr_eff = pwr_raw * coupling * args.eta_rx
        times.append(sim.meep_time())
        flux_hist.append(pwr_eff)

    # build simulation (dump Ez-*.h5 into data_dir)
    src = [ mp.Source(mp.ContinuousSource(wavelength=wavelength_um),
                      component=mp.Ez,
                      center=mp.Vector3(-0.5*args.distance + 0.2, 0, 0),
                      size=mp.Vector3(0,0,0)) ]

    sim = mp.Simulation(cell_size=cell_size,
                        boundary_layers=[mp.PML(dpml)],
                        geometry=[],
                        sources=src,
                        resolution=20,
                        default_material=water,
                        filename_prefix="flux_")
    sim.use_output_directory(data_dir)

    flux_region = mp.FluxRegion(center=mp.Vector3(0.5*args.distance - 0.2, 0, 0))
    global flux_obj
    flux_obj = sim.add_flux(1.0/wavelength_um, 0, 1, flux_region)

    # run and record
    sim.run(
        mp.at_every(1, record_flux),
        mp.at_every(1, mp.output_efield_z),
        until=50
    )

    # save effective flux history CSV & plot
    csv_name = f"flux_vs_time_λ{int(args.lambda_nm)}_d{args.distance:.2f}_t{args.time:.2f}.csv"
    csv_path = os.path.join(data_dir, csv_name)
    pd.DataFrame({'time': times, 'flux': flux_hist}).to_csv(csv_path, index=False)

    plt.figure()
    plt.plot(times, flux_hist)
    plt.xlabel('meep time')
    plt.ylabel('effective flux (W)')
    plt.title(f'Flux vs Time (λ={args.lambda_nm}nm, d={args.distance}m)')
    plt.grid(True)
    plot_name = csv_name.replace('.csv', '.png')
    plt.savefig(os.path.join(data_dir, plot_name))
    plt.close()
    print(f"Flux history saved to {csv_path} and plot {plot_name}")

    # convert Ez-*.h5 → PNG sequence
    h5_files = sorted(glob.glob(os.path.join(data_dir, 'flux_efield_z_*.h5')))
    png_files = []
    for h5 in h5_files:
        step = os.path.basename(h5).split('_')[-1].split('.')[0]
        with h5py.File(h5, 'r') as f:
            arr = np.array(f['Ez'])
        plt.figure()
        plt.plot(arr)
        plt.title(f'Ez at step {step}')
        plt.xlabel('grid index')
        plt.ylabel('Ez')
        png = os.path.join(data_dir, f'Ez_{step}.png')
        plt.savefig(png)
        plt.close()
        png_files.append(png)

    # build GIF if we have frames
    if png_files:
        gif_path = os.path.join(data_dir, f'Ez_evolution_λ{int(args.lambda_nm)}.gif')
        images = [imageio.imread(p) for p in png_files]
        imageio.mimsave(gif_path, images, fps=5)
        print(f"Field evolution GIF saved to {gif_path}")
    else:
        print("No Ez-*.h5 frames found; skipping GIF.")


if __name__ == "__main__":
    main()
