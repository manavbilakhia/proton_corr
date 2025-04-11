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

void delta_P_VS_P_rec_FD_sectors_1D(ROOT::RDF::RNode rdf, const std::string& output_folder) {
    // Filter for Forward Detector (FD)
    auto rdf_filtered = rdf.Filter("detector == \"FD\" && Theta_rec >= 28");

    // Create 3D histogram: X = p_proton_rec, Y = delta_p, Z = sector_proton
    auto hist3D = rdf_filtered.Histo3D(
        ROOT::RDF::TH3DModel("delta_P_VS_P_rec_FD_3D",
                             "delta P vs P_rec vs Sector;P_rec (GeV);delta P (GeV);Sector",
                             100, 0, 2.5,   // X-axis: P_rec
                             100, -0.1, 0.1, // Y-axis: delta P
                             6, 0, 7),       // Z-axis: sector (1-6)
        "p_proton_rec", "delta_p", "sector_proton"
    );

    std::vector<double> momentum_bins = {0.25,0.5, 0.75, 1.0, 1.25, 1.5, 1.75, 2.0,2.25};
    const size_t num_bins = momentum_bins.size() - 1;

    // Store TGraphErrors for each sector
    std::vector<TGraphErrors*> sector_graphs(6, nullptr);
    for (int i = 0; i < 6; ++i) {
        sector_graphs[i] = new TGraphErrors();
        sector_graphs[i]->SetName(Form("gSector%d", i + 1));
        sector_graphs[i]->SetTitle(Form("Sector %d;Momentum Bin Center (GeV);Mean Δp (GeV)", i + 1));
    }

    // Loop over sectors
    for (int sector = 1; sector <= 6; ++sector) {
        TCanvas* c = new TCanvas(Form("sector_canvas_%d", sector),
                                 Form("delta_p slices in Sector %d", sector),
                                 1200, 800);
        c->Divide(3, 3);

        hist3D->GetZaxis()->SetRange(sector, sector);

        for (size_t bin_idx = 0; bin_idx < num_bins; ++bin_idx) {
            double p_low = momentum_bins[bin_idx];
            double p_high = momentum_bins[bin_idx + 1];
            double p_center = 0.5 * (p_low + p_high);

            hist3D->GetXaxis()->SetRangeUser(p_low, p_high);
            TH1* hist1D = hist3D->Project3D("y");

            hist1D->SetName(Form("Theta>28_delta_p_sector%d_bin%zu", sector, bin_idx + 1));
            hist1D->SetTitle(Form("Theta > 28 Sector %d: %.2f - %.2f GeV;delta P (GeV);Counts",
                                  sector, p_low, p_high));

            c->cd(bin_idx + 1);
            hist1D->Draw();

            // Fit Gaussian in range [-0.04, 0.02]
            TF1* fit = new TF1("gaus_fit", "gaus", -0.08, 0.04);
            hist1D->Fit(fit, "RQ");

            // Extract mean and sigma
            double mean = fit->GetParameter(1);
            double sigma = fit->GetParameter(2);

            // Fill into sector graph
            TGraphErrors* graph = sector_graphs[sector - 1];
            graph->SetPoint(bin_idx, p_center, mean);
            graph->SetPointError(bin_idx, 0.0, sigma); // No X error, only Y error
        }

        c->SaveAs((output_folder + Form("HIGH_THETA_delta_p_sector%d_bins.pdf", sector)).c_str());
        delete c;
    }

    // === Summary canvas: 6 sector graphs (black points, red y=0 line) ===
TCanvas* summaryCanvas = new TCanvas("summaryCanvas", "Mean Δp vs Momentum Bin per Sector", 1400, 1000);
summaryCanvas->Divide(3, 2); // 6 pads

for (int i = 0; i < 6; ++i) {
    summaryCanvas->cd(i + 1);

    TGraphErrors* g = sector_graphs[i];
    g->SetMarkerStyle(20);
    g->SetMarkerColor(kBlack);
    g->SetLineColor(kBlack);
    g->Draw("AP");

    gPad->SetGrid();

    // Draw horizontal red line at y = 0
    double xmin = 0.25;
    double xmax = 2.5;
    TLine* zeroLine = new TLine(xmin, 0.0, xmax, 0.0);
    zeroLine->SetLineColor(kRed);
    zeroLine->SetLineStyle(2);
    zeroLine->SetLineWidth(2);
    zeroLine->Draw("SAME");
}

summaryCanvas->SaveAs((output_folder + "HIGH_THETA_mean_delta_p_vs_momentum_bin_by_sector.pdf").c_str());

std::cout << "Saved summary plot with black markers and red y=0 line.\n";
}



