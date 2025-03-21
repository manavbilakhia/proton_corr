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

std::string root_file_path = "../data_test/ManavTest500Runs.dat.root";


// Define the output folder as a constant
const std::string OUTPUT_FOLDER = "../analysis_out_test500" + farm_out ;


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
    auto hist = rdf.Histo1D(ROOT::RDF::TH1DModel("delta_P", "delta_P (gen - rec); delta P (GeV); Events", 100, -0.5, 0.5), "delta_p");
    hist->Draw();
    canvas.SaveAs((OUTPUT_FOLDER + "delta_P.png").c_str());
    std::cout << "Saved 1D histogram as delta_P.png" << std::endl;
}

void plot_delta_P_above_below_1GeV(ROOT::RDF::RNode rdf) {
    auto rdf_above = rdf.Filter("p_proton_gen > 1");
    auto rdf_below = rdf.Filter("p_proton_gen < 1");
    TCanvas canvas("c1", "delta_P", 800, 600);
    canvas.Divide(1,2);
    canvas.cd(1);
    auto hist_above = rdf_above.Histo1D(ROOT::RDF::TH1DModel("delta_P_above_1GeV", "delta_P (gen - rec) for protons > 1 GeV; delta P (GeV); Events", 200, -0.5, 0.5), "delta_p");
    hist_above->Draw();
    canvas.cd(2);   
    auto hist_below = rdf_below.Histo1D(ROOT::RDF::TH1DModel("delta_P_below_1GeV", "delta_P (gen - rec) for protons < 1 GeV; delta P (GeV); Events", 200, -0.5, 0.5), "delta_p");
    hist_below->Draw();
    canvas.SaveAs((OUTPUT_FOLDER + "delta_P_above_below_1GeV.png").c_str());
    std::cout << "Saved 1D histogram as delta_P.png" << std::endl;
}

void plot_momenta_components(ROOT::RDF::RNode rdf) { // do not use loops, the graphs are too different for slicing and loopiong will lead to lazy eval
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
    auto hist2D = rdf.Histo2D(ROOT::RDF::TH2DModel("delta_P_VS_P_rec", "delta P vs P_rec;  P_rec (GeV); delta P (GeV)", 100, 0, 5, 100, -0.1, 0.1), "p_proton_rec", "delta_p");
    hist2D->Draw("COLZ");
    canvas.SaveAs((OUTPUT_FOLDER + "delta_P_VS_P_rec.png").c_str());
    std::cout << "Saved 2D histogram as delta_P_VS_P_rec.png" << std::endl;
}

void plot_delta_P_VS_P_rec_above_below_1GeV(ROOT::RDF::RNode rdf) {
    auto rdf_above = rdf.Filter("p_proton_rec > 1");
    auto rdf_below = rdf.Filter("p_proton_rec < 1");
    TCanvas canvas("c1", "delta_P", 800, 600);
    canvas.Divide(1,2);
    canvas.cd(1);
    auto hist2D_above = rdf_above.Histo2D(ROOT::RDF::TH2DModel("delta_P_VS_P_rec", "delta P vs P_rec for protons > 1 GeV;  P_rec (GeV); delta P (GeV)", 100, 0, 2.5, 100, -1, 1), "p_proton_rec", "delta_p");
    hist2D_above->Draw("COLZ");
    canvas.cd(2);   
    auto hist2D_below = rdf_below.Histo2D(ROOT::RDF::TH2DModel("delta_P_VS_P_rec", "delta P vs P_rec for protons < 1 GeV;  P_rec (GeV); delta P (GeV)", 100, 0, 2.5, 100, -1, 1), "p_proton_rec", "delta_p");
    hist2D_below->Draw("COLZ");
    canvas.SaveAs((OUTPUT_FOLDER + "delta_P_VS_P_rec_above_below_1GeV.png").c_str());
    std::cout << "Saved 1D histogram as delta_P_VS_P_rec_above_below_1GeV.png" << std::endl;
}

