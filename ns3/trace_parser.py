#!/usr/bin/env python3
"""
trace_parser.py

Reads the flux & rate data from the Meep + modulation stage,
produces a time-based or distance-based CSV for NS-3.

Usage:
    python trace_parser.py --meep_data ../meep/data --output channel_trace.csv
"""

import argparse
import pandas as pd
import glob
import re
import numpy as np

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--meep_data", type=str, default="../meep/data",
                        help="Directory containing Meep output CSV files.")
    parser.add_argument("--output", type=str, default="channel_trace.csv",
                        help="Output CSV for NS-3.")
    args = parser.parse_args()

    # Example: gather (distance, rate_Mbps) from the modulation results
    # We assume we have a single file with aggregated rates, or we'd parse individually.
    # Here is a placeholder approach:
    data = {
        "time_s": [],
        "node_i": [],
        "node_j": [],
        "errorRate": []
    }

    # Suppose we set a simple model: errorRate ~ 1 / (rate_Mbps + 1)
    # We'll just produce some dummy lines:
    for t in np.linspace(0, 10, 11):
        for i in range(3):
            for j in range(3, 6):
                error_rate = 0.01 * (j - i)
                data["time_s"].append(t)
                data["node_i"].append(i)
                data["node_j"].append(j)
                data["errorRate"].append(error_rate)

    df = pd.DataFrame(data)
    df.to_csv(args.output, index=False)
    print(f"Saved trace to {args.output}")

if __name__ == "__main__":
    main()
