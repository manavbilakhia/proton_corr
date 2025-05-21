#include "ROOT/RDataFrame.hxx"
#include "TH3D.h"
#include "TH1.h"
#include "TF1.h"
#include "TLine.h"
#include "TCanvas.h"
#include "TGraphErrors.h"
#include "TLegend.h"
#include "TStyle.h"
#include "TString.h"
#include "TFile.h"
#include "THnSparse.h"  // Needed for THnSparseD
#include "TArrayD.h"
#include <string>

void plot_delta_P(ROOT::RDF::RNode rdf,const std::string& output_folder) {
    TCanvas canvas("c1", "delta_P", 800, 600);
    auto hist = rdf.Histo1D(ROOT::RDF::TH1DModel("delta_P", "delta_P (rec - gen); delta P (GeV); Events", 100, -0.5, 0.5), "delta_p");
    hist->Draw();
    canvas.SaveAs((output_folder + "delta_P.png").c_str());
    std::cout << "Saved 1D histogram as delta_P.png" << std::endl;
}

void plot_delta_P_above_below_1GeV(ROOT::RDF::RNode rdf, const std::string& output_folder) {
    auto rdf_above = rdf.Filter("p_proton_gen > 1");
    auto rdf_below = rdf.Filter("p_proton_gen < 1");
    TCanvas canvas("c1", "delta_P", 800, 600);
    canvas.Divide(1,2);
    canvas.cd(1);
    auto hist_above = rdf_above.Histo1D(ROOT::RDF::TH1DModel("delta_P_above_1GeV", "delta_P (rec - gen) for protons > 1 GeV; delta P (GeV); Events", 200, -0.5, 0.5), "delta_p");
    hist_above->Draw();
    canvas.cd(2);   
    auto hist_below = rdf_below.Histo1D(ROOT::RDF::TH1DModel("delta_P_below_1GeV", "delta_P (rec - gen) for protons < 1 GeV; delta P (GeV); Events", 200, -0.5, 0.5), "delta_p");
    hist_below->Draw();
    canvas.SaveAs((output_folder + "delta_P_above_below_1GeV.png").c_str());
    std::cout << "Saved 1D histogram as delta_P.png" << std::endl;
}

void plot_momenta_components(ROOT::RDF::RNode rdf, const std::string& output_folder) { // do not use loops, the graphs are too different for slicing and loopiong will lead to lazy eval
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

    canvas.SaveAs((output_folder + "momenta_components.png").c_str());
    std::cout << "Saved 1D histogram as momenta_components.png" << std::endl;
}


void plot_delta_P_VS_P_rec(ROOT::RDF::RNode rdf, const std::string& output_folder) {
    TCanvas canvas("c5", "delta P VS P_rec", 800, 600);
    auto hist2D = rdf.Histo2D(ROOT::RDF::TH2DModel("delta_P_VS_P_rec", "delta P vs P_rec;  P_rec (GeV); delta P (GeV)", 200, 0, 5, 200, -0.1, 0.1), "p_proton_rec", "delta_p");
    hist2D->Draw("COLZ");
    canvas.SaveAs((output_folder + "delta_P_VS_P_rec.png").c_str());
    std::cout << "Saved 2D histogram as delta_P_VS_P_rec.png" << std::endl;
}

void plot_delta_P_VS_P_rec_above_below_1GeV(ROOT::RDF::RNode rdf, const std::string& output_folder) {
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
    canvas.SaveAs((output_folder + "delta_P_VS_P_rec_above_below_1GeV.png").c_str());
    std::cout << "Saved 1D histogram as delta_P_VS_P_rec_above_below_1GeV.png" << std::endl;
}

void plot_delta_P_VS_P_rec_FD_Theta_below_above(ROOT::RDF::RNode rdf, const std::string& output_folder){
    auto rdf_above = rdf.Filter("(Theta_rec > 28) && detector == \"FD\" ");
    auto rdf_below = rdf.Filter("(Theta_rec < 28) && detector == \"FD\" ");
    TCanvas canvas("c1", "delta_P", 800, 600);
    canvas.Divide(1,2);
    canvas.cd(1);
    auto hist2D_above = rdf_above.Histo2D(ROOT::RDF::TH2DModel("delta_P_VS_P_rec_above_Theta", "delta P vs P_rec for theta > 28 deg;  P_rec (GeV); delta P (GeV)", 100, 0, 2.5, 100, -0.1, 0.1), "p_proton_rec", "delta_p");
    hist2D_above->Draw("COLZ");
    canvas.cd(2);
    auto hist2D_below = rdf_below.Histo2D(ROOT::RDF::TH2DModel("delta_P_VS_P_rec_below_Theta", "delta P vs P_rec for theta < 28 deg;  P_rec (GeV); delta P (GeV)", 100, 0, 2.5, 100, -0.1, 0.1), "p_proton_rec", "delta_p");
    hist2D_below->Draw("COLZ");
    canvas.SaveAs((output_folder + "delta_P_VS_P_rec_FD_Theta_below_above_28deg.png").c_str());
    std::cout << "Saved 1D histogram as delta_P_VS_P_rec_FD_Theta_below_above_28deg.png" << std::endl;
}

