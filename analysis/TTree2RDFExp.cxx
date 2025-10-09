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
#include <TPaveStats.h>


int isData = 1;  // 1 for real data, 0 for MC
bool isBigStatistics = false;
bool toFarm = false;

std::string farm_out = (toFarm == true) ? "/farm_out/" : "/";
//std::string root_file_path = (isBigStatistics == true) 
//? "../data/AlexRuns.dat.root"
//: "../data_test/NickRuns.dat.root";
// Sp2019DVPi0PRuns.dat.root

std::string root_file_path = "../data/Sp2019DVPi0PRuns.dat.root";


// Define the output folder as a constant
const std::string OUTPUT_FOLDER = "../analysis_in_Sp2019DVPi0P" + farm_out ;


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

//-------------------------------------------------------------------------------W, Q2 -----------------------------------------------------------
void plot_W_Q2_rec_from4v(ROOT::RDF::RNode rdf, const std::string& output_folder) {
    const double Ebeam = 10.6;
    const double Mp    = 0.938272;

    auto df = rdf
        // Q^2 = -q^2, with q = e_initial - e_final(rec)
        .Define("Q2_rec",
                [Ebeam](const TLorentzVector& e_rec_in) {
                    TLorentzVector e_f = e_rec_in;
                    if (e_f.E() == 0.0) e_f.SetE(e_f.P());  // treat as massless if E was set 0
                    TLorentzVector e_i(0.0, 0.0, Ebeam, Ebeam);
                    TLorentzVector q = e_i - e_f;
                    return -q.M2();
                },
                {"electron_rec_4_momentum"})
        // W = sqrt((p_target + q)^2), p_target = (0,0,0,Mp)
        .Define("W_rec",
                [Ebeam, Mp](const TLorentzVector& e_rec_in) {
                    TLorentzVector e_f = e_rec_in;
                    if (e_f.E() == 0.0) e_f.SetE(e_f.P());
                    TLorentzVector e_i(0.0, 0.0, Ebeam, Ebeam);
                    TLorentzVector q = e_i - e_f;
                    TLorentzVector p_target(0.0, 0.0, 0.0, Mp);
                    return (p_target + q).M();
                },
                {"electron_rec_4_momentum"});



    // 1) W distribution
    {
        TCanvas cW("cW_rec4v","W distribution",800,600);
        auto hW = df.Histo1D(
            ROOT::RDF::TH1DModel("hW_rec4v","W distribution;W (GeV);Counts",
                                 100, 1, 5),
            "W_rec");
        hW->SetLineWidth(2);
        hW->Draw("HIST");
        cW.SaveAs((output_folder + "W_rec4v.pdf").c_str());
    }

    // 2) Q^2 distribution
    {
        TCanvas cQ2("cQ2_rec4v","Q^{2} distribution",800,600);
        auto hQ2 = df.Histo1D(
            ROOT::RDF::TH1DModel("hQ2_rec4v","Q^{2} distribution;Q^{2} (GeV^{2});Counts",
                                 100, 0, 11),
            "Q2_rec");
        hQ2->SetLineWidth(2);
        hQ2->Draw("HIST");
        cQ2.SaveAs((output_folder + "Q2_rec4v.pdf").c_str());
    }

    // 3) 2D W vs Q^2 (X=Q^2, Y=W)
    {
        TCanvas c2D("cWQ2_rec4v","W vs Q^{2}",900,700);
        c2D.SetRightMargin(0.15);
        auto h2 = df.Histo2D(
            ROOT::RDF::TH2DModel("hWvsQ2_rec4v",
                                 "Q^{2} vs W; W (GeV); Q^{2} (GeV^{2})",
                                 100, 0, 5,
                                 100, 1 , 11),
            "W_rec", "Q2_rec");
        h2->Draw("COLZ");
        c2D.SaveAs((output_folder + "W_vs_Q2_rec4v.pdf").c_str());
    }

    std::cout << "[plot_W_Q2_rec_from4v] Saved: "
              << output_folder << "W_rec4v.pdf, "
              << output_folder << "Q2_rec4v.pdf, "
              << output_folder << "W_vs_Q2_rec4v.pdf" << std::endl;
}

//----------------------------------------------------------------------------------------------------------------------------------------------



