Underwater Laser Communication Project
An end-to-end pipeline for simulating an underwater laser link in a turbid environment, from fluid-flow (sediment) simulation in OpenFOAM → optical modeling in Meep → link-level modulation → network emulation in NS-3.

Repository Layout
foam/ OpenFOAM cases and mesh
0/ initial fields
system/ controlDict, fvSchemes, fvSolution
constant/ transportProperties, mesh files
…
foam/2_post/ post-processing scripts
compute_alpha.py turbidity → alpha_depth_time.csv
meep/
src/
run_pilot.py
run_sweep_d.py
run_sweep_lambda.py
compute_bg_photons.py
data/ Meep output CSVs and Jerlov data
notebooks/ exploratory analysis notebooks
modulation/
rate_vs_distance.py photon-budget → rate CSV
ber_curves/ vendor/literature BER lookup tables
rate_vs_distance_plot.png
ns3/ custom NS-3 module and examples
src/
underwater-laser/ helper and model submodules
scratch/
python/ flux_predictor.py and models/
examples/laser_mesh.cc
trace_parser.py post-run trace → CSV
CMakeLists.txt
env/ Conda environment.yml
docs/
architecture.md detailed architecture notes
ml/ train and predict your RF model
prepare_training_data.py
train_predictor.py
predict_flux.py
models/flux_predictor.joblib
README.md this file

Prerequisites
System dependencies
OpenFOAM (v8 or later)
Meep with Python bindings
NS-3 (revision XXX or later)
CMake 3.13 or higher, C/C++ toolchain, Python 3.8 or higher
Python environment
$ conda env create -f env/environment.yml
$ conda activate underwater-laser
$ pip install -r env/requirements.txt
or
$ python3 -m venv venv
$ source venv/bin/activate
$ pip install -r env/requirements.txt
Optional
Docker image (see docs/docker.md)
Jupyter for meep/notebooks

Quick Start

CFD → α(z,t)
$ cd foam
$ blockMesh
$ snappyHexMesh
$ pimpleTurbFoam (run to desired end time)
$ cd 2_post
$ python compute_alpha.py --case ../
produces foam/2_post/alpha_depth_time.csv

Ambient-Noise Calibration
$ cd meep/src
$ python compute_bg_photons.py --jerlov_csv ../data/jerlov/Jerlov_Type_1B_radiance.csv --params ../channel_params.json --bit_rate 20e6

Pilot & Sweeps (Meep)

pilot single distance
$ python run_pilot.py --lambda_nm 532 --distance 2.0 --time 1.5 --w0 0.001 --rx_radius 0.005 --eta_rx 0.5 --alpha_csv ../../foam/2_post/alpha_depth_time.csv

distance sweep
$ python run_sweep_d.py --lambda_nm 532 --d_start 1 --d_end 50 --d_step 1 --time 1.5

wavelength sweep
$ python run_sweep_lambda.py --distance 2.0 --lambda_start 450 --lambda_end 550 --lambda_step 25 --time 1.5
Results appear in meep/data/flux_vs_time_*.csv and PNGs

Photon-Budget & Rates
$ cd modulation
$ python rate_vs_distance.py --csv_dir ../meep/data --P_TX_mW 20 --eta_rx 0.5 --params ../meep/channel_params.json --ber_dir ber_curves
Outputs: modulation/rate_vs_distance.csv and rate_vs_distance_plot.png

Train / Retrain the ML Predictor
$ cd ml
$ python prepare_training_data.py (builds training.csv)
$ python train_predictor.py (outputs models/flux_predictor.joblib)

Build & Run NS-3 Emulation
From the ns3/ root directory:
$ rm -rf build
$ mkdir build && cd build
$ cmake .. -DENABLE_ECMP=ON -DPYTHON3_EXECUTABLE=$(which python3) -DCMAKE_BUILD_TYPE=Debug
$ cmake --build . -- -j12
$ ./ns3-dev-laser_mesh-debug --nodeSpacing=10.0 --attachErrorModel=true --txDbm=100.0 --simTime=30.0
Notes:
• Build in Debug to see NS_LOG output from UnderwaterLaser components.
• Ensure CMake picks up ns3/src/underwater-laser/.
• Always run from ns3/ root so relative paths resolve.

FAQ & Common Gotchas
“I don’t see my RF model being called!”
Verify scratch/python/flux_predictor.py is executable and models/flux_predictor.joblib is in scratch/python/../models/.
Check UnderwaterLaserHelper::FluxPredictorScript default path.

“CMake can’t find Python3 dev headers”
On Ubuntu: sudo apt-get install python3-dev
On macOS:
$ brew install python
$ xcode-select --install

“Feature-count mismatch when predicting”
Retrain using the same pipeline in prepare_training_data.py.
Confirm columns are alpha_0…alpha_N, lambda_nm, distance_m.

Plot axes look wrong
Verify Meep CSV time column matches your --time setting; confirm units (m, nm, s).

Relative paths fail
Always launch NS-3 from its root directory so “scratch/alpha_depth_time.csv” resolves.

Next Steps & Customization
Tune beam divergence via UnderwaterLaserHelper/BeamDivergence.
Swap in a new RF model by replacing flux_predictor.joblib and updating train_predictor.py.
Add directional MAC logic in underwater-laser-mac-helper.cc.
Extend to multi-wavelength by looping over alpha_<λ>.h5 in the propagation-loss model.

For detailed architecture, see docs/architecture.md. Pull requests and issues welcome.

ACKNOWLEDGEMENTS AND DISCLAIMER:

This codebase was developed by two programmers with assistance from AI. We take full responsibility for its contents, but acknowledge the assistance we had. It is provided “as-is” for at-will use, without warranty of any kind.