void plot_P_rec_P_gen(ROOT::RDF::RNode rdf, const std::string& output_folder) {
    TCanvas canvas("c6", "P_rec VS P_gen", 800, 600);
    canvas.Divide(1,2);
    canvas.cd(1);
    auto hist1 = rdf.Histo1D(ROOT::RDF::TH1DModel("p_proton_gen", "p_proton_gen; p_proton_gen (GeV); Events", 100, 0, 2.5), "p_proton_gen");
    hist1->Draw();
    canvas.cd(2);
    auto hist2 = rdf.Histo1D(ROOT::RDF::TH1DModel("p_proton_rec", "p_proton_rec; p_proton_rec (GeV); Events", 100, 0, 2.5), "p_proton_rec");
    hist2->Draw();

    canvas.SaveAs((output_folder + "P_rec_P_gen.png").c_str());
    std::cout << "Saved 1D histogram as P_rec_P_gen.png" << std::endl;
}


void Theta_VS_momentum(ROOT::RDF::RNode rdf, const std::string& output_folder) {
    TCanvas canvas("c7", "Theta VS momentum", 800, 600);
    canvas.Divide(1,2);
    canvas.cd(1);
    auto hist1 = rdf.Histo2D(ROOT::RDF::TH2DModel("Theta_gen_VS_P_gen", "Theta_gen VS P_gen; P_gen (GeV); Theta_gen (deg);", 100, 0, 7, 100, 0, 100), "p_proton_gen", "Theta_gen" );
    hist1->Draw("COLZ");
    canvas.cd(2);
    auto hist2 = rdf.Histo2D(ROOT::RDF::TH2DModel("Theta_rec_VS_P_rec", "Theta_rec VS P_rec; P_rec (GeV); Theta_rec (deg)", 100, 0, 7, 100, 0, 100), "p_proton_rec", "Theta_rec");
    hist2->Draw("COLZ");

    canvas.SaveAs((output_folder + "Theta_VS_momentum.png").c_str());
    std::cout << "Saved 2D histogram as Theta_VS_momentum.png" << std::endl;
}

void Theta_VS_momentum_FD_CD(ROOT::RDF::RNode rdf, const std::string& output_folder) {
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

    canvas.SaveAs((output_folder + "Theta_VS_momentum_FD_CD.png").c_str());
    std::cout << "Saved 2D histogram as Theta_VS_momentum_FD_CD.png" << std::endl;
}

void Phi_VS_momentum(ROOT::RDF::RNode rdf, const std::string& output_folder) {
    TCanvas canvas("c8", "Phi VS momentum", 800, 600);
    canvas.Divide(1,2);
    canvas.cd(1);
    auto hist1 = rdf.Histo2D(ROOT::RDF::TH2DModel("Phi_gen_VS_P_gen", "Phi_gen VS P_gen; Phi_gen (deg); P_gen (GeV)", 100, -200, 200, 100, 0, 5), "Phi_gen", "p_proton_gen");
    hist1->Draw("COLZ");
    canvas.cd(2);
    auto hist2 = rdf.Histo2D(ROOT::RDF::TH2DModel("Phi_rec_VS_P_rec", "Phi_rec VS P_rec; Phi_rec (deg); P_rec (GeV)", 100, -200, 200, 100, 0, 5), "Phi_rec",  "p_proton_rec");
    hist2->Draw("COLZ");

    canvas.SaveAs((output_folder + "Phi_VS_momentum.png").c_str());
    std::cout << "Saved 2D histogram as Phi_VS_momentum.png" << std::endl;
}