void plot_delta_P_VS_P_rec_FD_Theta_below_above(ROOT::RDF::RNode rdf){
    auto rdf_above = rdf.Filter("(Theta_rec > 27) && detector == \"FD\" ");
    auto rdf_below = rdf.Filter("(Theta_rec < 27) && detector == \"FD\" ");
    TCanvas canvas("c1", "delta_P", 800, 600);
    canvas.Divide(1,2);
    canvas.cd(1);
    auto hist2D_above = rdf_above.Histo2D(ROOT::RDF::TH2DModel("delta_P_VS_P_rec_above_Theta", "delta P vs P_rec for theta more than 27 deg;  P_rec (GeV); delta P (GeV)", 100, 0, 2.5, 100, -0.1, 0.1), "p_proton_rec", "delta_p");
    hist2D_above->Draw("COLZ");
    canvas.cd(2);
    auto hist2D_below = rdf_below.Histo2D(ROOT::RDF::TH2DModel("delta_P_VS_P_rec_below_Theta", "delta P vs P_rec for theta less than 27 deg;  P_rec (GeV); delta P (GeV)", 100, 0, 2.5, 100, -0.1, 0.1), "p_proton_rec", "delta_p");
    hist2D_below->Draw("COLZ");
    canvas.SaveAs((OUTPUT_FOLDER + "delta_P_VS_P_rec_FD_Theta_below_above.png").c_str());
    std::cout << "Saved 1D histogram as delta_P_VS_P_rec_FD_Theta_below_above.png" << std::endl;
}

void plot_P_rec_P_gen(ROOT::RDF::RNode rdf) {
    TCanvas canvas("c6", "P_rec VS P_gen", 800, 600);
    canvas.Divide(1,2);
    canvas.cd(1);
    auto hist1 = rdf.Histo1D(ROOT::RDF::TH1DModel("p_proton_gen", "p_proton_gen; p_proton_gen (GeV); Events", 100, 0, 2.5), "p_proton_gen");
    hist1->Draw();
    canvas.cd(2);
    auto hist2 = rdf.Histo1D(ROOT::RDF::TH1DModel("p_proton_rec", "p_proton_rec; p_proton_rec (GeV); Events", 100, 0, 2.5), "p_proton_rec");
    hist2->Draw();

    canvas.SaveAs((OUTPUT_FOLDER + "P_rec_P_gen.png").c_str());
    std::cout << "Saved 1D histogram as P_rec_P_gen.png" << std::endl;
}


void Theta_VS_momentum(ROOT::RDF::RNode rdf) {
    TCanvas canvas("c7", "Theta VS momentum", 800, 600);
    canvas.Divide(1,2);
    canvas.cd(1);
    auto hist1 = rdf.Histo2D(ROOT::RDF::TH2DModel("Theta_gen_VS_P_gen", "Theta_gen VS P_gen; P_gen (GeV); Theta_gen (deg);", 100, 0, 7, 100, 0, 100), "p_proton_gen", "Theta_gen" );
    hist1->Draw("COLZ");
    canvas.cd(2);
    auto hist2 = rdf.Histo2D(ROOT::RDF::TH2DModel("Theta_rec_VS_P_rec", "Theta_rec VS P_rec; P_rec (GeV); Theta_rec (deg)", 100, 0, 7, 100, 0, 100), "p_proton_rec", "Theta_rec");
    hist2->Draw("COLZ");

    canvas.SaveAs((OUTPUT_FOLDER + "Theta_VS_momentum.png").c_str());
    std::cout << "Saved 2D histogram as Theta_VS_momentum.png" << std::endl;
}

void Theta_VS_momentum_FD_CD(ROOT::RDF::RNode rdf) {
    TCanvas canvas("c8", "Theta VS momentum FD CD", 800, 600);
    canvas.Divide(2,2);
    canvas.cd(1);
    auto hist1 = rdf.Filter("detector == \"FD\"").Histo2D(ROOT::RDF::TH2DModel("Theta_gen_VS_P_gen_FD", "Theta_gen VS P_gen in FD; P_gen (GeV); Theta_gen (deg)", 100, 0, 5, 100, 0, 100),  "p_proton_gen", "Theta_gen" );
    hist1->Draw("COLZ");
    canvas.cd(2);
    auto hist2 = rdf.Filter("detector == \"FD\"").Histo2D(ROOT::RDF::TH2DModel("Theta_rec_VS_P_rec_FD", "Theta_rec VS P_rec in FD;  P_rec (GeV); Theta_rec (deg);", 100, 0, 5, 100, 0, 100), "p_proton_rec", "Theta_rec"  );
    hist2->Draw("COLZ");
    canvas.cd(3);
    auto hist3 = rdf.Filter("detector == \"CD\"").Histo2D(ROOT::RDF::TH2DModel("Theta_gen_VS_P_gen_CD", "Theta_gen VS P_gen in CD; P_gen (GeV); Theta_gen (deg); ", 100, 0, 5, 100, 0, 100), "p_proton_gen", "Theta_gen");
    hist3->Draw("COLZ");
    canvas.cd(4);
    auto hist4 = rdf.Filter("detector == \"CD\"").Histo2D(ROOT::RDF::TH2DModel("Theta_rec_VS_P_rec_CD", "Theta_rec VS P_rec in CD;  P_rec (GeV); Theta_rec (deg);",  100, 0, 5, 100, 0, 100), "p_proton_rec", "Theta_rec");
    hist4->Draw("COLZ");

    canvas.SaveAs((OUTPUT_FOLDER + "Theta_VS_momentum_FD_CD.png").c_str());
    std::cout << "Saved 2D histogram as Theta_VS_momentum_FD_CD.png" << std::endl;
}

