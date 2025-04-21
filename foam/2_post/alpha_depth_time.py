#!/usr/bin/env python3
import os, argparse
import numpy as np
import csv
import re

def find_all_times(case_dir):
    return sorted(
        [d for d in os.listdir(case_dir)
         if os.path.isdir(os.path.join(case_dir, d))
            and all(c.isdigit() or c == '.' for c in d)],
        key=float
    )

def read_turbidity(case_dir, time_dir):
    fpath = os.path.join(case_dir, time_dir, 'turbidity')
    with open(fpath) as f:
        lines = [l.strip() for l in f if l.strip()]
    for i, l in enumerate(lines):
        if l.startswith('internalField') and 'nonuniform' in l:
            n = int(lines[i+1])
            j = i+2 + (lines[i+2] == '(')
            return np.array([float(lines[j+k]) for k in range(n)])
        if l.startswith('internalField') and 'uniform' in l:
            return np.full(1, float(l.split()[-1].rstrip(';')))
    raise RuntimeError("Couldn't parse turbidity field")

def read_cell_centers_z(case_dir):
    pm = os.path.join(case_dir, 'constant', 'polyMesh')

    def _read_block(fname):
        """
        Read a file in constant/polyMesh, strip out C‐style (/*…*/) and // comments,
        then find the first pure‐digit line as 'count', skip a following '(',
        and return (count, the next count lines).
        """
        raw = []
        inside_cblock = False
        path = os.path.join(pm, fname)
        with open(path) as f:
            for line in f:
                s = line.strip()
                if not s:
                    continue

                # start of C‐comment
                if s.startswith('/*'):
                    inside_cblock = True
                    continue

                # end of C‐comment
                if inside_cblock:
                    if s.endswith('*/'):
                        inside_cblock = False
                    continue

                # skip single‐line C++ comment
                if s.startswith('//'):
                    continue

                raw.append(s)

        # now raw[] contains only "real" lines
        for idx, s in enumerate(raw):
            if re.fullmatch(r'\d+', s):
                count = int(s)
                start = idx + 1
                if start < len(raw) and raw[start] == '(':
                    start += 1
                block = raw[start:start+count]
                if len(block) != count:
                    raise RuntimeError(
                        f"{fname}: expected {count} entries, got {len(block)}"
                    )
                return count, block

        raise RuntimeError(f"{fname}: could not find a digit‐only header line")

    # 1) points
    np_pts, pt_lines = _read_block('points')
    points = np.array([
        list(map(float, re.findall(r'[-\d\.eE]+', ln)))
        for ln in pt_lines
    ])

    # 2) faces
    n_faces, face_lines = _read_block('faces')
    faces = []
    for ln in face_lines:
        inside = re.search(r'\((.*)\)', ln).group(1)
        faces.append(list(map(int, inside.split())))
    faces = np.array(faces, dtype=object)

    # 3) owner
    _, owner_lines = _read_block('owner')
    owner = np.array(list(map(int, owner_lines)))

    # 4) neighbour
    n_internal, neigh_lines = _read_block('neighbour')
    neigh = np.array(list(map(int, neigh_lines)))

    # 5) build cell→point sets
    Nc = max(owner.max(), neigh.max()) + 1
    cell_pts = [set() for _ in range(Nc)]
    for fi in range(n_faces):
        pts = faces[fi]
        c0 = owner[fi]
        cell_pts[c0].update(pts)
        if fi < n_internal:
            c1 = neigh[fi]
            cell_pts[c1].update(pts)

    # 6) compute z‐centroids
    zcc = np.empty(Nc, dtype=float)
    for ci in range(Nc):
        idxs = list(cell_pts[ci])
        zcc[ci] = points[idxs, 2].mean()

    return zcc

def main():
    p = argparse.ArgumentParser()
    p.add_argument('--case', required=True,
                   help='Path to FOAM case (has constant/, time dirs)')
    p.add_argument('--extinction-coeff', type=float, default=1.0,
                   help='Conversion factor k_ext [m^2/kg]')
    p.add_argument('--zbins',    type=int,   default=20,
                   help='Number of depth bins')
    args = p.parse_args()

    case = os.path.expanduser(args.case).rstrip('/')
    times = find_all_times(case)
    zcc   = read_cell_centers_z(case)
    zmin, zmax = zcc.min(), zcc.max()
    bins = np.linspace(zmin, zmax, args.zbins+1)

    out_file = os.path.join(case, '2_post', 'alpha_depth_time.csv')
    os.makedirs(os.path.dirname(out_file), exist_ok=True)

    with open(out_file, 'w', newline='') as csvf:
        writer = csv.writer(csvf)
        writer.writerow(['time','z_mid','alpha_m-1'])
        for t in times:
            turb = read_turbidity(case, t)
            if turb.size == 1:
                turb = np.full_like(zcc, turb[0])
            inds = np.digitize(zcc, bins)
            for b in range(1, args.zbins+1):
                mask = (inds == b)
                if not mask.any():
                    continue
                Cmean = turb[mask].mean()
                alpha = args.extinction_coeff * Cmean
                z_mid = 0.5*(bins[b-1] + bins[b])
                writer.writerow([t, f"{z_mid:.6f}", f"{alpha:.6e}"])

    print(f"Depth–profile α by time written to {out_file}")

if __name__ == '__main__':
    main()