void Phi_VS_momentum_FD_CD(ROOT::RDF::RNode rdf, const std::string& output_folder) {
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

    canvas.SaveAs((output_folder + "Phi_VS_momentum_FD_CD.png").c_str());
    std::cout << "Saved 2D histogram as Phi_VS_momentum_FD_CD.png" << std::endl;
}

void Phi_VS_Theta(ROOT::RDF::RNode rdf, const std::string& output_folder) {
    TCanvas canvas("c9", "Phi VS Theta", 800, 600);
    canvas.Divide(1,2);
    canvas.cd(1);
    auto hist1 = rdf.Histo2D(ROOT::RDF::TH2DModel("Phi_gen_VS_Theta_gen", "Phi_gen VS Theta_gen; Phi_gen (deg); Theta_gen (deg)", 100, -200, 200, 100, 0, 100), "Phi_gen", "Theta_gen");
    hist1->Draw("COLZ");
    canvas.cd(2);
    auto hist2 = rdf.Histo2D(ROOT::RDF::TH2DModel("Phi_rec_VS_Theta_rec", "Phi_rec VS Theta_rec; Phi_rec (deg) ;Theta_rec (deg)",  100, -200, 200, 100, 0, 100), "Phi_rec", "Theta_rec");
    hist2->Draw("COLZ");

    canvas.SaveAs((output_folder + "Phi_VS_Theta.png").c_str());
    std::cout << "Saved 2D histogram as Phi_VS_Theta.png" << std::endl;
}

void Phi_VS_Theta_FD_CD(ROOT::RDF::RNode rdf, const std::string& output_folder) {
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

    canvas.SaveAs((output_folder + "Phi_VS_Theta_FD_CD.png").c_str());
    std::cout << "Saved 2D histogram as Phi_VS_Theta_FD_CD.png" << std::endl;
}

void delta_P_VS_P_rec_FD_CD(ROOT::RDF::RNode rdf, const std::string& output_folder) {
    TCanvas canvas("c", "delta_P_VS_P_rec_FD_CD", 800, 600);
    canvas.Divide(1,2);
    canvas.cd(1);
    auto hist2D_1 = rdf.Filter("detector == \"FD\"").Histo2D(ROOT::RDF::TH2DModel("delta_P_VS_P_rec_FD", "delta P vs P_rec in FD;  P_rec (GeV); delta P (GeV)", 100, 0, 2.5, 100, -0.1, 0.1), "p_proton_rec", "delta_p");
    hist2D_1->Draw("COLZ");
    canvas.cd(2);
    auto hist2D_2 = rdf.Filter("detector == \"CD\"").Histo2D(ROOT::RDF::TH2DModel("delta_P_VS_P_rec_CD", "delta P vs P_rec in CD;  P_rec (GeV); delta P (GeV)", 100, 0, 2.5, 100, -0.1, 0.1), "p_proton_rec", "delta_p");
    hist2D_2->Draw("COLZ");

    canvas.SaveAs((output_folder + "delta_P_VS_P_rec_FD_CD.png").c_str());
    std::cout << "Saved 2D histogram as delta_P_VS_P_rec_FD_CD.png" << std::endl;
}



void delta_P_VS_P_rec_FD_sectors_2D(ROOT::RDF::RNode rdf, const std::string& output_folder) {
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
    canvas.SaveAs((output_folder + "delta_P_VS_P_rec_FD_sectors.pdf").c_str());
    std::cout << "Saved 2D histograms from 3D histogram as delta_P_VS_P_rec_FD_sectors.png" << std::endl;
}

