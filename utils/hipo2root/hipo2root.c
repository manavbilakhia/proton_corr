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

void hipo2root() {
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
    float px_prot_gen, py_prot_gen, pz_prot_gen, p_proton_gen;
    float px_prot_rec, py_prot_rec, pz_prot_rec, p_proton_rec;
    float vx_prot, vy_prot, vz_prot;
    int pid_proton, status_proton, sector_proton;

    TFile outFile(Form("../../data_test/%s.root", inputFile.Data()), "recreate");
    TTree out_tree("out_tree", "out_tree");

    out_tree.Branch("px_prot_gen", &px_prot_gen);
    out_tree.Branch("py_prot_gen", &py_prot_gen);
    out_tree.Branch("pz_prot_gen", &pz_prot_gen);
    out_tree.Branch("px_prot_rec", &px_prot_rec);
    out_tree.Branch("py_prot_rec", &py_prot_rec);
    out_tree.Branch("pz_prot_rec", &pz_prot_rec);
    out_tree.Branch("p_proton_gen", &p_proton_gen);
    out_tree.Branch("p_proton_rec", &p_proton_rec);
    out_tree.Branch("vx_prot", &vx_prot);
    out_tree.Branch("vy_prot", &vy_prot);
    out_tree.Branch("vz_prot", &vz_prot);
    out_tree.Branch("pid_proton", &pid_proton);
    out_tree.Branch("status_proton", &status_proton);
    out_tree.Branch("sector_proton", &sector_proton);

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
        hipo::bank MC_event(factory.getSchema("MC::Event"));
        hipo::bank MC_particle(factory.getSchema("MC::Particle"));
        hipo::bank REC_track(factory.getSchema("REC::Track"));

        while (reader.next() == true) {
            counter++;
            if (counter == 1) continue;

            reader.read(event);
            event.getStructure(REC_particle);
            event.getStructure(MC_event);
            event.getStructure(MC_particle);
            event.getStructure(REC_track);

            int N = REC_particle.getRows();
            int N_track = REC_track.getRows();
            int N_mc_event = MC_event.getRows();
            int N_mc_particle = MC_particle.getRows();

            std::vector<int> pid(N), status(N);
            for (int i = 0; i < N; i++) {
                pid[i] = REC_particle.getInt("pid", i);
                status[i] = REC_particle.getInt("status", i);
            }

            std::vector<int> pid_mc(N_mc_particle);
            for (int i = 0; i < N_mc_particle; i++) {
                pid_mc[i] = MC_particle.getInt("pid", i);
            }

            // Find first reconstructed proton in REC::Particle
            auto it_proton = std::find(pid.begin(), pid.end(), 2212);
            if (it_proton == pid.end()) continue; // No proton found
            int index = std::distance(pid.begin(), it_proton);

            // Find first generated proton in MC::Particle
            auto it_proton_mc = std::find(pid_mc.begin(), pid_mc.end(), 2212);
            if (it_proton_mc == pid_mc.end()) continue; // No proton found
            int index_mc = std::distance(pid_mc.begin(), it_proton_mc);

            // Fill tree with data
            px_prot_gen = MC_particle.getFloat("px", index_mc);
            py_prot_gen = MC_particle.getFloat("py", index_mc);
            pz_prot_gen = MC_particle.getFloat("pz", index_mc);

            px_prot_rec = REC_particle.getFloat("px", index);
            py_prot_rec = REC_particle.getFloat("py", index);
            pz_prot_rec = REC_particle.getFloat("pz", index);

            p_proton_rec = std::sqrt(px_prot_rec * px_prot_rec + py_prot_rec * py_prot_rec + pz_prot_rec * pz_prot_rec);
            p_proton_gen = std::sqrt(px_prot_gen * px_prot_gen + py_prot_gen * py_prot_gen + pz_prot_gen * pz_prot_gen);

            vx_prot = REC_particle.getFloat("vx", index);
            vy_prot = REC_particle.getFloat("vy", index);
            vz_prot = REC_particle.getFloat("vz", index);

            status_proton = status[index];
            pid_proton = pid[index];

            // Find corresponding track for the proton
            sector_proton = -1; // Default value if sector is not found
            for (int i = 0; i < N_track; i++) {
                if (REC_track.getInt("pindex", i) == index) { // Match track to particle index
                    //unsigned char sector_raw = static_cast<unsigned char>(REC_track.getByte("sector", i));
                    //sector_proton = static_cast<int>(sector_raw);
                    sector_proton = REC_track.getInt("sector", i);
                    break;
                }
            } 
     
            out_tree.Fill();
        }
    }

    outFile.Write();
    outFile.Close();
    std::cout << "Wrote data into a ROOT file." << std::endl;
}
