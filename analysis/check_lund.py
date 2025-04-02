import os
import math
import ROOT
import concurrent.futures

def extract_momentum_from_lund(file_path):
    electron_results = []
    proton_results = []

    with open(file_path, 'r') as f:
        lines = []
        for line in f:
            lines.append(line.strip())
            if len(lines) == 3:
                try:
                    electron_line = lines[1].split()
                    proton_line = lines[2].split()

                    e_px, e_py, e_pz = map(float, electron_line[6:9])
                    p_px, p_py, p_pz = map(float, proton_line[6:9])

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

    return electron_results, proton_results

def process_all_lund_files():
    directory = "/lustre24/expphy/volatile/clas12/manavb/proton_electron_lund_multithread"
    files = [
        os.path.join(directory, f)
        for f in os.listdir(directory)
        if f.endswith(".lund")
    ]

    electron_data_all = []
    proton_data_all = []

    with concurrent.futures.ThreadPoolExecutor() as executor:
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
    electron_data, proton_data = process_all_lund_files()
    fill_and_plot_histograms(electron_data, proton_data)