void delta_P_VS_P_rec_FD_sectors_1D_theta_sliced(ROOT::RDF::RNode rdf, const std::string& output_folder, const bool normalized) {
    std::string dp_Or_dpp = normalized ? "delta_p_norm" : "delta_p";

    // Define theta bin edges
    std::vector<std::pair<double, double>> theta_bins = {
       {0,25} ,{25,35}, {28,35}
    //{0, 26.5} , {26.5, 29}, {26.5, 31}, {26.5, 33}, {26.5, 35}
};

    // Momentum bin edges
    std::vector<double> momentum_bins = {0.4, 0.43, 0.45, 0.47,0.5,
                                         0.6, 0.75, 1.0, 1.25, 1.5, 1.75, 2.0, 2.25};
    const size_t num_bins = momentum_bins.size() - 1;

    for (size_t theta_idx = 0; theta_idx < theta_bins.size(); ++theta_idx) {
        double theta_min = theta_bins[theta_idx].first;
        double theta_max = theta_bins[theta_idx].second;
        std::string theta_label = Form("theta_%.0f_%.0f", theta_min, theta_max);

        // Filter by theta and detector
        ROOT::RDF::RNode rdf_filtered = rdf.Filter(Form("detector == \"FD\" && Theta_rec >= %.2f && Theta_rec < %.2f", theta_min, theta_max));

        auto hist3D = normalized
            ? rdf_filtered.Histo3D(
                  ROOT::RDF::TH3DModel("delta_p/p_VS_P_rec_FD_3D",
                                       "delta_p/p vs P_rec vs Sector;P_rec (GeV);delta_p/p;Sector",
                                       100, 0, 2.5, 100, -0.2, 0.1, 6, 0, 7),
                  "p_proton_rec", "dp_norm", "sector_proton")
            : rdf_filtered.Histo3D(
                  ROOT::RDF::TH3DModel("delta_P_VS_P_rec_FD_3D",
                                       "delta P vs P_rec vs Sector;P_rec (GeV/c);delta P (GeV/c);Sector",
                                       100, 0, 2.5, 100, -0.1, 0.1, 6, 0, 7),
                  "p_proton_rec", "delta_p", "sector_proton");

        std::vector<TGraphErrors*> sector_graphs(6, nullptr);
        for (int i = 0; i < 6; ++i) {
            sector_graphs[i] = new TGraphErrors();
            sector_graphs[i]->SetName(Form("gSector%d_%s", i + 1, theta_label.c_str()));
            sector_graphs[i]->SetTitle(Form("Sector %d (%s);Momentum Bin Center (GeV/c);Mean %s (GeV/c)", i + 1, theta_label.c_str(), dp_Or_dpp.c_str()));
        }

        for (int sector = 1; sector <= 6; ++sector) {
            TCanvas* c = new TCanvas(Form("sector_canvas_%d_%s", sector, theta_label.c_str()),
                                     Form("%s slices in Sector %d, Theta [%.0f, %.0f]", dp_Or_dpp.c_str(), sector, theta_min, theta_max),
                                     1200, 800);
            c->Divide(5, 5);

            hist3D->GetZaxis()->SetRange(sector, sector);

            for (size_t bin_idx = 0; bin_idx < num_bins; ++bin_idx) {
                double p_low = momentum_bins[bin_idx];
                double p_high = momentum_bins[bin_idx + 1];
                double p_center = 0.5 * (p_low + p_high);

                hist3D->GetXaxis()->SetRangeUser(p_low, p_high);
                TH1* hist1D = hist3D->Project3D("y");

                hist1D->SetName(Form("%s_%s_sector%d_bin%zu", theta_label.c_str(), dp_Or_dpp.c_str(), sector, bin_idx + 1));
                hist1D->SetTitle(Form("Theta [%.0f,%.0f] Sector %d: %.2f–%.2f GeV;%s (GeV/c);Counts",
                                      theta_min, theta_max, sector, p_low, p_high, dp_Or_dpp.c_str()));

                c->cd(bin_idx + 1);
                hist1D->Draw();

                TF1* fit_init = new TF1("gaus_init", "gaus", -0.2, 0.01);
                hist1D->Fit(fit_init, "RQ0");

                double mean_init = fit_init->GetParameter(1);
                double sigma_init = fit_init->GetParameter(2);

                double fit_min = mean_init - sigma_init;
                double fit_max = mean_init + sigma_init;
                TF1* fit_refined = new TF1("gaus_refined", "gaus", fit_min, fit_max);
                hist1D->Fit(fit_refined, "RQ");

                double mean = fit_refined->GetParameter(1);
                double sigma = fit_refined->GetParameter(2);

                TGraphErrors* graph = sector_graphs[sector - 1];
                graph->SetPoint(bin_idx, p_center, mean);
                graph->SetPointError(bin_idx, 0.0, sigma);
            }

            c->SaveAs((output_folder + Form("%s_%s_sector%d_bins.pdf", theta_label.c_str(), dp_Or_dpp.c_str(), sector)).c_str());
            delete c;
        }

        // Summary canvas
        TCanvas* summaryCanvas = new TCanvas(Form("summaryCanvas_%s", theta_label.c_str()),
                                             Form("Mean %s vs Momentum Bin per Sector (%s)", dp_Or_dpp.c_str(), theta_label.c_str()), 1400, 1000);
        summaryCanvas->Divide(3, 2);

        for (int i = 0; i < 6; ++i) {
            summaryCanvas->cd(i + 1);
            TGraphErrors* g = sector_graphs[i];
            g->SetMarkerStyle(20);
            g->SetMarkerColor(kBlack);
            g->SetLineColor(kBlack);
            g->Draw("AP");

            gPad->SetGrid();

            double xmin = 0.25;
            double xmax = 2.5;
            TLine* zeroLine = new TLine(xmin, 0.0, xmax, 0.0);
            zeroLine->SetLineColor(kRed);
            zeroLine->SetLineStyle(2);
            zeroLine->SetLineWidth(2);
            zeroLine->Draw("SAME");
        }

        summaryCanvas->SaveAs((output_folder + theta_label + "_mean_" + dp_Or_dpp + "_vs_momentum_bin_by_sector.pdf").c_str());
        std::cout << "Saved summary plot for theta bin [" << theta_min << ", " << theta_max << ")\n";
    }
}


