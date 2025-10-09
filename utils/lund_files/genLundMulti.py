#!/usr/bin/env python3
import os
import numpy as np
import multiprocessing as mp
import ROOT

# --- constants (GeV, radians) ---
M_E = 0.000511
M_P = 0.938
DEG2RAD = np.pi / 180.0

def _seed_this_process():
    """Unique RNG seeds per worker so parallel jobs don't clone events."""
    pid = os.getpid()
    ROOT.gRandom.SetSeed(pid)
    np.random.seed(pid & 0xFFFFFFFF)

def generate_lund_file(file_index, output_folder, num_events_per_file):
    _seed_this_process()
    path = os.path.join(output_folder, f"proton_electron_lund_{file_index}.lund")
    lines = []

    for _ in range(num_events_per_file):
        # ---- header: 10 fields ----
        # Npart  A  Z  tgtPol  spinZ  beamPID  Ebeam  struckNucleon  processID  weight
        Npart = 2
        A, Z = 1, 1
        tgtPol = 0.0
        spinZ  = 0.0
        beamPID = 11
        Ebeam = 10.6
        struck = 2212
        processID = 0
        weight = ROOT.gRandom.Uniform(0.2, 2.0)

        lines.append(f"{Npart} {A} {Z} {tgtPol} {spinZ} {beamPID} {Ebeam} {struck} {processID} {weight}\n")

        # ---- trigger electron (random toy kinematics) ----
        p_e  = ROOT.gRandom.Uniform(2.0, 8.0)
        th_e = ROOT.gRandom.Uniform(5.0, 25.0) * DEG2RAD
        ph_e = ROOT.gRandom.Uniform(-180.0, 180.0) * DEG2RAD
        px_e = p_e * np.sin(th_e) * np.cos(ph_e)
        py_e = p_e * np.sin(th_e) * np.sin(ph_e)
        pz_e = p_e * np.cos(th_e)
        E_e  = np.sqrt(px_e*px_e + py_e*py_e + pz_e*pz_e + M_E*M_E)

        # shared vertex (cm) — vx=vy=0, vz smeared
        vz = np.random.normal(-3.0, 2.5)

        # index  lifetime(ns)  type  pid   parent firstDau  px        py        pz        E         m        vx       vy       vz
        lines.append(f"1 -1 1 11   0      0        {px_e:.6f} {py_e:.6f} {pz_e:.6f} {E_e:.6f} {M_E:.6f} 0.000000 0.000000 {vz:.6f}\n")

        # ---- proton (uncorrelated toy hadron) ----
        p_p  = ROOT.gRandom.Uniform(0.3, 5.0)
        th_p = ROOT.gRandom.Uniform(3.0, 80.0) * DEG2RAD
        ph_p = ROOT.gRandom.Uniform(-180.0, 180.0) * DEG2RAD
        px_p = p_p * np.sin(th_p) * np.cos(ph_p)
        py_p = p_p * np.sin(th_p) * np.sin(ph_p)
        pz_p = p_p * np.cos(th_p)
        E_p  = np.sqrt(px_p*px_p + py_p*py_p + pz_p*pz_p + M_P*M_P)

        lines.append(f"2 -1 1 2212 0      0        {px_p:.6f} {py_p:.6f} {pz_p:.6f} {E_p:.6f} {M_P:.6f} 0.000000 0.000000 {vz:.6f}\n")

    with open(path, "w") as f:
        f.writelines(lines)

def generate_lund_files(output_folder, num_files, num_events_per_file):
    os.makedirs(output_folder, exist_ok=True)
    tasks = [(i, output_folder, num_events_per_file) for i in range(1, num_files + 1)]
    with mp.Pool(processes=mp.cpu_count()) as pool:
        pool.starmap(generate_lund_file, tasks)
    print(f"Generated {num_files} LUND files with {num_events_per_file} events each at {output_folder}")

if __name__ == "__main__":
    outdir = "/lustre24/expphy/volatile/clas12/bulgakov/proton_electron_lund_multithread"
    num_files = 500
    num_events_per_file = 10000
    generate_lund_files(outdir, num_files, num_events_per_file)
