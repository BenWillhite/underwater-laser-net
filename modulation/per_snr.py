#!/usr/bin/env python3
import numpy as np
import pandas as pd
import os

# USER PARAMETERS
INPUT_BER_CSV = 'modulation/ber_curves/ofdm+qam.csv'    # Replace this with your actual BER CSV file
OUTPUT_PER_CSV = 'modulation/per_snr.csv'    # Output file for PER vs SNR
PACKET_LENGTH_BITS = 512          # Adjust packet size here (256, 512, 1024...)

# Load BER vs. SNR data
ber_df = pd.read_csv(INPUT_BER_CSV)
snr_db = ber_df['SNR_dB']
ber = ber_df['BER']

# Compute Packet Error Rate (PER)
per = 1 - (1 - ber) ** PACKET_LENGTH_BITS
per = np.clip(per, 0, 1)  # Ensures numerical stability

# Create DataFrame
per_df = pd.DataFrame({
    'SNR_dB': snr_db,
    'PER': per
})

# Save to CSV
per_df.to_csv(OUTPUT_PER_CSV, index=False)

print(f"✅ PER vs. SNR generated and saved to {OUTPUT_PER_CSV}")