void Phi_VS_momentum(ROOT::RDF::RNode rdf) {
    TCanvas canvas("c8", "Phi VS momentum", 800, 600);
    canvas.Divide(1,2);
    canvas.cd(1);
    auto hist1 = rdf.Histo2D(ROOT::RDF::TH2DModel("Phi_gen_VS_P_gen", "Phi_gen VS P_gen; Phi_gen (deg); P_gen (GeV)", 100, -200, 200, 100, 0, 5), "Phi_gen", "p_proton_gen");
    hist1->Draw("COLZ");
    canvas.cd(2);
    auto hist2 = rdf.Histo2D(ROOT::RDF::TH2DModel("Phi_rec_VS_P_rec", "Phi_rec VS P_rec; Phi_rec (deg); P_rec (GeV)", 100, -200, 200, 100, 0, 5), "Phi_rec",  "p_proton_rec");
    hist2->Draw("COLZ");

    canvas.SaveAs((OUTPUT_FOLDER + "Phi_VS_momentum.png").c_str());
    std::cout << "Saved 2D histogram as Phi_VS_momentum.png" << std::endl;
}

void Phi_VS_momentum_FD_CD(ROOT::RDF::RNode rdf) {
    TCanvas canvas("c8", "Phi VS momentum FD CD", 800, 600);
    canvas.Divide(2,2);
    canvas.cd(1);
    auto hist1 = rdf.Filter("detector == \"FD\"").Histo2D(ROOT::RDF::TH2DModel("Phi_gen_VS_P_gen_FD", "Phi_gen VS P_gen in FD;  Phi_gen (deg); P_gen (GeV)", 100, -200, 200, 100, 0, 5), "Phi_gen", "p_proton_gen" );
    hist1->Draw("COLZ");
    canvas.cd(2);
    auto hist2 = rdf.Filter("detector == \"FD\"").Histo2D(ROOT::RDF::TH2DModel("Phi_rec_VS_P_rec_FD", "Phi_rec VS P_rec in FD;  Phi_rec (deg); P_rec (GeV)", 100, -200, 200, 100, 0, 5), "Phi_rec", "p_proton_rec" );
    hist2->Draw("COLZ");
    canvas.cd(3);
    auto hist3 = rdf.Filter("detector == \"CD\"").Histo2D(ROOT::RDF::TH2DModel("Phi_gen_VS_P_gen_CD", "Phi_gen VS P_gen in CD; Phi_gen (deg); P_gen (GeV)", 100, -200, 200, 100, 0, 5), "Phi_gen", "p_proton_gen");
    hist3->Draw("COLZ");
    canvas.cd(4);
    auto hist4 = rdf.Filter("detector == \"CD\"").Histo2D(ROOT::RDF::TH2DModel("Phi_rec_VS_P_rec_CD", "Phi_rec VS P_rec in CD; Phi_rec (deg); P_rec (GeV)", 100, -200, 200, 100, 0, 5), "Phi_rec", "p_proton_rec");
    hist4->Draw("COLZ");

    canvas.SaveAs((OUTPUT_FOLDER + "Phi_VS_momentum_FD_CD.png").c_str());
    std::cout << "Saved 2D histogram as Phi_VS_momentum_FD_CD.png" << std::endl;
}