void delta_P_VS_P_rec_FD_sectors_theta_bins_1D(ROOT::RDF::RNode rdf, const std::string& output_folder) {
    // Define momentum bins
    std::vector<double> momentum_bins = {0.25, 0.5, 0.75, 1.0, 1.25, 1.5, 1.75, 2.0, 2.25, 2.5};
    const size_t num_bins = momentum_bins.size() - 1;

    // Define theta bin filters
   
    auto rdf_theta_less_20 = rdf.Filter("detector == \"FD\" && Theta_rec < 20");
    auto rdf_theta_20_30 = rdf.Filter("detector == \"FD\" && Theta_rec >= 20 && Theta_rec < 30");
    auto rdf_theta_30_and_more = rdf.Filter("detector == \"FD\" && Theta_rec >= 30");

    auto rdf_theta_29_and_more = rdf.Filter("detector == \"FD\" && Theta_rec >= 29");
    auto rdf_theta_less_29 = rdf.Filter("detector == \"FD\" && Theta_rec <= 29");

    // Helper lambda to process a single (theta-filtered) RDF
    auto process_theta_bin = [&](ROOT::RDF::RNode filtered_rdf, const std::string& theta_label) {
        // Create 3D histogram for sector, p, delta_p
        auto hist3D = filtered_rdf.Histo3D(
            ROOT::RDF::TH3DModel(
                ("delta_P_VS_P_rec_FD_3D_" + theta_label).c_str(),
                ("delta P vs P_rec vs Sector (" + theta_label + ");P_rec (GeV);delta P (GeV);Sector").c_str(),
                100, 0.0, 2.5,
                100, -0.1, 0.1,
                6, 0, 7
            ),
            "p_proton_rec", "delta_p", "sector_proton"
        );

        for (int sector = 1; sector <= 6; ++sector) {
            TCanvas* c = new TCanvas(Form("canvas_theta_%s_sector_%d", theta_label.c_str(), sector),
                                     Form("Theta %s, Sector %d", theta_label.c_str(), sector),
                                     1200, 800);
            c->Divide(3, 3);

            hist3D->GetZaxis()->SetRange(sector, sector);

            for (size_t bin_idx = 0; bin_idx < num_bins; ++bin_idx) {
                double p_low = momentum_bins[bin_idx];
                double p_high = momentum_bins[bin_idx + 1];

                hist3D->GetXaxis()->SetRangeUser(p_low, p_high);

                TH1* hist1D = hist3D->Project3D("y");
                hist1D->SetName(Form("delta_p_%s_sector%d_bin%zu", theta_label.c_str(), sector, bin_idx + 1));
                hist1D->SetTitle(Form("Theta %s | Sector %d | %.2f - %.2f GeV;delta P (GeV);Counts",
                                      theta_label.c_str(), sector, p_low, p_high));

                c->cd(bin_idx + 1);
                hist1D->Draw();
            }

            c->SaveAs((output_folder + Form("delta_p_%s_sector%d_bins.pdf", theta_label.c_str(), sector)).c_str());
            delete c;
        }
    };

    // Process each theta bin individually (no loops over theta ranges)
 
    process_theta_bin(rdf_theta_less_29, "theta<29");
    process_theta_bin(rdf_theta_29_and_more, "theta>29");
    

    std::cout << "Saved Δp plots for 3 theta bins × 6 sectors = 18 plots (PDF).\n";
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

        // Gaussian fit in range [-0.04, 0.02]
        TF1* fit = new TF1("gaus_fit", "gaus", -0.1, 0.1);
        hist1D->Fit(fit, "RQ");

        double mean = fit->GetParameter(1);
        double sigma = fit->GetParameter(2);

        gCD->SetPoint(bin_idx, p_center, mean);
        gCD->SetPointError(bin_idx, 0.0, sigma);
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



void delta_P_VS_P_rec_FD_sectors_no_loop(ROOT::RDF::RNode rdf, const std::string& output_folder){
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

    canvas.SaveAs((output_folder + "delta_P_VS_P_rec_FD_sectors_no_loop.pdf").c_str());
    std::cout << "Saved 2D histograms as delta_P_VS_P_rec_FD_sectors_no_loop.pdf" << std::endl;


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