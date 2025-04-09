// to run, use  g++ TTree2RDF.cxx -o executable `root-config --cflags --glibs`
// ./executable

#include <iostream>
#include "plots.cxx"
#include <string>
#include <vector>
#include <TFile.h>
#include <TTree.h>
#include <ROOT/RDataFrame.hxx>
#include <TLorentzVector.h>
#include <TCanvas.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TKey.h>
#include <fstream>
#include <TLine.h> 
#include <TLegend.h> 
#include <string>
#include <TMath.h>
#include <cmath>
#include <chrono>


int isData = 0;  // 1 for real data, 0 for MC
bool isBigStatistics = false;
bool toFarm = false;

std::string farm_out = (toFarm == true) ? "/farm_out/" : "/";
//std::string root_file_path = (isBigStatistics == true) 
//? "../data/AlexRuns.dat.root"
//: "../data_test/NickRuns.dat.root";

std::string root_file_path = "../data/andrey_runs_test.dat.root";


// Define the output folder as a constant
const std::string OUTPUT_FOLDER = "../analysis_out_andrey_runs_test" + farm_out ;


ROOT::RDataFrame convert_ttrees_to_rdataframe(const std::string &root_file_path) {
    TFile *file = TFile::Open(root_file_path.c_str(), "READ");
    if (!file || file->IsZombie()) {
        std::cerr << "Error: Cannot open ROOT file " << root_file_path << std::endl;
        return ROOT::RDataFrame(0);
    }

    std::vector<std::string> keys;
    TIter next(file->GetListOfKeys());
    TKey *key;
    while ((key = (TKey *)next())) {
        if (std::string(key->GetClassName()) == "TTree") {
            keys.push_back(key->GetName());
        }
    }

    if (keys.empty()) {
        std::cerr << "No TTrees found in the ROOT file." << std::endl;
        return ROOT::RDataFrame(0);
    }

    std::string tree_name = keys[0];
    std::cout << "Processing TTree: " << tree_name << std::endl;

    ROOT::RDataFrame rdf(tree_name, root_file_path);
    file->Close();
    return rdf;
}

// Use ROOT::RDF::RNode instead of RDataFrame& to fix type mismatch



int main() {
    auto start = std::chrono::high_resolution_clock::now(); // STRAT

    // Load ROOT file and convert TTrees to RDataFrame
    ROOT::EnableImplicitMT(); // Enable multi-threading
    auto rdf = convert_ttrees_to_rdataframe(root_file_path);
    if (rdf.GetColumnNames().empty()) {
        std::cerr << "Error: Could not create RDataFrame." << std::endl;
        return 1;
    }

    // Define necessary variables in RDataFrame
    
    auto init_rdf = rdf//.Filter(p_proton_gen > 0.0)
                        //.Filter(p_proton_rec > 0.0)
                        .Define("delta_p", "p_proton_rec - p_proton_gen")
                        .Define("proton_rec_4_momentum", "TLorentzVector(px_prot_rec, py_prot_rec, pz_prot_rec, 0.938272)")
                        .Define("proton_gen_4_momentum", "TLorentzVector(px_prot_gen, py_prot_gen, pz_prot_gen, 0.938272)")
                        .Define("Phi_rec", "proton_rec_4_momentum.Phi()*TMath::RadToDeg()")  
                        .Define("Phi_gen", "proton_gen_4_momentum.Phi()*TMath::RadToDeg()")
                        .Define("Theta_rec", "proton_rec_4_momentum.Theta()*TMath::RadToDeg()")
                        .Define("Theta_gen", "proton_gen_4_momentum.Theta()*TMath::RadToDeg()")
                        .Define("detector", [](int status) { 
                            return status < 4000 ? std::string("FD") : (status < 8000 ? std::string("CD") : std::string("NA")); 
                        }, {"status_proton"})
                        .Define("electron_rec_4_momentum", "TLorentzVector(px_electron_rec, py_electron_rec, pz_electron_rec, 0.0)")
                        .Define("electron_gen_4_momentum", "TLorentzVector(px_electron_gen, py_electron_gen, pz_electron_gen, 0.0)")
                        .Define("Phi_electron_rec", "electron_rec_4_momentum.Phi()*TMath::RadToDeg()")
                        .Define("Phi_electron_gen", "electron_gen_4_momentum.Phi()*TMath::RadToDeg()")
                        .Define("Theta_electron_rec", "electron_rec_4_momentum.Theta()*TMath::RadToDeg()")
                        .Define("Theta_electron_gen", "electron_gen_4_momentum.Theta()*TMath::RadToDeg()");
                

    // Print column names
    //std::cout << "Columns in RDataFrame:" << std::endl;
    //for (const auto &col : init_rdf.GetColumnNames()) {
    //    std::cout << col << std::endl;
    //}
     
    //init_rdf.Filter("delta_p == 0").Display()->Print();
    //init_rdf.Display({"detector", "status_proton", "sector_proton"}, 200)->Print();

    //init_rdf.Filter("detector == \"FD\" && sector_proton != 1 && sector_proton != 2 && sector_proton != 3 && sector_proton != 4 && sector_proton != 5 && sector_proton != 6 ").Display({"pid_proton", "status_proton", "detector","sector_proton"}, 100)->Print();
    //init_rdf.Filter("status_proton > 8000").Display({"pid_proton", "status_proton", "detector","sector_proton"}, 100)->Print();


    //plot_delta_P_VS_P_rec(init_rdf);
    //plot_delta_P(init_rdf);
    //plot_delta_P_above_below_1GeV(init_rdf);
    //plot_delta_P_VS_P_rec_above_below_1GeV(init_rdf);
    //Theta_VS_momentum_FD_CD(init_rdf);
    //Phi_VS_momentum_FD_CD(init_rdf);
    //Phi_VS_Theta_FD_CD(init_rdf);
    delta_P_VS_P_rec_FD_sectors_1D(init_rdf,OUTPUT_FOLDER);
    delta_P_VS_P_rec_FD_sectors_2D(init_rdf,OUTPUT_FOLDER);
    delta_P_VS_P_rec_CD_1D(init_rdf,OUTPUT_FOLDER);
    delta_P_VS_P_rec_FD_CD(init_rdf,OUTPUT_FOLDER);
    //Theta_VS_momentum_FD_CD(init_rdf);
    //Phi_VS_momentum_FD_CD(init_rdf);
    //Phi_VS_Theta_FD_CD(init_rdf);
    //plot_delta_P_VS_P_rec_FD_Theta_below_above(init_rdf);
    //gen_theta_VS_rec_theta(init_rdf);
    //gen_phi_VS_rec_phi(init_rdf);
    //gen_P_VS_rec_P(init_rdf);
    //plot_delta_P_VS_P_rec(init_rdf);

    //plot_momenta_components(init_rdf, OUTPUT_FOLDER);
    //delta_P_VS_P_rec_FD_CD(init_rdf, OUTPUT_FOLDER);

    //init_rdf.Display({"p_proton_gen", "p_proton_rec", "delta_p"}, 10)->Print();
    auto end = std::chrono::high_resolution_clock::now(); // END

    std::chrono::duration<double> elapsed = end - start;
    std::cout << "Time of execution: " << elapsed.count() << " sec" << std::endl;

    return 0;
}
