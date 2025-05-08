Underwater Laser Networking: Architecture & Plan

This document outlines the end-to-end workflow for simulating, analyzing, and emulating underwater laser communication links, from CFD to network simulation. It reflects the latest pipeline enhancements, including beam-divergence, ambient noise, and data-driven link-budget modeling.

Sections A–F

A. Flow & Sediment Dynamics (OpenFOAM)

Simulation: 3D fluid flow and sediment transport around the submarine geometry.

Turbidity Output: foam/0 and time-stamped subfolders contain turbidity fields: C(x,y,z,t).

B. Post-Processing to Optical Properties

Compute Extinction & Scattering:
• Script: foam/2_post/alpha_depth_time.py reads turbidity and cell centroids, applies extinction coefficient, writes foam/2_post/alpha_depth_time.csv.

3D Fields (future): Export HDF5 fields per wavelength—alpha_<λ>.h5 and scatter_<λ>.h5—for volume-scattering modeling.

C. FDTD Optical Simulations (Meep)

Pilot Run: meep/src/run_pilot.py
Inputs: --lambda_nm (nm), --distance (m), --time (s), --w0 (launch beam radius), --rx_radius (receiver aperture), --eta_rx, --alpha_csv
Outputs: meep/data/flux_vs_time_λ{λ}_d{d}_t{t}.csv, corresponding PNG plot, Ez field GIF.

Distance Sweep: meep/src/run_sweep_d.py

Wavelength Sweep: meep/src/run_sweep_lambda.py

Ambient Noise Calibration: meep/src/compute_bg_photons.py
Inputs: --jerlov_csv meep/data/jerlov/Jerlov_Type_1B_radiance.csv, --params meep/channel_params.json, --bit_rate
Output: updates N_bg in meep/channel_params.json

Channel Params file:
{
"A_rx": 3e-5,
"I_dark": 1e-9,
"NEP": 1e-12,
"B": 1e7,
"RIN": 1e-13,
"FEC_rate": 0.8,
"N_bg": <computed>
}

D. Link-Level Modulation Analysis (Photon-Budget)
• Script: modulation/rate_vs_distance.py
• Inputs: --csv_dir meep/data, --P_TX_mW, --eta_rx, --params meep/channel_params.json, --ber_dir modulation/ber_curves
• Outputs: modulation/rate_vs_distance.csv and rate_vs_distance_plot.png
• BER curves: place CSVs (ook.csv, 4pam.csv, etc.) in modulation/ber_curves/

E. Network-Level Emulation (NS-3)
All code lives under ns3/src/underwater-laser/
Directory structure:
helper/
underwater-laser-channel-helper.cc/.h
underwater-laser-helper.cc/.h
underwater-laser-mac-helper.cc/.h
underwater-laser-phy-helper.cc/.h
model/
underwater-laser-channel.cc/.h
underwater-laser-propagation-loss-model.cc/.h
underwater-laser-error-rate-model.cc/.h
underwater-laser-rate-table.cc/.h
underwater-laser-net-device.cc/.h

Helper layer
UnderwaterLaserChannelHelper: configures channel + loss model
UnderwaterLaserHelper: top-level installer that builds the shared RateTable, instantiates channel, and for each node creates and wires an UnderwaterLaserNetDevice
UnderwaterLaserPhyHelper: attaches error-rate model
UnderwaterLaserMacHelper: stub for CSMA/CA or directional MAC

Model layer
UnderwaterLaserChannel: on TransmitStart, computes Rx power via PropagationLossModel and delivers packets
UnderwaterLaserPropagationLossModel: loads alpha_depth_time.csv, applies Beer–Lambert + divergence
UnderwaterLaserErrorRateModel: interpolates SNR→PER/BER tables
UnderwaterLaserRateTable: interpolates distance→rate CSV
UnderwaterLaserNetDevice: implements NetDevice API, shapes packets by rate, applies errors, schedules transmissions

F. Integration & Automation
Environment: Conda (env/environment.yml) or Docker
End-to-End pipeline via Makefile or Snakemake:
1. python foam/2_post/compute_alpha.py --case foam
2. python meep/src/compute_bg_photons.py --jerlov_csv … --params … --bit_rate 20e6
3. python meep/src/run_pilot.py --lambda_nm 532 --distance 2.0 --time 1.5 --w0 0.001 --rx_radius 0.005 --eta_rx 0.5
4. python meep/src/run_sweep_d.py --lambda_nm 532 --d_start 1 --d_end 50 --d_step 1 --time 1.5
python meep/src/run_sweep_lambda.py --distance 2 --lambda_start 450 --lambda_end 550 --lambda_step 25 --time 1.5
5. cd modulation && python rate_vs_distance.py --csv_dir ../meep/data --P_TX_mW 20 --eta_rx 0.5 --params ../meep/channel_params.json --ber_dir ber_curves
6. NS-3 build & run (must run from ns-3 root so all relative paths resolve):
rm -rf build
mkdir build && cd build
cmake .. -DENABLE_ECMP=ON -DPYTHON_BINDINGS=ON -DCMAKE_BUILD_TYPE=Debug
cmake --build . -- -j12
./build/ns3-dev-laser_mesh-debug --nodeSpacing=10.0 --attachErrorModel=true --txDbm=100.0 --simTime=30.0

Notes:
• Always build in Debug to see full NS_LOG output.
• The custom underwater-laser helper, channel, models and rate-table only compile when the module’s CMakeLists.txt is enabled (under ns3/src/underwater-laser).

ML folder layout (underwater-laser-net/ml):
models/
flux_predictor.joblib
prepare_training_data.py
training.csv
train_predictor.py
predict_flux.py

Keep all of these in place so you can re-train or tweak your RandomForest predictor without touching the C++ side.