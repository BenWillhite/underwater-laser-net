# Underwater Laser Communication Project

This repository demonstrates an end-to-end pipeline for simulating
an underwater laser link in a turbid environment, from fluid flow
(sediment) simulation in OpenFOAM to optical modeling in Meep,
then link-level modulation and final network emulation in NS-3.

## Directory Layout
foam/ 
2_post/ 
meep/ 
    src/ 
    data/ 
    notebooks/ 
modulation/
ns3/ 
    scratch/ 
    examples/ 
    trace_parser.py 
env/ 
docs/ README.md

See `docs/architecture.md` for detailed notes on each stage.

## Quick Start
1. **OpenFOAM**: Run your mesh generation (`blockMesh`, `snappyHexMesh`), then `pimpleTurbFoam` with the provided cases to generate `turbidity`.
2. **Post-Process**: `cd foam/2_post && python compute_alpha.py --case ../`.
3. **Meep**: `cd meep/src && python run_sweep_λ.py --distance 2.0` or `python run_sweep_d.py --lambda_nm 532`.
4. **Analysis**: Open `meep/notebooks/analysis.ipynb` in Jupyter to visualize flux and extract attenuation.
5. **Modulation**: `cd modulation && python rate_vs_distance.py`.
6. **Trace Parser**: `cd ns3 && python trace_parser.py` -> produces `channel_trace.csv`.
7. **NS-3**: Build ns-3 with your custom `underwater_laser.cc`, then run `laser_mesh.cc`.

## Environment
- We provide `env/environment.yml` as a starting point.  
- You may need system-level installs (OpenFOAM, NS-3) or Docker for a fully reproducible setup.