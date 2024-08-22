// birthdays_analysis.C
#include <TFile.h>
#include <TH1D.h>
#include <TGraph.h>
#include <TCanvas.h>
#include <TLine.h>
#include <TMath.h>
#include <TLegend.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>

void birthdays_analysis() {
    // Parameters
    const int total_days = 366; // Including Feb 29
    const double total_births = 480040.0;
    const double mean_non_leap = 1314.28;
    const double var_non_leap = 1313.41;
    const double mean_leap = 328.57;
    const double var_leap = 329.03;

    // Data vectors
    std::vector<double> counts(total_days, 0);
    std::vector<double> days(total_days, 0);
    std::vector<double> z_scores(total_days, 0);
    std::vector<double> z_squared(total_days, 0);
    std::vector<bool> significant(total_days, false);

    // Read data
    std::ifstream infile("../../data/birthdays.dat");
    if (!infile.is_open()) {
        std::cerr << "Errore nell'apertura del file!" << std::endl;
        return;
    }
    
    std::string date;
    double count;
    while (infile >> date >> count) {
        int month = std::stoi(date.substr(0, 2));
        int day = std::stoi(date.substr(2, 2));
        int day_of_year = (month - 1) * 31 + day; // Approximate day of year (not precise)
        if (day_of_year >= total_days) day_of_year = total_days - 1; // Handle edge case
        counts[day_of_year] += count;
    }
    infile.close();
    
    // Calculate statistics
    double total_count = 0;
    for (double c : counts) total_count += c;
    
    double expected_births_per_day_non_leap = mean_non_leap / total_days;
    double expected_births_per_day_leap = mean_leap / total_days;
    double sigma_non_leap = TMath::Sqrt(var_non_leap / total_days);
    double sigma_leap = TMath::Sqrt(var_leap / total_days);

    for (int i = 0; i < total_days; ++i) {
        double mean = (i == 59) ? mean_leap : mean_non_leap; // Feb 29th is the 60th day
        double sigma = (i == 59) ? sigma_leap : sigma_non_leap;
        double expected_count = expected_births_per_day_non_leap;
        
        if (i == 59) { // Feb 29
            expected_count = expected_births_per_day_leap;
        }
        
        double observed_count = counts[i];
        double z_score = (observed_count - expected_count) / sigma;
        double z_squared_val = z_score * z_score;
        
        z_scores[i] = z_score;
        z_squared[i] = z_squared_val;
        significant[i] = (TMath::ChisquareQuantile(0.999, total_days - 1) < z_squared_val);
    }
    
    // Chi-square statistics
    double chi_square = 0;
    for (double zs : z_squared) chi_square += zs;
    
    // Print results
    std::cout << "Chi-square statistic: " << chi_square << std::endl;
    std::cout << "Critical value at 99.9%: " << TMath::ChisquareQuantile(0.999, total_days - 1) << std::endl;
    
    // Create Canvas
    TCanvas *c1 = new TCanvas("c1", "Distribuzione e Grafico", 1800, 600);
    c1->Divide(3, 1);
    
    // Histogram of Z-scores
    c1->cd(1);
    TH1D *h_zscore = new TH1D("h_zscore", "Distribuzione del Z-Score", 20, -4, 4);
    for (double z : z_scores) h_zscore->Fill(z);
    
    TH1D *h_norm = new TH1D("h_norm", "Normale Standard", 100, -4, 4);
    h_norm->FillRandom("gaus", 10000);
    
    h_zscore->Draw();
    h_norm->SetLineColor(kRed);
    h_norm->Draw("SAME");
    
    // Plot by Day
    c1->cd(2);
    TGraph *g1 = new TGraph(total_days, &days[0], &counts[0]);
    g1->SetMarkerStyle(21);
    g1->SetMarkerColor(kBlue);
    g1->Draw("AP");
    
    TLine *line = new TLine(0, expected_births_per_day_non_leap, total_days, expected_births_per_day_non_leap);
    line->SetLineColor(kGreen);
    line->SetLineStyle(2);
    line->Draw("same");
    
    // Plot by Month
    c1->cd(3);
    TH1D *h_month = new TH1D("h_month", "Distribuzione delle Nascite per Mese", 12, 0, 12);
    for (int i = 0; i < total_days; ++i) {
        int month = (i / 31) + 1; // Approximate month
        h_month->Fill(month, counts[i]);
    }
    
    h_month->Draw();
    
    // Save Canvas
    c1->SaveAs("birthdays_analysis.png");
}
