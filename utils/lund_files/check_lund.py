#!/usr/bin/env python3
import ROOT
import os
import sys
import glob
import math

# ----------------- Constants -----------------
PROTON_MASS = 0.938   # GeV
BEAM_ENERGY = 10.6    # GeV (massless e- along +z)

# ----------------- Globals -------------------
event_count = 0

# ----------------- Histograms ----------------
h2_q2_xbj = ROOT.TH2D("h2_q2_xbj",
                      "Q^{2} vs x_{Bj};x_{Bj};Q^{2} [GeV^{2}]",
                      100, 0.0, 3.0, 100, 0.0, 12.0)

h1_q2 = ROOT.TH1D("h1_q2",
                  "Q^{2} Distribution;Q^{2} [GeV^{2}];Counts",
                  100, 0.0, 15.0)

h1_xbj = ROOT.TH1D("h1_xbj",
                   "x_{Bj} Distribution;x_{Bj};Counts",
                   100, 0.0, 3.0)

# Note: your proton generator goes up to 5 GeV, so set axis to 5.
h2_q2_pp = ROOT.TH2D("h2_q2_pp",
                     "Q^{2} vs Proton Momentum Magnitude;|p_{proton}| [GeV];Q^{2} [GeV^{2}]",
                     100, 0.0, 10.0, 100, 0.0, 12.0)

# --- New histograms ---
# Electron
h2_e_theta_p = ROOT.TH2D("h2_e_theta_p", "Electron #theta vs |p|;|p_{e}| [GeV];#theta_{e} [deg]", 100, 0, 10, 100, 0, 180)
h2_e_phi_p   = ROOT.TH2D("h2_e_phi_p", "Electron #phi vs |p|;|p_{e}| [GeV];#phi_{e} [deg]", 100, 0, 10, 100, -180, 180)
h2_e_theta_phi = ROOT.TH2D("h2_e_theta_phi", "Electron #theta vs #phi;#phi_{e} [deg];#theta_{e} [deg]", 100, -180, 180, 100, 0, 180)

# Proton
h2_p_theta_p = ROOT.TH2D("h2_p_theta_p", "Proton #theta vs |p|;|p_{p}| [GeV];#theta_{p} [deg]", 100, 0, 10, 100, 0, 180)
h2_p_phi_p   = ROOT.TH2D("h2_p_phi_p", "Proton #phi vs |p|;|p_{p}| [GeV];#phi_{p} [deg]", 100, 0, 10, 100, -180, 180)
h2_p_theta_phi = ROOT.TH2D("h2_p_theta_phi", "Proton #theta vs #phi;#phi_{p} [deg];#theta_{p} [deg]", 100, -180, 180, 100, 0, 180)


# ---------------- Beam 4-vector ---------------
beam_vec = ROOT.TLorentzVector(0.0, 0.0, BEAM_ENERGY, BEAM_ENERGY)


def _parse_int(token, default=None):
    try:
        return int(token)
    except Exception:
        return default


def _parse_float(token, default=None):
    try:
        return float(token)
    except Exception:
        return default


def process_file(filepath):
    """
    Read a single LUND file and fill histograms.
    Assumes event header line followed by exactly N particle lines.
    Blank lines are ignored.
    """
    global event_count

    with open(filepath, "r") as f:
        # Strip whitespace and drop blanks so header & particles are contiguous
        lines = [line.strip() for line in f if line.strip()]

    i = 0
    n_lines = len(lines)
    while i < n_lines:
        # --- Header ---
        header_tokens = lines[i].split()
        n_particles = _parse_int(header_tokens[0], default=None)
        if n_particles is None:
            # Malformed header; try to skip this line
            i += 1
            continue

        # Sanity: ensure we have enough lines left for the event
        if i + 1 + n_particles > n_lines:
            # Incomplete event at EOF; stop cleanly
            break

        # --- Particle block ---
        event_lines = lines[i + 1: i + 1 + n_particles]
        i += 1 + n_particles  # advance cursor

        scattered_electron = None
        proton_momentum_mag = None

        for line in event_lines:
            parts = line.split()
            if len(parts) < 14:
                continue  # malformed particle line, skip

            # LUND particle fields (indices):
            # 0:index 1:lifetime 2:type 3:pid 4:parent 5:firstDau 6:px 7:py 8:pz 9:E 10:m 11:vx 12:vy 13:vz
            pid = _parse_int(parts[3], default=0)

            px = _parse_float(parts[6], default=0.0)
            py = _parse_float(parts[7], default=0.0)
            pz = _parse_float(parts[8], default=0.0)
            E  = _parse_float(parts[9], default=0.0)
            
            p_mag = math.sqrt(px*px + py*py + pz*pz)
            theta = math.degrees(math.acos(pz / p_mag)) if p_mag > 0 else 0.0
            phi   = math.degrees(math.atan2(py, px))

            if pid == 11:
                scattered_electron = ROOT.TLorentzVector(px, py, pz, E)
                h2_e_theta_p.Fill(p_mag, theta)
                h2_e_phi_p.Fill(p_mag, phi)
                h2_e_theta_phi.Fill(phi, theta)
            elif pid == 2212:
                proton_momentum_mag = p_mag
                h2_p_theta_p.Fill(p_mag, theta)
                h2_p_phi_p.Fill(p_mag, phi)
                h2_p_theta_phi.Fill(phi, theta)

        # --- Kinematics from scattered electron ---
        if scattered_electron:
            q = beam_vec - scattered_electron
            Q2 = -q.Mag2()
            nu = q.E()
            if Q2 > 0.0 and nu > 0.0:
                xbj = Q2 / (2.0 * PROTON_MASS * nu)
                if xbj > 0.0:
                    h2_q2_xbj.Fill(xbj, Q2)
                    h1_q2.Fill(Q2)
                    h1_xbj.Fill(xbj)
                    if proton_momentum_mag is not None:
                        h2_q2_pp.Fill(proton_momentum_mag, Q2)
                    event_count += 1