void plot_momenta_components_proton(ROOT::RDF::RNode rdf) { // do not use loops, the graphs are too different for slicing and loopiong will lead to lazy eval
    TCanvas canvas("c2", "momenta_components", 1200, 800);
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




void plot_P_rec_proton(ROOT::RDF::RNode rdf) {
    TCanvas canvas("c6", "P_rec", 1200, 800);
    auto hist2 = rdf.Histo1D(ROOT::RDF::TH1DModel("p_proton_rec", "p_proton_rec; p_proton_rec (GeV); Events", 100, 0, 8), "p_proton_rec");
    hist2->Draw();

    canvas.SaveAs((OUTPUT_FOLDER + "P_rec.pdf").c_str());
    std::cout << "Saved 1D histogram as P_rec.pdf" << std::endl;
}


void Theta_VS_momentum_proton(ROOT::RDF::RNode rdf) {
    TCanvas canvas("c7", "Theta VS momentum", 1200, 800);
    auto hist2 = rdf.Histo2D(ROOT::RDF::TH2DModel("Theta_rec_VS_P_rec", "Theta_rec VS P_rec; P_rec (GeV); Theta_rec (deg)", 100, 0, 10, 100, 0, 180), "p_proton_rec", "Theta_rec");
    hist2->Draw("COLZ");

    canvas.SaveAs((OUTPUT_FOLDER + "Theta_VS_momentum_proton.pdf").c_str());
    std::cout << "Saved 2D histogram as Theta_VS_momentum.pdf" << std::endl;
}

void Theta_VS_momentum_electron(ROOT::RDF::RNode rdf) {
    TCanvas canvas("c7", "Theta VS momentum", 1200, 800);
    auto hist2 = rdf.Histo2D(ROOT::RDF::TH2DModel("Theta_rec_VS_P_rec", "Theta_rec VS P_rec; P_rec (GeV); Theta_rec (deg)", 100, 0, 10, 100, 0, 50), "p_electron_rec", "Theta_electron_rec");
    hist2->Draw("COLZ");

    canvas.SaveAs((OUTPUT_FOLDER + "Theta_VS_momentum_electron.pdf").c_str());
    std::cout << "Saved 2D histogram as Theta_VS_momentum_electron.pdf" << std::endl;
}

void Theta_VS_momentum_FD_CD_proton(ROOT::RDF::RNode rdf) {
    TCanvas canvas("c8", "Theta VS momentum FD CD", 1200, 800);
    canvas.Divide(1,2);
    canvas.cd(1);
    auto hist2 = rdf.Filter("detector == \"FD\"").Histo2D(ROOT::RDF::TH2DModel("Theta_rec_VS_P_rec_FD", "Theta_rec VS P_rec in FD;  P_rec (GeV); Theta_rec (deg);", 100, 0, 10, 100, 0, 90), "p_proton_rec", "Theta_rec"  );
    hist2->Draw("COLZ");
    canvas.cd(2);
    auto hist4 = rdf.Filter("detector == \"CD\"").Histo2D(ROOT::RDF::TH2DModel("Theta_rec_VS_P_rec_CD", "Theta_rec VS P_rec in CD;  P_rec (GeV); Theta_rec (deg);",  100, 0, 10, 100, 0, 180), "p_proton_rec", "Theta_rec");
    hist4->Draw("COLZ");

    canvas.SaveAs((OUTPUT_FOLDER + "Theta_VS_momentum_FD_CD_proton.pdf").c_str());
    std::cout << "Saved 2D histogram as Theta_VS_momentum_FD_CD.pdf" << std::endl;
}

void Theta_VS_momentum_FD_proton_theta_gt_40(ROOT::RDF::RNode rdf) {
    TCanvas canvas("c_fd_theta_gt_40", "Theta vs P (FD, Theta > 40)", 1200, 800);
    auto hist = rdf.Filter("detector == \"FD\" && Theta_rec > 40")
                  .Histo2D(ROOT::RDF::TH2DModel("Theta_rec_VS_P_rec_FD_theta_gt_40",
                                                "Theta_rec vs P_rec in FD (Theta > 40 deg); P_rec (GeV); Theta_rec (deg)",
                                                100, 0, 10, 100, 30, 90),
                           "p_proton_rec", "Theta_rec");
    hist->Draw("COLZ");

    canvas.SaveAs((OUTPUT_FOLDER + "Theta_VS_momentum_FD_theta_gt_40.pdf").c_str());
    std::cout << "Saved 2D histogram as Theta_VS_momentum_FD_theta_gt_40.pdf" << std::endl;
}

void Phi_VS_momentum_proton(ROOT::RDF::RNode rdf) {
    TCanvas canvas("c8", "Phi VS momentum", 1200, 800);
    auto hist2 = rdf.Histo2D(ROOT::RDF::TH2DModel("Phi_rec_VS_P_rec", "Phi_rec VS P_rec; Phi_rec (deg); P_rec (GeV)", 100, -200, 200, 100, 0, 10), "Phi_rec",  "p_proton_rec");
    hist2->Draw("COLZ");
    canvas.SaveAs((OUTPUT_FOLDER + "Phi_VS_momentum_proton.pdf").c_str());
    std::cout << "Saved 2D histogram as Phi_VS_momentum.pdf" << std::endl;
}

void Phi_VS_momentum_FD_CD_proton(ROOT::RDF::RNode rdf) {
    TCanvas canvas("c8", "Phi VS momentum FD CD", 1200, 800);
    canvas.Divide(1,2);

    canvas.cd(1);
    auto hist2 = rdf.Filter("detector == \"FD\"").Histo2D(ROOT::RDF::TH2DModel("Phi_rec_VS_P_rec_FD", "Phi_rec VS P_rec in FD;  Phi_rec (deg); P_rec (GeV)", 100, -200, 200, 100, 0, 10), "Phi_rec", "p_proton_rec" );
    hist2->Draw("COLZ");
    canvas.cd(2);
    auto hist4 = rdf.Filter("detector == \"CD\"").Histo2D(ROOT::RDF::TH2DModel("Phi_rec_VS_P_rec_CD", "Phi_rec VS P_rec in CD; Phi_rec (deg); P_rec (GeV)", 100, -200, 200, 100, 0, 10), "Phi_rec", "p_proton_rec");
    hist4->Draw("COLZ");

    canvas.SaveAs((OUTPUT_FOLDER + "Phi_VS_momentum_FD_CD_proton.pdf").c_str());
    std::cout << "Saved 2D histogram as Phi_VS_momentum_FD_CD.pdf" << std::endl;
}

void Phi_VS_Theta_proton(ROOT::RDF::RNode rdf) {
    TCanvas canvas("c9", "Phi VS Theta", 1200, 800);
    auto hist2 = rdf.Histo2D(ROOT::RDF::TH2DModel("Phi_rec_VS_Theta_rec", "Phi_rec VS Theta_rec; Phi_rec (deg) ;Theta_rec (deg)",  100, -200, 200, 100, 0, 180), "Phi_rec", "Theta_rec");
    hist2->Draw("COLZ");

    canvas.SaveAs((OUTPUT_FOLDER + "Phi_VS_Theta_proton.pdf").c_str());
    std::cout << "Saved 2D histogram as Phi_VS_Theta.pdf" << std::endl;
}

void Phi_VS_Theta_FD_CD_proton(ROOT::RDF::RNode rdf) {
    TCanvas canvas("c10", "Phi VS Theta FD CD", 1200, 800);
    canvas.Divide(1,2);
    canvas.cd(1);
    auto hist2 = rdf.Filter("detector == \"FD\"").Histo2D(ROOT::RDF::TH2DModel("Phi_rec_VS_Theta_rec_FD", "Phi_rec VS Theta_rec in FD;  Phi_rec (deg); Theta_rec (deg)", 100, -200, 200, 100, 0, 180), "Phi_rec", "Theta_rec" );
    hist2->Draw("COLZ");
    canvas.cd(2);
    auto hist4 = rdf.Filter("detector == \"CD\"").Histo2D(ROOT::RDF::TH2DModel("Phi_rec_VS_Theta_rec_CD", "Phi_rec VS Theta_rec in CD; Phi_rec (deg); Theta_rec (deg)", 100, -200, 200, 100, 0, 180), "Phi_rec", "Theta_rec");
    hist4->Draw("COLZ");

    canvas.SaveAs((OUTPUT_FOLDER + "Phi_VS_Theta_FD_CD_proton.pdf").c_str());
    std::cout << "Saved 2D histogram as Phi_VS_Theta_FD_CD.pdf" << std::endl;
}

//-----------------------------------------------------------------DC fiducial cut functions --------------------------------------------------------------------------------------------------------------//

void Theta_VS_momentum_FD_proton_fiducial_cut(ROOT::RDF::RNode rdf) {
    TCanvas canvas("c8", "Theta VS momentum FD with Fiducial cut", 1200, 800);

    auto hist2 = rdf.Filter("detector == \"FD\" && DC_fiducial_cut_proton == true").Histo2D(ROOT::RDF::TH2DModel("Theta_rec_VS_P_rec_FD", "Theta_rec VS P_rec in FD Fiducial cuts ON;  P_rec (GeV); Theta_rec (deg);", 100, 0, 10, 100, 0, 90), "p_proton_rec", "Theta_rec"  );
    hist2->Draw("COLZ");

    canvas.SaveAs((OUTPUT_FOLDER + "Theta_VS_momentum_FD_proton_fiducial_cut.pdf").c_str());
    std::cout << "Saved 2D histogram as Theta_VS_momentum_FD_proton_fiducial_cut.pdf" << std::endl;
}

void Theta_VS_momentum_FD_proton_theta_gt_40_fiducial_cut(ROOT::RDF::RNode rdf) {
    TCanvas canvas("c_fd_theta_gt_40", "Theta vs P (FD, Theta > 40, Fid cuts on)", 1200, 800);
    auto hist = rdf.Filter("detector == \"FD\" && Theta_rec > 40 && DC_fiducial_cut_proton == true && DC_fiducial_cut_electron == true")
                  .Histo2D(ROOT::RDF::TH2DModel("Theta_rec_VS_P_rec_FD_theta_gt_40",
                                                "Theta_rec vs P_rec in FD (Theta > 40 deg, Fiducial cuts ON); P_rec (GeV); Theta_rec (deg)",
                                                100, 0, 10, 100, 30, 90),
                           "p_proton_rec", "Theta_rec");
    hist->Draw("COLZ");

    canvas.SaveAs((OUTPUT_FOLDER + "Theta_VS_momentum_FD_theta_gt_40_fiducial_cut.pdf").c_str());
    std::cout << "Saved 2D histogram as Theta_VS_momentum_FD_theta_gt_40_fiducial_cut.pdf" << std::endl;
}

void Theta_VS_momentum_electron_fiducial_cut(ROOT::RDF::RNode rdf) {
    TCanvas canvas("c7", "Theta VS momentum", 1200, 800);
    auto hist2 = rdf.Filter("DC_fiducial_cut_proton == true && DC_fiducial_cut_electron == true").Histo2D(ROOT::RDF::TH2DModel("Theta_rec_VS_P_rec", "Theta_rec VS P_rec Fiducial cuts ON; P_rec (GeV); Theta_rec (deg)", 100, 0, 10, 100, 0, 50), "p_electron_rec", "Theta_electron_rec");
    hist2->Draw("COLZ");

    canvas.SaveAs((OUTPUT_FOLDER + "Theta_VS_momentum_electron_fiducial_cuts.pdf").c_str());
    std::cout << "Saved 2D histogram as Theta_VS_momentum_electron.pdf" << std::endl;

}

void Phi_VS_momentum_FD_CD_proton_fiducial_cut(ROOT::RDF::RNode rdf) {
    TCanvas canvas("c8", "Phi VS momentum FD CD", 1200, 800);
    canvas.Divide(1,2);

    canvas.cd(1);
    auto hist2 = rdf.Filter("detector == \"FD\" && DC_fiducial_cut_proton == true && DC_fiducial_cut_electron == true").Histo2D(ROOT::RDF::TH2DModel("Phi_rec_VS_P_rec_FD", "Phi_rec VS P_rec in FD Fiducial cuts ON;  Phi_rec (deg); P_rec (GeV)", 100, -200, 200, 100, 0, 10), "Phi_rec", "p_proton_rec" );
    hist2->Draw("COLZ");
    canvas.cd(2);
    auto hist4 = rdf.Filter("detector == \"CD\" && DC_fiducial_cut_proton == true && DC_fiducial_cut_electron == true").Histo2D(ROOT::RDF::TH2DModel("Phi_rec_VS_P_rec_CD", "Phi_rec VS P_rec in CD Fiducial cuts ON; Phi_rec (deg); P_rec (GeV)", 100, -200, 200, 100, 0, 10), "Phi_rec", "p_proton_rec");
    hist4->Draw("COLZ");

    canvas.SaveAs((OUTPUT_FOLDER + "Phi_VS_momentum_FD_CD_proton_fiducial_cut.pdf").c_str());
    std::cout << "Saved 2D histogram as Phi_VS_momentum_FD_CD.pdf" << std::endl;
}

void Phi_VS_Theta_FD_CD_proton_fiducial_cut(ROOT::RDF::RNode rdf) {
    TCanvas canvas("c10", "Phi VS Theta FD CD", 1200, 800);
    canvas.Divide(1,2);
    canvas.cd(1);
    auto hist2 = rdf.Filter("detector == \"FD\" && DC_fiducial_cut_proton == true && DC_fiducial_cut_electron == true ").Histo2D(ROOT::RDF::TH2DModel("Phi_rec_VS_Theta_rec_FD", "Phi_rec VS Theta_rec in FD Fiducial cuts ON;  Phi_rec (deg); Theta_rec (deg)", 100, -200, 200, 100, 0, 180), "Phi_rec", "Theta_rec" );
    hist2->Draw("COLZ");
    canvas.cd(2);
    auto hist4 = rdf.Filter("detector == \"CD\" && DC_fiducial_cut_proton == true && DC_fiducial_cut_electron == true").Histo2D(ROOT::RDF::TH2DModel("Phi_rec_VS_Theta_rec_CD", "Phi_rec VS Theta_rec in CD Fiducial cuts ON; Phi_rec (deg); Theta_rec (deg)", 100, -200, 200, 100, 0, 180), "Phi_rec", "Theta_rec");
    hist4->Draw("COLZ");


    canvas.SaveAs((OUTPUT_FOLDER + "Phi_VS_Theta_FD_CD_proton_fiducial_cut.pdf").c_str());
    std::cout << "Saved 2D histogram as Phi_VS_Theta_FD_CD.pdf" << std::endl;
}

//----------------------------------------------------Andrey asked me----------------------------------------------------------------------//
void plot_Q2_xB_and_protonP_from_real_data(ROOT::RDF::RNode rdf) {

        // Q² vs x_bj
    auto h2 = rdf.Histo2D(
        ROOT::RDF::TH2DModel("Q2_vs_xbj", "Q^{2} vs x_{Bj};x_{Bj};Q^{2} [GeV^{2}]",
                             100, 0, 1, 100, 0, 10),"xB", "Q2");
    TCanvas c1("c1", "Q2 vs xbj", 800, 600);
    h2->Draw("COLZ");
    c1.SaveAs((OUTPUT_FOLDER + "Q2_vs_xbj_data.pdf").c_str());

    // Q² 1D
    auto hQ2 = rdf.Histo1D(
        ROOT::RDF::TH1DModel("Q2_dist", "Q^{2} Distribution;Q^{2} [GeV^{2}];Events", 100, 0, 10),"Q2");
    TCanvas c2("c2", "Q2 dist", 800, 600);
    hQ2->Draw();
    c2.SaveAs((OUTPUT_FOLDER + "Q2_hist_data.pdf").c_str());

      // xB 1D
    auto h_xB = rdf.Histo1D(
        ROOT::RDF::TH1DModel("h_xB", "x_{B} Distribution;xB;Events", 100, 0, 1),"xB");
    TCanvas c3("c3", "xB dist", 800, 600);
    h_xB->Draw();
    c3.SaveAs((OUTPUT_FOLDER + "xB_hist_data.pdf").c_str());

    auto h_Q2_vs_protonP = rdf.Histo2D(
        ROOT::RDF::TH2DModel("Q2_vs_protonP", "Q^{2} vs P_{proton};P_{proton} [GeV];Q^{2} [GeV^{2}]",
                             100, 0, 10, 100, 0, 10),"p_proton_rec", "Q2");
    TCanvas c4("c4", "Q2 vs protonP", 800, 600);
    h_Q2_vs_protonP->Draw("COLZ");
    c4.SaveAs((OUTPUT_FOLDER + "Q2_vs_protonP_data.pdf").c_str());

    std::cout << "Saved Q² vs xB and proton momentum plots." << std::endl;

}


//--------------------------------------------------------------------------------------------------------------------------------------------------//

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
                        .Define("proton_rec_4_momentum", "TLorentzVector(px_prot_rec, py_prot_rec, pz_prot_rec, sqrt(px_prot_rec*px_prot_rec+py_prot_rec*py_prot_rec+pz_prot_rec*pz_prot_rec+0.938272*0.938272) )")
                        .Define("Phi_rec", "proton_rec_4_momentum.Phi()*TMath::RadToDeg()")  
                        .Define("Theta_rec", "proton_rec_4_momentum.Theta()*TMath::RadToDeg()")
                        .Define("detector", [](int status) { 
                            return status < 4000 ? std::string("FD") : (status < 8000 ? std::string("CD") : std::string("NA")); 
                        }, {"status_proton"})
                        .Define("electron_rec_4_momentum", "TLorentzVector(px_electron_rec, py_electron_rec, pz_electron_rec, sqrt(px_electron_rec*px_electron_rec+py_electron_rec*py_electron_rec+pz_electron_rec*pz_electron_rec+0.000511*0.000511) )")
                        .Define("Phi_electron_rec", "electron_rec_4_momentum.Phi()*TMath::RadToDeg()")
                        .Define("Theta_electron_rec", "electron_rec_4_momentum.Theta()*TMath::RadToDeg()")
                        .Define("DC_fiducial_cut_electron", "detector == \"FD\" && edge1_electron > 7.0 && edge2_electron > 7.0 && edge3_electron > 15.0")
                        .Define("DC_fiducial_cut_proton", "detector == \"FD\" && edge1_proton > 7.0 && edge2_proton > 7.0 && edge3_proton > 12.0")
                        .Define("el_initial", "TLorentzVector(0, 0, 10.6, 10.6)")
                        .Define("Q2", "-(el_initial-electron_rec_4_momentum).M2()")
                        .Define("nu", "el_initial.E() - electron_rec_4_momentum.E()")
                        .Define("xB", "Q2/(2*0.938272*nu)");
                        

    // Print column names
    //std::cout << "Columns in RDataFrame:" << std::endl;
    //for (const auto &col : init_rdf.GetColumnNames()) {
    //    std::cout << col << std::endl;
    //}
     
    //init_rdf.Filter("delta_p == 0").Display()->Print();
    //init_rdf.Display({"detector", "status_proton", "sector_proton"}, 200)->Print();

    //init_rdf.Filter("detector == \"FD\" && sector_proton != 1 && sector_proton != 2 && sector_proton != 3 && sector_proton != 4 && sector_proton != 5 && sector_proton != 6 ").Display({"pid_proton", "status_proton", "detector","sector_proton"}, 100)->Print();
    //init_rdf.Filter("status_proton > 8000").Display({"pid_proton", "status_proton", "detector","sector_proton"}, 100)->Print();

    //Theta_VS_momentum_electron(init_rdf);
    //Theta_VS_momentum_electron_fiducial_cut(init_rdf);
    //Phi_VS_Theta_FD_CD_proton(init_rdf);
    //Phi_VS_Theta_FD_CD_proton_fiducial_cut(init_rdf);
    //Phi_VS_momentum_FD_CD_proton(init_rdf);
    //Phi_VS_momentum_FD_CD_proton_fiducial_cut(init_rdf);
    //Theta_VS_momentum_FD_proton_fiducial_cut(init_rdf);
    //Theta_VS_momentum_FD_CD_proton(init_rdf);
    //Theta_VS_momentum_FD_proton_theta_gt_40_fiducial_cut(init_rdf);
    //Theta_VS_momentum_FD_proton_theta_gt_40(init_rdf);
    //plot_Q2_xB_and_protonP_from_real_data(init_rdf);

    plot_W_Q2_rec_from4v(init_rdf, OUTPUT_FOLDER);



    auto end = std::chrono::high_resolution_clock::now(); // END

    std::chrono::duration<double> elapsed = end - start;
    std::cout << "Time of execution: " << elapsed.count() << " sec" << std::endl;

    return 0;
}
