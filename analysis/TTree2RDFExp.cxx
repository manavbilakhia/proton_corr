// to run, use  g++ TTree2RDFExp.cxx -o executable_exp `root-config --cflags --glibs`
// ./executable_exp

#include <iostream>
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


int isData = 1;  // 1 for real data, 0 for MC
bool isBigStatistics = false;
bool toFarm = false;

std::string farm_out = (toFarm == true) ? "/farm_out/" : "/";
//std::string root_file_path = (isBigStatistics == true) 
//? "../data/AlexRuns.dat.root"
//: "../data_test/NickRuns.dat.root";

std::string root_file_path = "../data_test/nSidisRuns.dat.root";


// Define the output folder as a constant
const std::string OUTPUT_FOLDER = "../analysis_out_nSidis" + farm_out ;


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





void plot_momenta_components(ROOT::RDF::RNode rdf) { // do not use loops, the graphs are too different for slicing and loopiong will lead to lazy eval
    TCanvas canvas("c2", "momenta_components", 800, 600);
    canvas.Divide(3,1);
    canvas.cd(1);
    auto hist4 = rdf.Histo1D(ROOT::RDF::TH1DModel("px_proton_rec", "px_proton_rec; px_proton_rec (GeV); Events", 100, -2, 2), "px_prot_rec");
    hist4->Draw();
    canvas.cd(2);
    auto hist5 = rdf.Histo1D(ROOT::RDF::TH1DModel("py_proton_rec", "py_proton_rec; py_proton_rec (GeV); Events", 100, -2, 2), "py_prot_rec");
    hist5->Draw();
    canvas.cd(3);
    auto hist6 = rdf.Histo1D(ROOT::RDF::TH1DModel("pz_proton_rec", "pz_proton_rec; pz_proton_rec (GeV); Events", 100, 0, 8), "pz_prot_rec");
    hist6->Draw();

    canvas.SaveAs((OUTPUT_FOLDER + "momenta_components_Exp.pdf").c_str());
    std::cout << "Saved 1D histogram as momenta_components.pdf" << std::endl;
}




void plot_P_rec(ROOT::RDF::RNode rdf) {
    TCanvas canvas("c6", "P_rec", 800, 600);
    auto hist2 = rdf.Histo1D(ROOT::RDF::TH1DModel("p_proton_rec", "p_proton_rec; p_proton_rec (GeV); Events", 100, 0, 8), "p_proton_rec");
    hist2->Draw();

    canvas.SaveAs((OUTPUT_FOLDER + "P_rec.pdf").c_str());
    std::cout << "Saved 1D histogram as P_rec.pdf" << std::endl;
}


void Theta_VS_momentum(ROOT::RDF::RNode rdf) {
    TCanvas canvas("c7", "Theta VS momentum", 800, 600);
    auto hist2 = rdf.Histo2D(ROOT::RDF::TH2DModel("Theta_rec_VS_P_rec", "Theta_rec VS P_rec; P_rec (GeV); Theta_rec (deg)", 100, 0, 10, 100, 0, 180), "p_proton_rec", "Theta_rec");
    hist2->Draw("COLZ");

    canvas.SaveAs((OUTPUT_FOLDER + "Theta_VS_momentum.pdf").c_str());
    std::cout << "Saved 2D histogram as Theta_VS_momentum.pdf" << std::endl;
}

void Theta_VS_momentum_FD_CD(ROOT::RDF::RNode rdf) {
    TCanvas canvas("c8", "Theta VS momentum FD CD", 800, 600);
    canvas.Divide(1,2);
    canvas.cd(1);
    auto hist2 = rdf.Filter("detector == \"FD\"").Histo2D(ROOT::RDF::TH2DModel("Theta_rec_VS_P_rec_FD", "Theta_rec VS P_rec in FD;  P_rec (GeV); Theta_rec (deg);", 100, 0, 10, 100, 0, 180), "p_proton_rec", "Theta_rec"  );
    hist2->Draw("COLZ");
    canvas.cd(2);
    auto hist4 = rdf.Filter("detector == \"CD\"").Histo2D(ROOT::RDF::TH2DModel("Theta_rec_VS_P_rec_CD", "Theta_rec VS P_rec in CD;  P_rec (GeV); Theta_rec (deg);",  100, 0, 10, 100, 0, 180), "p_proton_rec", "Theta_rec");
    hist4->Draw("COLZ");

    canvas.SaveAs((OUTPUT_FOLDER + "Theta_VS_momentum_FD_CD.pdf").c_str());
    std::cout << "Saved 2D histogram as Theta_VS_momentum_FD_CD.pdf" << std::endl;
}

void Phi_VS_momentum(ROOT::RDF::RNode rdf) {
    TCanvas canvas("c8", "Phi VS momentum", 800, 600);
    auto hist2 = rdf.Histo2D(ROOT::RDF::TH2DModel("Phi_rec_VS_P_rec", "Phi_rec VS P_rec; Phi_rec (deg); P_rec (GeV)", 100, -200, 200, 100, 0, 10), "Phi_rec",  "p_proton_rec");
    hist2->Draw("COLZ");
    canvas.SaveAs((OUTPUT_FOLDER + "Phi_VS_momentum.pdf").c_str());
    std::cout << "Saved 2D histogram as Phi_VS_momentum.pdf" << std::endl;
}