void delta_P_VS_P_rec_FD_sectors_1D(ROOT::RDF::RNode rdf, const std::string& output_folder, const std::string& thetaBin, const bool normalized) {
    std::string dp_Or_dpp = normalized ? "delta_p_norm" : "delta_p";
    ROOT::RDF::RNode rdf_filtered = rdf;

    if (thetaBin == "high") {
        rdf_filtered = rdf.Filter("detector == \"FD\" && Theta_rec >= 28");
    } else if (thetaBin == "low") {
        rdf_filtered = rdf.Filter("detector == \"FD\" && Theta_rec < 28");
    }

    auto hist3D = normalized
        ? rdf_filtered.Histo3D(
            ROOT::RDF::TH3DModel("delta_p/p_VS_P_rec_FD_3D",
                                 "delta_p/p vs P_rec vs Sector;P_rec (GeV);delta_p/p;Sector",
                                 100, 0, 2.5,
                                 100, -0.2, 0.1,
                                 6, 0, 7),
            "p_proton_rec", "dp_norm", "sector_proton")
        : rdf_filtered.Histo3D(
            ROOT::RDF::TH3DModel("delta_P_VS_P_rec_FD_3D",
                                 "delta P vs P_rec vs Sector;P_rec (GeV/c);delta P (GeV/c);Sector",
                                 100, 0, 2.5,
                                 100, -0.1, 0.1,
                                 6, 0, 7),
            "p_proton_rec", "delta_p", "sector_proton");

    std::vector<double> momentum_bins = {0.4, 0.5, 0.6, 0.75, 1.0, 1.25, 1.5, 1.75, 2.0, 2.25}; // new bins
    const size_t num_bins = momentum_bins.size() - 1;

    std::vector<TGraphErrors*> sector_graphs(6, nullptr);
    for (int i = 0; i < 6; ++i) {
        sector_graphs[i] = new TGraphErrors();
        sector_graphs[i]->SetName(Form("gSector%d", i + 1));
        sector_graphs[i]->SetTitle(Form("Sector %d;Momentum Bin Center (GeV/c);Mean %s (GeV/c)", i + 1, dp_Or_dpp.c_str()));
    }

    for (int sector = 1; sector <= 6; ++sector) {
        TCanvas* c = new TCanvas(Form("sector_canvas_%d", sector),
                                 Form("%s slices in Sector %d", dp_Or_dpp.c_str(), sector),
                                 1200, 800);
        c->Divide(3,3);

        hist3D->GetZaxis()->SetRange(sector, sector);

        for (size_t bin_idx = 0; bin_idx < num_bins; ++bin_idx) {
            double p_low = momentum_bins[bin_idx];
            double p_high = momentum_bins[bin_idx + 1];
            double p_center = 0.5 * (p_low + p_high);

            hist3D->GetXaxis()->SetRangeUser(p_low, p_high);
            TH1* hist1D = hist3D->Project3D("y");

            if (thetaBin == "high") {
                hist1D->SetName(Form("Theta>28_%s_sector%d_bin%zu", dp_Or_dpp.c_str(), sector, bin_idx + 1));
                hist1D->SetTitle(Form("Theta > 28 Sector %d: %.2f - %.2f GeV;%s (GeV/c);Counts",
                                      sector, p_low, p_high, dp_Or_dpp.c_str()));
            } else {
                hist1D->SetName(Form("Theta<28_%s_sector%d_bin%zu", dp_Or_dpp.c_str(), sector, bin_idx + 1));
                hist1D->SetTitle(Form("Theta < 28 Sector %d: %.2f - %.2f GeV;%s (GeV/c);Counts",
                                      sector, p_low, p_high, dp_Or_dpp.c_str()));
            }

            c->cd(bin_idx + 1);
            hist1D->Draw();

            TF1* fit_init = new TF1("gaus_init", "gaus", -0.2, 0.01);
            hist1D->Fit(fit_init, "RQ0");
            
            double mean_init = fit_init->GetParameter(1);
            double sigma_init = fit_init->GetParameter(2);

            double fit_min = mean_init - sigma_init;
            double fit_max = mean_init + sigma_init;
            TF1* fit_refined = new TF1("gaus_refined", "gaus", fit_min, fit_max);
            hist1D->Fit(fit_refined, "RQ");

            double mean = fit_refined->GetParameter(1);
            double sigma = fit_refined->GetParameter(2);

            TGraphErrors* graph = sector_graphs[sector - 1];
            graph->SetPoint(bin_idx, p_center, mean);
            graph->SetPointError(bin_idx, 0.0, sigma);
        }

        c->SaveAs((output_folder + thetaBin + Form("_theta_%s_sector%d_bins.pdf", dp_Or_dpp.c_str(), sector)).c_str());
        delete c;
    }

    TCanvas* summaryCanvas = new TCanvas("summaryCanvas", Form("Mean %s vs Momentum Bin per Sector", dp_Or_dpp.c_str()), 1400, 1000);
    summaryCanvas->Divide(3, 2);

    for (int i = 0; i < 6; ++i) {
        summaryCanvas->cd(i + 1);
        TGraphErrors* g = sector_graphs[i];
        g->SetMarkerStyle(20);
        g->SetMarkerColor(kBlack);
        g->SetLineColor(kBlack);
        g->Draw("AP");

        gPad->SetGrid();

        double xmin = 0.25;
        double xmax = 2.5;
        TLine* zeroLine = new TLine(xmin, 0.0, xmax, 0.0);
        zeroLine->SetLineColor(kRed);
        zeroLine->SetLineStyle(2);
        zeroLine->SetLineWidth(2);
        zeroLine->Draw("SAME");
    }

    summaryCanvas->SaveAs((output_folder + thetaBin + "_theta_mean_" + dp_Or_dpp + "_vs_momentum_bin_by_sector.pdf").c_str());

    std::cout << "Saved summary plot with black markers and red y=0 line.\n";
}

