import os
import math
import ROOT
from concurrent.futures import ProcessPoolExecutor

ROOT.gROOT.SetBatch(True)

def parse_lund_momentum(line):
    tokens = line.split()
    return map(float, tokens[6:9])

def extract_momentum_from_lund(file_path, max_lines=None, return_raw=False):
    electron_results = []
    proton_results = []
    raw_lines = []

    with open(file_path, 'r') as f:
        lines = []
        line_count = 0
        for line in f:
            if max_lines and line_count >= max_lines:
                break
            lines.append(line.strip())
            line_count += 1
            if len(lines) == 5:
                try:
                    electron_line = lines[1]
                    proton_line = lines[4]

                    if return_raw:
                        raw_lines.append((electron_line, proton_line))

                    e_px, e_py, e_pz = parse_lund_momentum(electron_line)
                    p_px, p_py, p_pz = parse_lund_momentum(proton_line)

                    e_P = math.sqrt(e_px**2 + e_py**2 + e_pz**2)
                    p_P = math.sqrt(p_px**2 + p_py**2 + p_pz**2)

                    e_theta = math.degrees(math.acos(e_pz / e_P))
                    p_theta = math.degrees(math.acos(p_pz / p_P))

                    e_phi = math.degrees(math.atan2(e_py, e_px))
                    p_phi = math.degrees(math.atan2(p_py, p_px))

                    electron_results.append((e_P, e_theta, e_phi))
                    proton_results.append((p_P, p_theta, p_phi))

                except (IndexError, ValueError, ZeroDivisionError):
                    pass
                lines = []

    if return_raw:
        return electron_results, proton_results, raw_lines
    return electron_results, proton_results

def process_all_lund_files():
    directory = "/volatile/clas12/kenjo/hepgen/lund01/"
    files = [
        os.path.join(directory, f.name)
        for f in os.scandir(directory)
        if f.name.endswith(".lund")
    ]

    electron_data_all = []
    proton_data_all = []

    with ProcessPoolExecutor() as executor:
        futures = executor.map(extract_momentum_from_lund, files)
        for electron_data, proton_data in futures:
            electron_data_all.extend(electron_data)
            proton_data_all.extend(proton_data)

    return electron_data_all, proton_data_all

def fill_and_plot_histograms(electron_data, proton_data):
    # Constants
    MOM_BINS = 200
    THETA_BINS = 200
    PHI_BINS = 200

    P_MIN_PROTON, P_MAX_PROTON = 0, 6
    P_MIN_ELECTRON, P_MAX_ELECTRON = 0, 9
    THETA_MIN, THETA_MAX = 0, 90
    PHI_MIN, PHI_MAX = -190, 190

    # 1D Histograms
    electron_P_hist = ROOT.TH1F("electron_P", "Electron Momentum;P (GeV/c);Counts", MOM_BINS, P_MIN_ELECTRON, P_MAX_ELECTRON)
    proton_P_hist = ROOT.TH1F("proton_P", "Proton Momentum;P (GeV/c);Counts", MOM_BINS, P_MIN_PROTON, P_MAX_PROTON)

    # 2D Histograms
    electron_theta_P = ROOT.TH2F("electron_theta_P", "Electron #theta vs P;#theta (deg);P (GeV/c)", THETA_BINS, THETA_MIN, THETA_MAX, MOM_BINS, P_MIN_ELECTRON, P_MAX_ELECTRON)
    electron_phi_P   = ROOT.TH2F("electron_phi_P", "Electron #phi vs P;#phi (deg);P (GeV/c)", PHI_BINS, PHI_MIN, PHI_MAX, MOM_BINS, P_MIN_ELECTRON, P_MAX_ELECTRON)
    electron_phi_theta = ROOT.TH2F("electron_phi_theta", "Electron #phi vs #theta;#theta (deg);#phi (deg)", THETA_BINS, THETA_MIN, THETA_MAX, PHI_BINS, PHI_MIN, PHI_MAX)

    proton_theta_P = ROOT.TH2F("proton_theta_P", "Proton #theta vs P;#theta (deg);P (GeV/c)", THETA_BINS, THETA_MIN, THETA_MAX, MOM_BINS, P_MIN_PROTON, P_MAX_PROTON)
    proton_phi_P   = ROOT.TH2F("proton_phi_P", "Proton #phi vs P;#phi (deg);P (GeV/c)", PHI_BINS, PHI_MIN, PHI_MAX, MOM_BINS, P_MIN_PROTON, P_MAX_PROTON)
    proton_phi_theta = ROOT.TH2F("proton_phi_theta", "Proton #phi vs #theta;#theta (deg);#phi (deg)", THETA_BINS, THETA_MIN, THETA_MAX, PHI_BINS, PHI_MIN, PHI_MAX)

    # Fill histograms
    for P, theta, phi in electron_data:
        electron_P_hist.Fill(P)
        electron_theta_P.Fill(theta, P)
        electron_phi_P.Fill(phi, P)
        electron_phi_theta.Fill(theta, phi)

    for P, theta, phi in proton_data:
        proton_P_hist.Fill(P)
        proton_theta_P.Fill(theta, P)
        proton_phi_P.Fill(phi, P)
        proton_phi_theta.Fill(theta, phi)

    # Save ROOT file
    root_file = ROOT.TFile("histograms.root", "RECREATE")
    for h in [electron_P_hist, proton_P_hist,
              electron_theta_P, electron_phi_P, electron_phi_theta,
              proton_theta_P, proton_phi_P, proton_phi_theta]:
        h.Write()
    root_file.Close()

    # Save plots
    def save_canvas(hist, filename):
        c = ROOT.TCanvas()
        hist.Draw("COLZ" if isinstance(hist, ROOT.TH2) else "")
        c.SaveAs(filename)
        c.Close()

    save_canvas(electron_P_hist, "electron_P.png")
    save_canvas(proton_P_hist, "proton_P.png")
    save_canvas(electron_theta_P, "electron_theta_vs_P.png")
    save_canvas(electron_phi_P, "electron_phi_vs_P.png")
    save_canvas(electron_phi_theta, "electron_phi_vs_theta.png")
    save_canvas(proton_theta_P, "proton_theta_vs_P.png")
    save_canvas(proton_phi_P, "proton_phi_vs_P.png")
    save_canvas(proton_phi_theta, "proton_phi_vs_theta.png")

if __name__ == "__main__":
    TEST_MODE = False  # Set to False to process all files

    if TEST_MODE:
        test_file = "/volatile/clas12/kenjo/hepgen/lund01/hepgen-0000.lund"  # Replace with actual test filename
        electron_data, proton_data, raw_lines = extract_momentum_from_lund(test_file, max_lines=50, return_raw=True)

        for e_line, p_line in raw_lines:
            print("Electron line:", e_line)
            print("Proton line:  ", p_line)
            print("---")
    else:
        electron_data, proton_data = process_all_lund_files()
        fill_and_plot_histograms(electron_data, proton_data)