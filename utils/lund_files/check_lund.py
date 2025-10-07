import ROOT
import os
import sys

# Constants
proton_mass = 0.938  # GeV
beam_energy = 10.6   # GeV
event_count = 0

# Histograms
h2_q2_xbj = ROOT.TH2D("h2_q2_xbj", "Q^{2} vs x_{Bj};x_{Bj};Q^{2} [GeV^{2}]", 100, 0, 1, 100, 0, 6)
h1_q2 = ROOT.TH1D("h1_q2", "Q^{2} Distribution;Q^{2} [GeV^{2}];Counts", 100, 0, 6)
h1_xbj = ROOT.TH1D("h1_xbj", "x_{Bj} Distribution;x_{Bj};Counts", 100, 0, 1)
h2_q2_pp = ROOT.TH2D("h2_q2_pp", "Q^{2} vs Proton Momentum Magnitude;|p_{proton}| [GeV];Q^{2} [GeV^{2}]", 100, 0, 3, 100, 0, 6)

# Beam 4-vector (massless electron along +z)
beam_vec = ROOT.TLorentzVector(0, 0, beam_energy, beam_energy)

def process_file(filepath):
    global event_count
    with open(filepath) as f:
        lines = [line.strip() for line in f if line.strip()]
    
    i = 0
    while i < len(lines):
        n_particles = int(lines[i].split()[0])
        event_lines = lines[i+1:i+1+n_particles]
        i += n_particles + 1

        scattered_electron = None
        proton_momentum_mag = None

        for line in event_lines:
            parts = line.split()
            pid = int(parts[3])
            charge = float(parts[1])

            px, py, pz = map(float, parts[6:9])
            if pid == 11 and charge < 0:
                E = float(parts[9])
                scattered_electron = ROOT.TLorentzVector(px, py, pz, E)
            elif pid == 2212:
                proton_momentum_mag = (px**2 + py**2 + pz**2)**0.5

        if scattered_electron:
            q = beam_vec - scattered_electron
            Q2 = -q.Mag2()
            nu = q.E()
            if Q2 > 0 and nu > 0:
                xbj = Q2 / (2 * proton_mass * nu)
                if xbj > 0:
                    h2_q2_xbj.Fill(xbj, Q2)
                    h1_q2.Fill(Q2)
                    h1_xbj.Fill(xbj)
                    if proton_momentum_mag is not None:
                        h2_q2_pp.Fill(proton_momentum_mag, Q2)
                    event_count += 1

def draw_with_text(hist, outname, text_x=0.2, text_y=0.85):
    c = ROOT.TCanvas()
    hist.Draw("COLZ" if isinstance(hist, ROOT.TH2) else "")
    l = ROOT.TLatex()
    l.SetNDC()
    l.SetTextSize(0.04)
    l.DrawLatex(text_x, text_y, f"N = {event_count}")
    c.SaveAs(outname)

# ---------- MAIN EXEC ----------
if len(sys.argv) < 2:
    print("Usage: python3 process_lund_root.py /path/to/file.lund")
    sys.exit(1)

filepath = sys.argv[1]
print(f"Processing {filepath}")
process_file(filepath)

# Plot outputs
draw_with_text(h2_q2_xbj, "Q2_vs_xbj.pdf")
draw_with_text(h1_q2, "Q2_hist.pdf")
draw_with_text(h1_xbj, "xbj_hist.pdf")
draw_with_text(h2_q2_pp, "Q2_vs_proton_pmag.pdf")