void plot_theta_slices_2D(ROOT::RDF::RNode rdf, const std::string& output_folder) {
    auto rdf_theta = rdf.Filter("detector == \"FD\" && Theta_rec >= 28 && Theta_rec < 30");

    std::vector<std::pair<double, double>> p_bins = {
        {0.4, 0.5},
        {0.5, 0.6}  // Change to {0.6, 0.7} if needed
    };

    for (const auto& [p_min, p_max] : p_bins) {
        std::string label = Form("p%.2f_%.2f", p_min, p_max);
        auto rdf_p = rdf_theta.Filter(Form("p_proton_rec >= %.3f && p_proton_rec <= %.3f", p_min, p_max));

        auto h_dp_vs_p = rdf_p.Histo2D(
            {"h_dp_vs_p", Form("delta_p vs p_rec [Theta 28–30, %s];p_rec (GeV/c);delta_p (GeV/c)", label.c_str()),
             100, 0, 2.5, 100, -0.1, 0.1},
            "p_proton_rec", "delta_p");

        auto h_theta_vs_p = rdf_p.Histo2D(
            {"h_theta_vs_p", Form("Theta_rec vs p_rec [Theta 28–30, %s];p_rec (GeV/c);Theta_rec (deg)", label.c_str()),
             100, 0, 2.5, 100, 0, 60},
            "p_proton_rec", "Theta_rec");

        auto h_theta_vs_dpnorm = rdf_p.Histo2D(
            {"h_theta_vs_dpnorm", Form("Theta_rec vs delta_p/p [Theta 28–30, %s];delta_p/p;Theta_rec (deg)", label.c_str()),
             100, -0.2, 0.1, 100, 0, 60},
            "dp_norm", "Theta_rec");

        TCanvas* c1 = new TCanvas(Form("canvas_dp_vs_p_%s", label.c_str()), "delta_p vs p", 800, 600);
        h_dp_vs_p->Draw("COLZ");
        c1->SaveAs((output_folder + "delta_p_vs_p_" + label + ".pdf").c_str());
        delete c1;

        TCanvas* c2 = new TCanvas(Form("canvas_theta_vs_p_%s", label.c_str()), "Theta vs p", 800, 600);
        h_theta_vs_p->Draw("COLZ");
        c2->SaveAs((output_folder + "theta_vs_p_" + label + ".pdf").c_str());
        delete c2;

        TCanvas* c3 = new TCanvas(Form("canvas_theta_vs_dpnorm_%s", label.c_str()), "Theta vs delta_p/p", 800, 600);
        h_theta_vs_dpnorm->Draw("COLZ");
        c3->SaveAs((output_folder + "theta_vs_dpnorm_" + label + ".pdf").c_str());
        delete c3;
    }

    std::cout << "Saved 2D plots for Theta_rec ∈ [28, 30) and selected p_rec bins.\n";
}



