#include <cstdlib>
#include <iostream>
#include <chrono>
#include <fstream> 
#include <vector>
#include <cmath>
#include <iomanip>  
#include <TFile.h>
#include <TTree.h>
#include <TApplication.h>
#include <TROOT.h>
#include <TDatabasePDG.h>
#include <TLorentzVector.h>
#include <TVector3.h>
#include <TH1.h>
#include <TH2.h>
#include <TChain.h>
#include <TCanvas.h>
#include <TBenchmark.h>
#include "clas12reader.h"
#include <TLine.h>
#include <TNtuple.h>

using namespace clas12;

void ProcessHipo(TString inputFile);

void hipo2rootExp() {
    TString inputFile;
    int isHipo = -1;
    
    for (Int_t i = 1; i < gApplication->Argc(); i++) {
        TString opt = gApplication->Argv(i);
        if ((opt.Contains(".dat") || opt.Contains(".txt"))) {
            inputFile = opt(5, opt.Sizeof());
            isHipo = 1;
        } else if (opt.Contains(".root")) {
            inputFile = opt(5, opt.Sizeof());
            isHipo = 0;
        }
    }
    
    if (isHipo < 0) {
        std::cout << " *** Please provide a ROOT or text input file name..." << std::endl;
        exit(0);
    }
    
    ProcessHipo(inputFile);
}

