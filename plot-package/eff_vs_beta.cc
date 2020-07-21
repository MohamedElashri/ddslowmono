/*

  Run over MC monopole tree and generate efficiency distribution

  as a function of beta.

*/

#include "Constants.hh"

#include "Event_Info.hh"

#include "MFRoot.hh"

#include <TFile.h>

#include <TGraphErrors.h>

#include <TH1.h>

#include <TTree.h>

#include <TStyle.h>

#include <cmath>

#include <map>

#include <fstream>

MFRoot::Canvas_Manager ccans;

using MFRoot::get;

TGraphErrors* combined_efficiency(TH1 *low, TH1 *high);

void draw_counts(TH1 *all, TH1 *ddt, TH1 *signal, std::string energy_label);

void draw_efficiency(TH1 *eff, std::string type, std::string energy_label);

TH1* eff_vs_beta(std::string energy_label, std::string file_name);

void make_efficiency_file(TGraphErrors* eff, std::string outfile_name);

TGraph* min_efficiency(TGraphErrors *eff);

void eff_vs_beta()

{

  TH1 *dEdx90   = eff_vs_beta("dEdx90", Constants::MC_RECO_FILE);

  TH1 *low      = eff_vs_beta("LowE", Constants::MC_RECO_FILE);

  TH1 *high     = eff_vs_beta("HighE", Constants::MC_RECO_FILE_2);

  TGraphErrors* eff = combined_efficiency(low, high);

   TGraph* min       = min_efficiency(eff);

  

  // Style all of the histograms/graphs:

  low->SetLineColor(kGreen + 2);

   high->SetLineColor(kRed);  

   dEdx90->SetLineColor(51);

   eff->SetMarkerStyle(kOpenCircle);

   min->SetMarkerColor(kBlue);

   min->SetMarkerStyle(kFullTriangleUp);

   Draw:

   ccans.new_canvas("Signal_Efficiency_vs_Beta_All");
   TLegend *l = new TLegend(0.5, 0.7, 0.88, 0.88);

   l->SetTextSizePixels(20);

   low->Draw();

   high->Draw("SAME");

   eff->Draw("P SAME");
   l->AddEntry(low, "Original High Model", "l");
   l->AddEntry(high, "Proposed High Model", "l");
   l->Draw();

   ccans.new_canvas("Signal_Efficiency_vs_Beta_Final_Only");

   low->Draw("AXIS");

   eff->Draw("P SAME");

   ccans.new_canvas("Minimum_Efficiency_with_Signal_Efficiency");

   low->Draw("AXIS");

   eff->Draw("P SAME");

   min->Draw("P SAME");

   ccans.new_canvas("Minimum_Efficiency");

   low->Draw("AXIS");

   min->Draw("P SAME");

   ccans.new_canvas("LowE_and_dEdx90E");

   low->Draw();

   dEdx90->Draw("SAME");

   std::cout << "HighE Efficiencies:" << std::endl;

   high->Print("all");

   std::cout << "LowE Efficiencies:" << std::endl;

   low->Print("all");

   std::cout << "dEdx90E Efficiencies:" << std::endl;

   dEdx90->Print("all");

  // Print combined efficiency to file:

   make_efficiency_file(eff, "Efficiency.log");

}

TGraphErrors* combined_efficiency(TH1 *low, TH1 *high)

{

  int n_bins = low->GetNbinsX();

  TGraphErrors* eff = new TGraphErrors(n_bins);

  for (int bin = 1; bin <= n_bins; ++bin)

  {

    double log_beta = low->GetBinCenter(bin);

    double beta = std::pow(10, log_beta);

    

    double l = low->GetBinContent(bin);

    double h = high->GetBinContent(bin);

    double mean = (h+l) / 2;

    double error = std::fabs(h - l) / 2;

    int point = bin - 1;

    eff->SetPoint(point, log_beta, mean);

    // eff->SetPointError(point, 0.0, error);
 
  }

  return eff;

}

TH1* eff_vs_beta(std::string energy_label, std::string file_name)

