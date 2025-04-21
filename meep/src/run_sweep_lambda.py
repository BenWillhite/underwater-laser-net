#!/usr/bin/env python3
"""
run_sweep_lambda.py

Sweeps wavelengths at a fixed link distance,
calling run_pilot.py for each λ. Stores CSV in meep/data/.

Usage:
    python run_sweep_lambda.py --distance 2.0 --lambda_start 450 --lambda_end 550 --lambda_step 50 [--time 1.5]
"""
import argparse
import subprocess
import os

def main():
    # locate run_pilot.py next to this script
    script_dir = os.path.dirname(os.path.realpath(__file__))
    pilot_py   = os.path.join(script_dir, 'run_pilot.py')

    parser = argparse.ArgumentParser()
    parser.add_argument("--distance", type=float, default=2.0,
                        help="Link distance in meters.")
    parser.add_argument("--lambda_start", type=float, default=450,
                        help="Start wavelength in nm.")
    parser.add_argument("--lambda_end", type=float, default=550,
                        help="End wavelength in nm.")
    parser.add_argument("--lambda_step", type=float, default=50,
                        help="Wavelength step in nm.")
    parser.add_argument("--time", type=float, default=None,
                        help="Optional CFD snapshot time to pass along.")
    args = parser.parse_args()

    lambdas = list(range(int(args.lambda_start),
                         int(args.lambda_end) + 1,
                         int(args.lambda_step)))

    for wl in lambdas:
        cmd = [
            "python", pilot_py,
            "--lambda_nm", str(wl),
            "--distance",  f"{args.distance:.3f}"
        ]
        if args.time is not None:
            cmd += ["--time", str(args.time)]
        print("Running:", " ".join(cmd))
        subprocess.run(cmd, check=True)

    print("✅ Wavelength sweep completed.")

if __name__ == "__main__":
    main()