void Phi_VS_momentum_FD_CD(ROOT::RDF::RNode rdf) {
    TCanvas canvas("c8", "Phi VS momentum FD CD", 800, 600);
    canvas.Divide(1,2);

    canvas.cd(1);
    auto hist2 = rdf.Filter("detector == \"FD\"").Histo2D(ROOT::RDF::TH2DModel("Phi_rec_VS_P_rec_FD", "Phi_rec VS P_rec in FD;  Phi_rec (deg); P_rec (GeV)", 100, -200, 200, 100, 0, 10), "Phi_rec", "p_proton_rec" );
    hist2->Draw("COLZ");
    canvas.cd(2);
    auto hist4 = rdf.Filter("detector == \"CD\"").Histo2D(ROOT::RDF::TH2DModel("Phi_rec_VS_P_rec_CD", "Phi_rec VS P_rec in CD; Phi_rec (deg); P_rec (GeV)", 100, -200, 200, 100, 0, 10), "Phi_rec", "p_proton_rec");
    hist4->Draw("COLZ");

    canvas.SaveAs((OUTPUT_FOLDER + "Phi_VS_momentum_FD_CD.pdf").c_str());
    std::cout << "Saved 2D histogram as Phi_VS_momentum_FD_CD.pdf" << std::endl;
}

void Phi_VS_Theta(ROOT::RDF::RNode rdf) {
    TCanvas canvas("c9", "Phi VS Theta", 800, 600);
    auto hist2 = rdf.Histo2D(ROOT::RDF::TH2DModel("Phi_rec_VS_Theta_rec", "Phi_rec VS Theta_rec; Phi_rec (deg) ;Theta_rec (deg)",  100, -200, 200, 100, 0, 180), "Phi_rec", "Theta_rec");
    hist2->Draw("COLZ");

    canvas.SaveAs((OUTPUT_FOLDER + "Phi_VS_Theta.pdf").c_str());
    std::cout << "Saved 2D histogram as Phi_VS_Theta.pdf" << std::endl;
}

void Phi_VS_Theta_FD_CD(ROOT::RDF::RNode rdf) {
    TCanvas canvas("c10", "Phi VS Theta FD CD", 800, 600);
    canvas.Divide(1,2);
    canvas.cd(1);
    auto hist2 = rdf.Filter("detector == \"FD\"").Histo2D(ROOT::RDF::TH2DModel("Phi_rec_VS_Theta_rec_FD", "Phi_rec VS Theta_rec in FD;  Phi_rec (deg); Theta_rec (deg)", 100, -200, 200, 100, 0, 180), "Phi_rec", "Theta_rec" );
    hist2->Draw("COLZ");
    canvas.cd(2);
    auto hist4 = rdf.Filter("detector == \"CD\"").Histo2D(ROOT::RDF::TH2DModel("Phi_rec_VS_Theta_rec_CD", "Phi_rec VS Theta_rec in CD; Phi_rec (deg); Theta_rec (deg)", 100, -200, 200, 100, 0, 180), "Phi_rec", "Theta_rec");
    hist4->Draw("COLZ");

    canvas.SaveAs((OUTPUT_FOLDER + "Phi_VS_Theta_FD_CD.pdf").c_str());
    std::cout << "Saved 2D histogram as Phi_VS_Theta_FD_CD.pdf" << std::endl;
}



int main() {
    auto start = std::chrono::high_resolution_clock::now(); // STRAT

    // Load ROOT file and convert TTrees to RDataFrame
    //ROOT::EnableImplicitMT(); // Enable multi-threading
    auto rdf = convert_ttrees_to_rdataframe(root_file_path);
    if (rdf.GetColumnNames().empty()) {
        std::cerr << "Error: Could not create RDataFrame." << std::endl;
        return 1;
    }

    // Define necessary variables in RDataFrame
    
    auto init_rdf = rdf//.Filter(p_proton_gen > 0.0)
                        //.Filter(p_proton_rec > 0.0)
                        .Define("proton_rec_4_momentum", "TLorentzVector(px_prot_rec, py_prot_rec, pz_prot_rec, 0.938272)")
                        .Define("Phi_rec", "proton_rec_4_momentum.Phi()*TMath::RadToDeg()")  
                        .Define("Theta_rec", "proton_rec_4_momentum.Theta()*TMath::RadToDeg()")
                        .Define("detector", [](int status) { 
                            return status < 4000 ? std::string("FD") : (status < 8000 ? std::string("CD") : std::string("NA")); 
                        }, {"status_proton"});

    // Print column names
    std::cout << "Columns in RDataFrame:" << std::endl;
    for (const auto &col : init_rdf.GetColumnNames()) {
        std::cout << col << std::endl;
    }
     
    //init_rdf.Filter("delta_p == 0").Display()->Print();
    //init_rdf.Display({"detector", "status_proton", "sector_proton"}, 200)->Print();

    //init_rdf.Filter("detector == \"FD\" && sector_proton != 1 && sector_proton != 2 && sector_proton != 3 && sector_proton != 4 && sector_proton != 5 && sector_proton != 6 ").Display({"pid_proton", "status_proton", "detector","sector_proton"}, 100)->Print();
    //init_rdf.Filter("status_proton > 8000").Display({"pid_proton", "status_proton", "detector","sector_proton"}, 100)->Print();


    plot_momenta_components(init_rdf);
    plot_P_rec(init_rdf);
    Theta_VS_momentum(init_rdf);
    Theta_VS_momentum_FD_CD(init_rdf);
    Phi_VS_momentum(init_rdf);
    Phi_VS_momentum_FD_CD(init_rdf);
    Phi_VS_Theta(init_rdf);
    Phi_VS_Theta_FD_CD(init_rdf);

    auto end = std::chrono::high_resolution_clock::now(); // END

    std::chrono::duration<double> elapsed = end - start;
    std::cout << "Time of execution: " << elapsed.count() << " sec" << std::endl;

    return 0;
}