{

  gStyle->SetOptStat("");

  std::cout << "Processing efficiency for " << energy_label << std::endl;

  

  TFile *file = MFRoot::open(file_name);

  TTree *tree = dynamic_cast<TTree*>(file->Get("mono/Event"));

  std::map<std::string, double> t;

  for (auto const& name : Constants::BRANCH_NAMES)

    tree->SetBranchAddress(name.c_str(), &t[name]);

  std::string *input_file_name = NULL;

  tree->SetBranchAddress("input_file_name", &input_file_name);

  MFRoot::hist_t h;

  std::vector<std::string> names =

    { "all", "hit_detector", "ddt", "signal" };

  for (auto const& name : names)

    h[name] = new TH1D(name.c_str(), ";log_{10}#beta;", 21, -4.05, -1.95);

  for (int entry = 0; entry != tree->GetEntries(); ++entry)

  // for (int entry = 0; entry != 100000; ++entry)

  {

    if (entry % 100000 == 0)

      std::cout << "\tprocessing entry " << entry << "..." << std::endl;

    tree->GetEntry(entry);

    Event_Info e(t, *input_file_name);

    double lbeta = std::log10(e.beta_from_file_name());

    double mc_length = e.get("mc_length") / 100;

    if (mc_length > 10)

    // if (e.is_preselected_reco())

    {

      get(h, "all")->Fill(lbeta);

      if (e.hit_detector())

      {

        get(h, "hit_detector")->Fill(lbeta);

    

        if (e.is_triggered())

        {

          get(h, "ddt")->Fill(lbeta);

          if (e.is_linear() &&

              e.is_slow())

          {

            get(h, "signal")->Fill(lbeta);

          }

        }

      }

    }

  }

  h["ddt_eff"] =

    MFRoot::divide_with_poisson_errors

    (get(h, "ddt"), get(h, "all"), "signal_ddt");

  h["signal_eff"] =

    MFRoot::divide_with_poisson_errors

    (get(h, "signal"), get(h, "all"), "signal_eff");

  

  draw_counts(get(h, "all"), get(h, "ddt"), get(h, "signal"), energy_label);

  draw_efficiency(get(h, "ddt_eff"), "DDT", energy_label);

  draw_efficiency(get(h, "signal_eff"), "Signal", energy_label);

  std::cout << "DDT Efficiencies:" << std::endl;

  get(h, "ddt_eff")->Print("all");

  std::cout << "Signal Efficiencies:" << std::endl;

  get(h, "signal_eff")->Print("all");

  

  std::string clone_label = "signal_eff_" + energy_label;

  return dynamic_cast<TH1*>(get(h, "signal_eff")->Clone(clone_label.c_str()));

}

void draw_counts(TH1 *all, TH1 *ddt, TH1 *signal, std::string energy_label)

{

  TAxis *x = all->GetXaxis();

  TAxis *y = all->GetYaxis();

  x->CenterTitle();

  y->SetTitle("Number of Events");

  y->CenterTitle();

  y->SetRangeUser(0, 2.1e4);

  

  ddt->SetLineColor(kBlue);

  signal->SetLineColor(kRed);

  ccans.new_canvas(energy_label + "_Event_Counts_vs_Beta");

  all->Draw("");

  ddt->Draw("SAME");

  signal->Draw("SAME");
 nova::Simulation();
}

void draw_efficiency(TH1 *eff, std::string type, std::string energy_label)

{

  TAxis *x = eff->GetXaxis();

  TAxis *y = eff->GetYaxis();

  x->CenterTitle();

  y->SetTitle("Efficiency [%]");

  y->CenterTitle();

  eff->Scale(100.0);

  eff->SetMarkerStyle(kFullTriangleUp);

  

  if (type == "Signal")

    eff->SetLineColor(kRed);

  else if (type == "DDT")

    eff->SetLineColor(kBlue);

  std::string title = energy_label + "_" + type + "_Efficiency_vs_Beta";

  ccans.new_canvas(title.c_str());

  eff->Draw("HIST P");

  y->SetRangeUser(0, 105);

  nova::Simulation();

}

void make_efficiency_file(TGraphErrors* eff, std::string outfile_name)

{

  std::string log_beta_string = "log10_beta = [ ";

  std::string beta_string     = "beta       = [ ";

  std::string eff_string      = "mean_eff   = [ ";

  std::string error_string    = "error      = [ ";

  for (int point = 0; point != eff->GetN(); ++point)

  {

    double log_beta, mean_eff;

    eff->GetPoint(point, log_beta, mean_eff);

    double error = eff->GetErrorY(point);

    double beta = std::pow(10, log_beta);

    log_beta_string += std::to_string(log_beta) + " ";

    beta_string     += std::to_string(beta)     + " ";

    eff_string      += std::to_string(mean_eff) + " ";

    error_string    += std::to_string(error)    + " ";

  }

  log_beta_string += "]";

  beta_string     += "]";

  eff_string      += "]";

  error_string    += "]";

  std::ofstream outfile(outfile_name, std::ofstream::trunc);

  outfile << log_beta_string << "\n"

          << beta_string << "\n"

          << eff_string << "\n"

          << error_string << std::endl;
  nova::Simulation();

  outfile.close();

  std::cout << "File " << outfile_name << " has been written." << std::endl;
  

}

TGraph* min_efficiency(TGraphErrors *eff)

{

  int n_points = eff->GetN();

  TGraph* min = new TGraphErrors(n_points);

  for (int point = 0; point != n_points; ++point)

  {

    double log_beta, mean;

    eff->GetPoint(point, log_beta, mean);

    double error = eff->GetErrorY(point);

    min->SetPoint(point, log_beta, mean - error);

  }

  return min;

}