void Phi_VS_Theta(ROOT::RDF::RNode rdf) {
    TCanvas canvas("c9", "Phi VS Theta", 800, 600);
    canvas.Divide(1,2);
    canvas.cd(1);
    auto hist1 = rdf.Histo2D(ROOT::RDF::TH2DModel("Phi_gen_VS_Theta_gen", "Phi_gen VS Theta_gen; Phi_gen (deg); Theta_gen (deg)", 100, -200, 200, 100, 0, 100), "Phi_gen", "Theta_gen");
    hist1->Draw("COLZ");
    canvas.cd(2);
    auto hist2 = rdf.Histo2D(ROOT::RDF::TH2DModel("Phi_rec_VS_Theta_rec", "Phi_rec VS Theta_rec; Phi_rec (deg) ;Theta_rec (deg)",  100, -200, 200, 100, 0, 100), "Phi_rec", "Theta_rec");
    hist2->Draw("COLZ");

    canvas.SaveAs((OUTPUT_FOLDER + "Phi_VS_Theta.png").c_str());
    std::cout << "Saved 2D histogram as Phi_VS_Theta.png" << std::endl;
}

void Phi_VS_Theta_FD_CD(ROOT::RDF::RNode rdf) {
    TCanvas canvas("c10", "Phi VS Theta FD CD", 800, 600);
    canvas.Divide(2,2);
    canvas.cd(1);
    auto hist1 = rdf.Filter("detector == \"FD\"").Histo2D(ROOT::RDF::TH2DModel("Phi_gen_VS_Theta_gen_FD", "Phi_gen VS Theta_gen in FD;  Phi_gen (deg); Theta_gen (deg)", 100, -200, 200, 100, 0, 100), "Phi_gen", "Theta_gen" );
    hist1->Draw("COLZ");
    canvas.cd(2);
    auto hist2 = rdf.Filter("detector == \"FD\"").Histo2D(ROOT::RDF::TH2DModel("Phi_rec_VS_Theta_rec_FD", "Phi_rec VS Theta_rec in FD;  Phi_rec (deg); Theta_rec (deg)", 100, -200, 200, 100, 0, 100), "Phi_rec", "Theta_rec" );
    hist2->Draw("COLZ");
    canvas.cd(3);
    auto hist3 = rdf.Filter("detector == \"CD\"").Histo2D(ROOT::RDF::TH2DModel("Phi_gen_VS_Theta_gen_CD", "Phi_gen VS Theta_gen in CD; Phi_gen (deg); Theta_gen (deg)", 100, -200, 200, 100, 0, 100), "Phi_gen", "Theta_gen");
    hist3->Draw("COLZ");
    canvas.cd(4);
    auto hist4 = rdf.Filter("detector == \"CD\"").Histo2D(ROOT::RDF::TH2DModel("Phi_rec_VS_Theta_rec_CD", "Phi_rec VS Theta_rec in CD; Phi_rec (deg); Theta_rec (deg)", 100, -200, 200, 100, 0, 100), "Phi_rec", "Theta_rec");
    hist4->Draw("COLZ");

    canvas.SaveAs((OUTPUT_FOLDER + "Phi_VS_Theta_FD_CD.png").c_str());
    std::cout << "Saved 2D histogram as Phi_VS_Theta_FD_CD.png" << std::endl;
}

void delta_P_VS_P_rec_FD_CD(ROOT::RDF::RNode rdf) {
    TCanvas canvas("c", "delta_P_VS_P_rec_FD_CD", 800, 600);
    canvas.Divide(1,2);
    canvas.cd(1);
    auto hist2D_1 = rdf.Filter("detector == \"FD\"").Histo2D(ROOT::RDF::TH2DModel("delta_P_VS_P_rec_FD", "delta P vs P_rec in FD;  P_rec (GeV); delta P (GeV)", 100, 0, 2.5, 100, -0.1, 0.1), "p_proton_rec", "delta_p");
    hist2D_1->Draw("COLZ");
    canvas.cd(2);
    auto hist2D_2 = rdf.Filter("detector == \"CD\"").Histo2D(ROOT::RDF::TH2DModel("delta_P_VS_P_rec_CD", "delta P vs P_rec in CD;  P_rec (GeV); delta P (GeV)", 100, 0, 2.5, 100, -0.1, 0.1), "p_proton_rec", "delta_p");
    hist2D_2->Draw("COLZ");

    canvas.SaveAs((OUTPUT_FOLDER + "delta_P_VS_P_rec_FD_CD.png").c_str());
    std::cout << "Saved 2D histogram as delta_P_VS_P_rec_FD_CD.png" << std::endl;
}