def draw_with_text(hist, outname, text_x=0.2, text_y=0.85):
    """
    Draws a histogram and stamps N = total processed events.
    Uses COLZ for TH2; default draw for TH1.
    """
    c = ROOT.TCanvas("c_" + hist.GetName(), "", 900, 700)
    draw_opt = "COLZ" if hist.InheritsFrom("TH2") else ""
    hist.Draw(draw_opt)

    lat = ROOT.TLatex()
    lat.SetNDC()
    lat.SetTextSize(0.04)
    lat.DrawLatex(text_x, text_y, f"N = {event_count}")

    c.SaveAs(outname)
    c.Close()


def main():
    if len(sys.argv) < 2:
        print("Usage:")
        print("  python3 check_lund.py /path/to/file.lund")
        print("  python3 check_lund.py '/path/to/dir/*.lund'")
        print("  python3 check_lund.py /path/to/dir")
        sys.exit(1)

    pattern = sys.argv[1]

    # Build a list of files: support directory, glob, or single file
    paths = []
    if os.path.isdir(pattern):
        paths = sorted(glob.glob(os.path.join(pattern, "*.lund")))
    else:
        # Pattern may be an explicit file or a glob
        matched = glob.glob(pattern)
        if matched:
            # Keep only files
            paths = sorted(p for p in matched if os.path.isfile(p))
        elif os.path.isfile(pattern):
            paths = [pattern]

    if not paths:
        print(f"No LUND files matched: {pattern}")
        sys.exit(1)

    print(f"Processing {len(paths)} file(s)")
    for p in paths:
        print(f"  -> {p}")
        process_file(p)
        
    preface = "my_multithread"

    # Draw/save plots once after all files are processed
    draw_with_text(h2_q2_xbj, f"{preface}_Q2_vs_xbj.pdf")
    draw_with_text(h1_q2, f"{preface}_Q2_hist.pdf")
    draw_with_text(h1_xbj, f"{preface}_xbj_hist.pdf")
    draw_with_text(h2_q2_pp, f"{preface}_Q2_vs_proton_pmag.pdf")
    
     # --- New plots ---
    draw_with_text(h2_e_theta_p, f"{preface}_electron_theta_vs_p.pdf")
    draw_with_text(h2_e_phi_p, f"{preface}_electron_phi_vs_p.pdf")
    draw_with_text(h2_e_theta_phi, f"{preface}_electron_theta_vs_phi.pdf")
    draw_with_text(h2_p_theta_p, f"{preface}_proton_theta_vs_p.pdf")
    draw_with_text(h2_p_phi_p, f"{preface}_proton_phi_vs_p.pdf")
    draw_with_text(h2_p_theta_phi, f"{preface}_proton_theta_vs_phi.pdf")

    print("Done. Wrote:")
    print(f"{preface}_Q2_vs_xbj.pdf")
    print(f"{preface}_Q2_hist.pdf")
    print(f"{preface}_xbj_hist.pdf")
    print(f"{preface}_Q2_vs_proton_pmag.pdf")
    print(f"{preface}_electron_theta_vs_p.pdf")
    print(f"{preface}_electron_phi_vs_p.pdf")
    print(f"{preface}_electron_theta_vs_phi.pdf")
    print(f"{preface}_proton_theta_vs_p.pdf")
    print(f"{preface}_proton_phi_vs_p.pdf")
    print(f"{preface}_proton_theta_vs_phi.pdf")


if __name__ == "__main__":
    # Ensure ROOT doesn’t pop interactive canvases
    ROOT.gROOT.SetBatch(True)
    main()
