// to run, use  g++ TTree2RDF.cxx -o executable `root-config --cflags --glibs`
// ./executable

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
#include <cmath>
#include <chrono>


int isData = 0;  // 1 for real data, 0 for MC
bool isBigStatistics = false;
bool toFarm = false;

std::string farm_out = (toFarm == true) ? "/farm_out/" : "/";
std::string root_file_path = (isBigStatistics == true) 
? "../data/NickRuns.dat.root"
: "../data_test/NickRuns.dat.root";


// Define the output folder as a constant
const std::string OUTPUT_FOLDER = "../analysis_out" + farm_out ;


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

void plot_delta_P(ROOT::RDF::RNode rdf) {
    TCanvas canvas("c1", "delta_P", 800, 600);
    auto hist = rdf.Histo1D(ROOT::RDF::TH1DModel("delta_P", "delta_P (gen - rec); delta P (GeV); Events", 100, -2, 10), "delta_p");
    hist->Draw();
    canvas.SaveAs((OUTPUT_FOLDER + "delta_P.png").c_str());
    std::cout << "Saved 1D histogram as delta_P.png" << std::endl;
}

void plot_momenta_components(ROOT::RDF::RNode rdf) {
    TCanvas canvas("c2", "momenta_components", 800, 600);
    canvas.Divide(3,2);
    canvas.cd(1);
    auto hist1 = rdf.Histo1D(ROOT::RDF::TH1DModel("px_proton_gen", "px_proton_gen; px_proton_gen (GeV); Events", 100, -2, 2), "px_prot_gen");
    hist1->Draw();
    canvas.cd(2);
    auto hist2 = rdf.Histo1D(ROOT::RDF::TH1DModel("py_proton_gen", "py_proton_gen; py_proton_gen (GeV); Events", 100, -2, 2), "py_prot_gen");
    hist2->Draw();
    canvas.cd(3);   
    auto hist3 = rdf.Histo1D(ROOT::RDF::TH1DModel("pz_proton_gen", "pz_proton_gen; pz_proton_gen (GeV); Events", 100, 0, 8), "pz_prot_gen");
    hist3->Draw();
    canvas.cd(4);
    auto hist4 = rdf.Histo1D(ROOT::RDF::TH1DModel("px_proton_rec", "px_proton_rec; px_proton_rec (GeV); Events", 100, -2, 2), "px_prot_rec");
    hist4->Draw();
    canvas.cd(5);
    auto hist5 = rdf.Histo1D(ROOT::RDF::TH1DModel("py_proton_rec", "py_proton_rec; py_proton_rec (GeV); Events", 100, -2, 2), "py_prot_rec");
    hist5->Draw();
    canvas.cd(6);
    auto hist6 = rdf.Histo1D(ROOT::RDF::TH1DModel("pz_proton_rec", "pz_proton_rec; pz_proton_rec (GeV); Events", 100, 0, 3), "pz_prot_rec");
    hist6->Draw();

    canvas.SaveAs((OUTPUT_FOLDER + "momenta_components.png").c_str());
    std::cout << "Saved 1D histogram as momenta_components.png" << std::endl;
}


void plot_delta_P_VS_P_rec(ROOT::RDF::RNode rdf) {
    TCanvas canvas("c5", "delta P VS P_rec", 800, 600);
    auto hist2D = rdf.Histo2D(ROOT::RDF::TH2DModel("delta_P_VS_P_rec", "delta P vs P_rec;  P_rec (GeV); delta P (GeV)", 100, 0, 2.5, 100, -10, 10), "p_proton_rec", "delta_p");
    hist2D->Draw("COLZ");
    canvas.SaveAs((OUTPUT_FOLDER + "delta_P_VS_P_rec.png").c_str());
    std::cout << "Saved 2D histogram as delta_P_VS_P_rec.png" << std::endl;
}


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

    auto init_rdf = rdf.Define("delta_p", "return p_proton_gen - p_proton_rec ;");
    

    // Print column names
    std::cout << "Columns in RDataFrame:" << std::endl;
    for (const auto &col : init_rdf.GetColumnNames()) {
        std::cout << col << std::endl;
    }
     
    plot_delta_P_VS_P_rec(init_rdf);
    plot_delta_P(init_rdf);
    plot_momenta_components(init_rdf);
    
    auto end = std::chrono::high_resolution_clock::now(); // END

    std::chrono::duration<double> elapsed = end - start;
    std::cout << "Time of execution: " << elapsed.count() << " sec" << std::endl;

    return 0;
}