void delta_P_VS_P_rec_FD_sectors(ROOT::RDF::RNode rdf) {
    // Apply initial filter for detector
    auto rdf_filtered = rdf.Filter("detector == \"FD\"");
    // Create 3D histogram: X = p_proton_rec, Y = delta_p, Z = sector_proton
    auto hist3D = rdf_filtered.Histo3D(
        ROOT::RDF::TH3DModel("delta_P_VS_P_rec_FD_3D",
                             "delta P vs P_rec vs Sector; P_rec (GeV); delta P (GeV); Sector",
                             100, 0, 2.5,   // X-axis: P_rec
                             100, -0.1, 0.1, // Y-axis: delta P
                             6, 0, 7),   // Z-axis: sector (1-6)
        "p_proton_rec", "delta_p", "sector_proton"
    );

    // Prepare Canvas
    TCanvas canvas("c", "delta_P_VS_P_rec_FD_sectors", 800, 600);
    canvas.Divide(3,2);

    // Slice 3D Histogram into 2D Histograms for each sector
    for (int sector = 1; sector <= 6; ++sector) {
        canvas.cd(sector);

        // Set Z-axis range to only include the current sector
        hist3D->GetZaxis()->SetRange(sector, sector);

        // Project X-Y plane (p_proton_rec vs delta_p) for this sector
        auto hist2D = hist3D->Project3D("yx"); // Correct projection: Y = delta_p, X = p_proton_rec
        hist2D->SetName(Form("delta_P_VS_P_rec_FD_sector%d", sector));
        hist2D->SetTitle(Form("delta P vs P_rec Sector %d; P_rec (GeV); delta P (GeV)", sector));

        hist2D->Draw("COLZ");
    }

    // Save the final canvas
    canvas.SaveAs((OUTPUT_FOLDER + "delta_P_VS_P_rec_FD_sectors.pdf").c_str());
    std::cout << "Saved 2D histograms from 3D histogram as delta_P_VS_P_rec_FD_sectors.png" << std::endl;
}

