#!/usr/bin/env python3
"""
run_sweep_d.py

Sweeps link distances (d) at a fixed wavelength,
calling run_pilot.py for each distance. Stores CSV in meep/data/.

Usage:
    python run_sweep_d.py --lambda_nm 532 --d_start 1.0 --d_end 5.0 --d_step 1.0 [--time 1.5]
"""
import argparse
import subprocess
import numpy as np
import os

def main():
    # find the run_pilot.py alongside this script
    script_dir = os.path.dirname(os.path.realpath(__file__))
    pilot_py   = os.path.join(script_dir, 'run_pilot.py')

    parser = argparse.ArgumentParser()
    parser.add_argument("--lambda_nm", type=float, default=532.0,
                        help="Laser wavelength in nm.")
    parser.add_argument("--d_start", type=float, default=1.0,
                        help="Starting link distance (m).")
    parser.add_argument("--d_end", type=float, default=5.0,
                        help="Ending link distance (m).")
    parser.add_argument("--d_step", type=float, default=1.0,
                        help="Step size (m).")
    parser.add_argument("--time", type=float, default=None,
                        help="Optional CFD snapshot time to pass along.")
    args = parser.parse_args()

    distances = np.arange(args.d_start, args.d_end + args.d_step/2, args.d_step)
    for dist in distances:
        cmd = [
            "python", pilot_py,
            "--lambda_nm", str(args.lambda_nm),
            "--distance",  f"{dist:.3f}"
        ]
        if args.time is not None:
            cmd += ["--time", str(args.time)]
        print("Running:", " ".join(cmd))
        subprocess.run(cmd, check=True)

    print("Distance sweep completed.")

if __name__ == "__main__":
    main()