void delta_P_VS_P_rec_CD_1D(ROOT::RDF::RNode rdf, const std::string& output_folder) {
    // Filter for Central Detector (CD)
    auto rdf_filtered = rdf.Filter("detector == \"CD\"");

    // Create 2D histogram: X = p_proton_rec, Y = delta_p
    auto hist2D = rdf_filtered.Histo2D(
        ROOT::RDF::TH2DModel("delta_P_VS_P_rec_CD_2D",
                             "delta P vs P_rec [CD];P_rec (GeV);delta P (GeV)",
                             100, 0, 2.5,
                             100, -0.1, 0.1),
        "p_proton_rec", "delta_p"
    );

    // Fine momentum binning from 0.25 to 1.25 in steps of 0.1
    std::vector<double> momentum_bins;
    for (double p = 0.25; p <= 1.25 + 1e-6; p += 0.1) {
        momentum_bins.push_back(p);
    }
    const size_t num_bins = momentum_bins.size() - 1;

    // Create canvas for 1D plots
    size_t nCols = 4;
    size_t nRows = (num_bins + nCols - 1) / nCols;
    TCanvas* c = new TCanvas("cd_canvas", "Central Detector Δp in Momentum Bins", 300 * nCols, 300 * nRows);
    c->Divide(nCols, nRows);

    // Prepare graph for mean vs momentum bin center
    TGraphErrors* gCD = new TGraphErrors();
    gCD->SetName("gCD");
    gCD->SetTitle("Central Detector: Mean Δp vs Momentum Bin;Momentum Bin Center (GeV);Mean Δp (GeV)");

    for (size_t bin_idx = 0; bin_idx < num_bins; ++bin_idx) {
        double p_low = momentum_bins[bin_idx];
        double p_high = momentum_bins[bin_idx + 1];
        double p_center = 0.5 * (p_low + p_high);

        hist2D->GetXaxis()->SetRangeUser(p_low, p_high);

        TH1* hist1D = hist2D->ProjectionY(Form("delta_p_bin%zu", bin_idx + 1));
        hist1D->SetTitle(Form("CD: %.2f - %.2f GeV;delta P (GeV);Counts", p_low, p_high));

        c->cd(bin_idx + 1);
        hist1D->Draw();

        // First rough fit
        TF1* rough_fit = new TF1("rough_fit", "gaus", -0.1, 0.1);
        hist1D->Fit(rough_fit, "RQ0");  // Q0 = quiet and no draw

        double mean = rough_fit->GetParameter(1);
        double sigma = rough_fit->GetParameter(2);

        // Refined fit in [mean - sigma, mean + sigma]
        TF1* refined_fit = new TF1("refined_fit", "gaus", mean - sigma, mean + sigma);
        hist1D->Fit(refined_fit, "RQ");

        double refined_mean = refined_fit->GetParameter(1);
        double refined_sigma = refined_fit->GetParameter(2);

        gCD->SetPoint(bin_idx, p_center, refined_mean);
        gCD->SetPointError(bin_idx, 0.0, refined_sigma);
    }

    c->SaveAs((output_folder + "delta_p_CD_bins_fine.pdf").c_str());
    delete c;

    // === Summary plot ===
    TCanvas* summaryCanvas = new TCanvas("summaryCanvas_CD", "CD Mean Δp vs Momentum Bin (fine bins)", 800, 600);
    summaryCanvas->cd();

    gCD->SetMarkerStyle(20);
    gCD->SetMarkerColor(kBlack);
    gCD->SetLineColor(kBlack);
    gCD->Draw("AP");
    gCD->GetXaxis()->SetLimits(0.2, 1.25);  // <-- Add this line to set X-axis range
    gPad->SetGrid();

    // Red horizontal line at y = 0
    TLine* zeroLine = new TLine(0.25, 0.0, 1.25, 0.0);
    zeroLine->SetLineColor(kRed);
    zeroLine->SetLineStyle(2);
    zeroLine->SetLineWidth(2);
    zeroLine->Draw("SAME");

    summaryCanvas->SaveAs((output_folder + "mean_delta_p_vs_momentum_bin_CD_fine.pdf").c_str());

    std::cout << "Saved fine-binned central detector Δp plots and summary with fit mean/sigma.\n";
}