void delta_P_VS_P_rec_FD_sectors_no_loop(ROOT::RDF::RNode rdf){
    auto rdf_filtered = rdf.Filter("detector == \"FD\"");
    TCanvas canvas("c", "delta_P_VS_P_rec_FD_sectors_no_loop", 800, 600);
    canvas.Divide(3,2);
    canvas.cd(1);
    auto hist2D_1 = rdf_filtered.Filter("sector_proton == 1").Histo2D(ROOT::RDF::TH2DModel("delta_P_VS_P_rec_FD_sector1", "delta P vs P_rec Sector 1; P_rec (GeV); delta P (GeV)", 100, 0, 2.5, 100, -0.1, 0.1), "p_proton_rec", "delta_p");
    hist2D_1->Draw("COLZ");
    canvas.cd(2);
    auto hist2D_2 = rdf_filtered.Filter("sector_proton == 2").Histo2D(ROOT::RDF::TH2DModel("delta_P_VS_P_rec_FD_sector2", "delta P vs P_rec Sector 2; P_rec (GeV); delta P (GeV)", 100, 0, 2.5, 100, -0.1, 0.1), "p_proton_rec", "delta_p");
    hist2D_2->Draw("COLZ");
    canvas.cd(3);
    auto hist2D_3 = rdf_filtered.Filter("sector_proton == 3").Histo2D(ROOT::RDF::TH2DModel("delta_P_VS_P_rec_FD_sector3", "delta P vs P_rec Sector 3; P_rec (GeV); delta P (GeV)", 100, 0, 2.5, 100, -0.1, 0.1), "p_proton_rec", "delta_p");
    hist2D_3->Draw("COLZ");
    canvas.cd(4);
    auto hist2D_4 = rdf_filtered.Filter("sector_proton == 4").Histo2D(ROOT::RDF::TH2DModel("delta_P_VS_P_rec_FD_sector4", "delta P vs P_rec Sector 4; P_rec (GeV); delta P (GeV)", 100, 0, 2.5, 100, -0.1, 0.1), "p_proton_rec", "delta_p");
    hist2D_4->Draw("COLZ");
    canvas.cd(5);
    auto hist2D_5 = rdf_filtered.Filter("sector_proton == 5").Histo2D(ROOT::RDF::TH2DModel("delta_P_VS_P_rec_FD_sector5", "delta P vs P_rec Sector 5; P_rec (GeV); delta P (GeV)", 100, 0, 2.5, 100, -0.1, 0.1), "p_proton_rec", "delta_p");
    hist2D_5->Draw("COLZ");
    canvas.cd(6);
    auto hist2D_6 = rdf_filtered.Filter("sector_proton == 6").Histo2D(ROOT::RDF::TH2DModel("delta_P_VS_P_rec_FD_sector6", "delta P vs P_rec Sector 6; P_rec (GeV); delta P (GeV)", 100, 0, 2.5, 100, -0.1, 0.1), "p_proton_rec", "delta_p");
    hist2D_6->Draw("COLZ");

    canvas.SaveAs((OUTPUT_FOLDER + "delta_P_VS_P_rec_FD_sectors_no_loop.pdf").c_str());
    std::cout << "Saved 2D histograms as delta_P_VS_P_rec_FD_sectors_no_loop.pdf" << std::endl;


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
                        .Define("delta_p", "p_proton_rec - p_proton_gen")
                        .Define("proton_rec_4_momentum", "TLorentzVector(px_prot_rec, py_prot_rec, pz_prot_rec, 0.938272)")
                        .Define("proton_gen_4_momentum", "TLorentzVector(px_prot_gen, py_prot_gen, pz_prot_gen, 0.938272)")
                        .Define("Phi_rec", "proton_rec_4_momentum.Phi()*TMath::RadToDeg()")  
                        .Define("Phi_gen", "proton_gen_4_momentum.Phi()*TMath::RadToDeg()")
                        .Define("Theta_rec", "proton_rec_4_momentum.Theta()*TMath::RadToDeg()")
                        .Define("Theta_gen", "proton_gen_4_momentum.Theta()*TMath::RadToDeg()")
                        .Define("detector", [](int status) { 
                            return status < 4000 ? std::string("FD") : (status < 8000 ? std::string("CD") : std::string("NA")); 
                        }, {"status_proton"});

    // Print column names
    //std::cout << "Columns in RDataFrame:" << std::endl;
    //for (const auto &col : init_rdf.GetColumnNames()) {
    //    std::cout << col << std::endl;
    //}
     
    //init_rdf.Filter("delta_p == 0").Display()->Print();
    //init_rdf.Display({"detector", "status_proton", "sector_proton"}, 200)->Print();

    //init_rdf.Filter("detector == \"FD\" && sector_proton != 1 && sector_proton != 2 && sector_proton != 3 && sector_proton != 4 && sector_proton != 5 && sector_proton != 6 ").Display({"pid_proton", "status_proton", "detector","sector_proton"}, 100)->Print();
    //init_rdf.Filter("status_proton > 8000").Display({"pid_proton", "status_proton", "detector","sector_proton"}, 100)->Print();


    plot_delta_P_VS_P_rec(init_rdf);
    plot_delta_P(init_rdf);
    //plot_delta_P_above_below_1GeV(init_rdf);
    //plot_delta_P_VS_P_rec_above_below_1GeV(init_rdf);
    Theta_VS_momentum_FD_CD(init_rdf);
    Phi_VS_momentum_FD_CD(init_rdf);
    Phi_VS_Theta_FD_CD(init_rdf);
    //delta_P_VS_P_rec_FD_sectors(init_rdf);
    //delta_P_VS_P_rec_FD_sectors_no_loop(init_rdf);
    //delta_P_VS_P_rec_FD_CD(init_rdf);
    //Theta_VS_momentum_FD_CD(init_rdf);
    //Phi_VS_momentum_FD_CD(init_rdf);
    //Phi_VS_Theta_FD_CD(init_rdf);
    //plot_delta_P_VS_P_rec_FD_Theta_below_above(init_rdf);

    //init_rdf.Display({"p_proton_gen", "p_proton_rec", "delta_p"}, 10)->Print();
    auto end = std::chrono::high_resolution_clock::now(); // END

    std::chrono::duration<double> elapsed = end - start;
    std::cout << "Time of execution: " << elapsed.count() << " sec" << std::endl;

    return 0;
}