void ProcessHipo(TString inputFile) {
    auto db = TDatabasePDG::Instance();

    // Variables for TTree branches
    float px_prot_rec, py_prot_rec, pz_prot_rec, p_proton_rec;
    float vx_prot, vy_prot, vz_prot;
    int pid_proton, status_proton, sector_proton;

    float px_electron_rec, py_electron_rec, pz_electron_rec, p_electron_rec;
    float vx_electron, vy_electron, vz_electron;
    int pid_electron, status_electron, sector_electron;

    float edge1_electron = -1, edge2_electron = -1, edge3_electron = -1;
    float edge1_proton = -1, edge2_proton = -1, edge3_proton = -1;

    TFile outFile(Form("../../data/%s.root", inputFile.Data()), "recreate");
    TTree out_tree("out_tree", "out_tree");

    out_tree.Branch("px_prot_rec", &px_prot_rec);
    out_tree.Branch("py_prot_rec", &py_prot_rec);
    out_tree.Branch("pz_prot_rec", &pz_prot_rec);
    out_tree.Branch("p_proton_rec", &p_proton_rec);
    out_tree.Branch("vx_prot", &vx_prot);
    out_tree.Branch("vy_prot", &vy_prot);
    out_tree.Branch("vz_prot", &vz_prot);
    out_tree.Branch("pid_proton", &pid_proton);
    out_tree.Branch("status_proton", &status_proton);
    out_tree.Branch("sector_proton", &sector_proton);

    out_tree.Branch("px_electron_rec", &px_electron_rec);
    out_tree.Branch("py_electron_rec", &py_electron_rec);
    out_tree.Branch("pz_electron_rec", &pz_electron_rec);
    out_tree.Branch("p_electron_rec", &p_electron_rec);
    out_tree.Branch("pid_electron", &pid_electron);
    out_tree.Branch("status_electron", &status_electron);

    out_tree.Branch("edge1_electron", &edge1_electron);
    out_tree.Branch("edge2_electron", &edge2_electron);
    out_tree.Branch("edge3_electron", &edge3_electron);

    out_tree.Branch("edge1_proton", &edge1_proton);
    out_tree.Branch("edge2_proton", &edge2_proton);
    out_tree.Branch("edge3_proton", &edge3_proton);

    // Start timing
    auto start = std::chrono::high_resolution_clock::now();
    gBenchmark->Start("timer");
    int counter = 0;

    std::cout << "Reading file example program (HIPO) " << __cplusplus << std::endl;

    // Read file paths from input file
    std::ifstream file(inputFile.Data());
    if (!file.is_open()) {
        std::cerr << "Error opening file: " << inputFile << std::endl;
        exit(1);
    }

    std::vector<std::string> data;
    std::string line;
    while (std::getline(file, line)) {
        if (!line.empty()) data.push_back(line);
    }
    file.close();

    // Process each HIPO file
    for (const auto& filePath : data) {
        hipo::reader reader;
        reader.open(filePath.c_str());

        hipo::dictionary factory;
        reader.readDictionary(factory);

        hipo::event event;
        hipo::bank REC_particle(factory.getSchema("REC::Particle"));
        hipo::bank REC_traj(factory.getSchema("REC::Traj"));
        hipo::bank REC_track(factory.getSchema("REC::Track"));

        while (reader.next() == true) {
            counter++;
            if (counter == 1) continue;

            reader.read(event);
            event.getStructure(REC_particle);
            event.getStructure(REC_traj);
            event.getStructure(REC_track);

            int N = REC_particle.getRows();
            int N_track = REC_track.getRows();

            if (N == 0 || REC_traj.getRows() == 0 || REC_track.getRows() == 0) continue;

            std::vector<int> pid(N), status(N);
            for (int i = 0; i < N; i++) {
                pid[i] = REC_particle.getInt("pid", i);
                status[i] = REC_particle.getInt("status", i);
            }

            if (pid.size() == 0) continue;
            if (pid[0] != 11) continue;

            // Find first reconstructed proton in REC::Particle
            auto it_proton = std::find(pid.begin(), pid.end(), 2212);
            if (it_proton == pid.end()) continue; // No proton found
            int index = std::distance(pid.begin(), it_proton);

            int index_electron = 0;

            px_electron_rec = REC_particle.getFloat("px", index_electron);
            py_electron_rec = REC_particle.getFloat("py", index_electron);
            pz_electron_rec = REC_particle.getFloat("pz", index_electron);

            px_prot_rec = REC_particle.getFloat("px", index);
            py_prot_rec = REC_particle.getFloat("py", index);
            pz_prot_rec = REC_particle.getFloat("pz", index);

            p_proton_rec = std::sqrt(px_prot_rec * px_prot_rec + py_prot_rec * py_prot_rec + pz_prot_rec * pz_prot_rec);
            p_electron_rec = std::sqrt(px_electron_rec * px_electron_rec + py_electron_rec * py_electron_rec + pz_electron_rec * pz_electron_rec);

            vx_prot = REC_particle.getFloat("vx", index);
            vy_prot = REC_particle.getFloat("vy", index);
            vz_prot = REC_particle.getFloat("vz", index);

            status_proton = status[index];
            pid_proton = pid[index];

            status_electron = status[index_electron];
            pid_electron = pid[index_electron];

            if (pid_electron != 11) {
                std::cout << "pid electron: " << pid_electron << std::endl;
            }

            // Find corresponding track for the proton
            sector_proton = -1;
            for (int i = 0; i < N_track; i++) {
                if (REC_track.getInt("pindex", i) == index) {
                    sector_proton = REC_track.getInt("sector", i);
                    break;
                }
            }

            // Reset edge variables
            edge1_electron = edge2_electron = edge3_electron = -1;
            edge1_proton = edge2_proton = edge3_proton = -1;

            // Fill edge variables from REC::Traj
            for (int i = 0; i < REC_traj.getRows(); i++) {
                int pid_traj = REC_traj.getInt("pindex", i);
                int det = REC_traj.getInt("detector", i);
                int layer = REC_traj.getInt("layer", i);
                float edge = REC_traj.getFloat("edge", i);

                if (det != 6) continue;

                if (pid_traj == index_electron) {
                    if (layer == 6) edge1_electron = edge;
                    else if (layer == 18) edge2_electron = edge;
                    else if (layer == 36) edge3_electron = edge;
                }

                if (pid_traj == index) {
                    if (layer == 6) edge1_proton = edge;
                    else if (layer == 18) edge2_proton = edge;
                    else if (layer == 36) edge3_proton = edge;
                }
            }

            out_tree.Fill();
        }
    }

    outFile.Write();
    outFile.Close();
    std::cout << "Wrote data into a ROOT file." << std::endl;
}