// -------------------------------------Don't need -------------------------------------//

void gen_theta_VS_rec_theta(ROOT::RDF::RNode rdf, const std::string& output_folder) {
    TCanvas canvas("c11", "gen_theta_VS_rec_theta", 800, 600);
    canvas.Divide(1,2);
    canvas.cd(1);
    auto hist2D = rdf.Histo2D(ROOT::RDF::TH2DModel("gen_theta_VS_rec_theta PROTON", "gen_theta VS rec_theta PROTON; rec_theta (deg); gen_theta (deg)", 100, 0, 100, 100, 0, 100), "Theta_rec", "Theta_gen");
    hist2D->Draw("COLZ");
    canvas.cd(2);
    auto hist2D_e = rdf.Histo2D(ROOT::RDF::TH2DModel("gen_theta_VS_rec_theta ELECTRON", "gen_theta VS rec_theta ELECTRON; rec_theta (deg); gen_theta (deg)", 100, 5, 25, 100, 5, 25), "Theta_electron_rec", "Theta_electron_gen");
    hist2D_e->Draw("COLZ");
    canvas.SaveAs((output_folder + "gen_theta_VS_rec_theta.png").c_str());
    std::cout << "Saved 2D histogram as gen_theta_VS_rec_theta.png" << std::endl;
}

void gen_phi_VS_rec_phi(ROOT::RDF::RNode rdf, const std::string& output_folder) {
    TCanvas canvas("c12", "gen_phi_VS_rec_phi", 800, 600);
    canvas.Divide(1,2);
    canvas.cd(1);
    auto hist2D = rdf.Histo2D(ROOT::RDF::TH2DModel("gen_phi_VS_rec_phi PROTON", "gen_phi VS rec_phi PROTON; rec_phi (deg); gen_phi (deg)", 100, -200, 200, 100, -200, 200), "Phi_rec", "Phi_gen");
    hist2D->Draw("COLZ");
    canvas.cd(2);
    auto hist2D_e = rdf.Histo2D(ROOT::RDF::TH2DModel("gen_phi_VS_rec_phi ELECTRON", "gen_phi VS rec_phi ELECTRON; rec_phi (deg); gen_phi (deg)", 100, -200, 200, 100, -200, 200), "Phi_electron_rec", "Phi_electron_gen");
    hist2D_e->Draw("COLZ");
    canvas.SaveAs((output_folder + "gen_phi_VS_rec_phi.png").c_str());
    std::cout << "Saved 2D histogram as gen_phi_VS_rec_phi.png" << std::endl;
}

void gen_P_VS_rec_P(ROOT::RDF::RNode rdf, const std::string& output_folder) {
    TCanvas canvas("c13", "gen_P_VS_rec_P", 800, 600);
    canvas.Divide(1,2);
    canvas.cd(1);
    auto hist2D = rdf.Histo2D(ROOT::RDF::TH2DModel("gen_P_VS_rec_P PROTON", "gen_P VS rec_P PROTON; rec_P (GeV); gen_P (GeV)", 100, 0, 5, 100, 0, 5), "p_proton_rec", "p_proton_gen");
    hist2D->Draw("COLZ");
    canvas.cd(2);
    auto hist2D_e = rdf.Histo2D(ROOT::RDF::TH2DModel("gen_P_VS_rec_P ELECTRON", "gen_P VS rec_P ELECTRON; rec_P (GeV); gen_P (GeV)", 100, 2, 8, 100, 2, 8), "p_electron_rec", "p_electron_gen");
    hist2D_e->Draw("COLZ");
    canvas.SaveAs((output_folder + "gen_P_VS_rec_P.png").c_str());
    std::cout << "Saved 2D histogram as gen_P_VS_rec_P.png" << std::endl;
}