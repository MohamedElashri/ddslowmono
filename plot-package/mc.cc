/*

  Run over monopole tree and generate MC truth histograms.

*/

#include "Constants.hh"

#include "MFRoot.hh"

#include <TFile.h>

#include <TH1.h>

#include <TLegend.h>

#include <TMath.h>

#include <TStyle.h>

#include <TTree.h>

#include <iostream>

using namespace MFRoot;

Canvas_Manager gepp;

double theta(double dvdz)

{ return std::atan(dvdz) * (180.0 / TMath::Pi()); }

void draw_theta_x(hist_t const& h);

void draw_theta_y(hist_t const& h);

void mc(int const n_events = 1000)

{

  gStyle->SetOptStat("");

  TFile *file =  open(Constants::MC_RECO_FILE);

  // TFile *file = open("~/nsrc/mono/Monopole_Tree.root");

  TTree *tree = dynamic_cast<TTree*>(file->Get("mono/Event"));

  std::vector<std::string> names =

    { "mc_dxdz", "gen_dxdz",

      "mc_dydz", "gen_dydz",

      "mc_monopole_hit_detector"

    };

  tree_t t;

  for (auto const& name : names)

    tree->SetBranchAddress(name.c_str(), &t[name]);

  hist_t h;

  // Angles

  h["mc_theta_x"] = new TH1D("mc_theta_x", "", 19, -95, 95);

  copy(h, "mc_theta_x", "mc_theta_y");

  copy(h, "mc_theta_x", "gen_theta_x");

  copy(h, "mc_theta_x", "gen_theta_y");

  

  // Velocity

  h["mc_beta"] = new TH1D("mc_beta", "", 10, 0, 0.01);

  copy(h, "mc_beta", "gen_beta");

  

  int const until = std::min(n_events, static_cast<int>(tree->GetEntries()));

  for (int entry = 0; entry != until; ++entry)

  {

    tree->GetEntry(entry);

    h.at("gen_theta_x")->Fill(theta(t.at("gen_dxdz")));

    h.at("gen_theta_y")->Fill(theta(t.at("gen_dydz")));

    

    if (t.at("mc_monopole_hit_detector"))

    {

      h.at("mc_theta_x")->Fill(theta(t.at("mc_dxdz")));

      h.at("mc_theta_y")->Fill(theta(t.at("mc_dydz")));

    }

  }

  for (auto const& hist : h)

    hist.second->SetLineWidth(2);

  draw_theta_x(h);

  draw_theta_y(h);

}

void draw_theta_x(hist_t const& h)

{

  TH1* gen = h.at("gen_theta_x");

  TH1* mc = h.at("mc_theta_x");

  TAxis *x = gen->GetXaxis();

  TAxis *y = gen->GetYaxis();

  mc->SetLineColor(kBlack);

  gen->SetLineColor(kGreen + 1);

  

  x->SetTitle("#theta_{x}");

  x->SetTitleOffset(1.2);

  y->SetTitle("Number of Events");

  y->CenterTitle();

  y->SetRangeUser(0, 100);

  gepp.new_canvas("theta_x");

  gen->Draw("");

  mc->Draw("SAME");

  

  TLegend *l = new TLegend(0.55, 0.75, 0.88, 0.88);

  l->AddEntry

    (gen, Form("Generator (N_{events} = %.0f)", gen->GetEntries()), "l");

  l->AddEntry

    (mc, Form("Hits in Detector (N_{events} = %.0f)", mc->GetEntries()), "l");

  l->Draw();

}

void draw_theta_y(hist_t const& h)

{

  TH1* gen = h.at("gen_theta_y");

  TH1* mc = h.at("mc_theta_y");

  TAxis *x = gen->GetXaxis();

  TAxis *y = gen->GetYaxis();

  mc->SetLineColor(kBlack);

  gen->SetLineColor(kGreen + 1);

  

  x->SetTitle("#theta_{y}");

  x->SetTitleOffset(1.2);

  y->SetTitle("Number of Events");

  y->CenterTitle();

  y->SetRangeUser(0, 100);

  gepp.new_canvas("theta_y");

  gen->Draw("");

  mc->Draw("SAME");

  

  TLegend *l = new TLegend(0.55, 0.75, 0.88, 0.88);

  l->AddEntry

    (gen, Form("Generator (N_{events} = %.0f)", gen->GetEntries()), "l");

  l->AddEntry

    (mc, Form("Hits in Detector (N_{events} = %.0f)", mc->GetEntries()), "l");

  l->Draw();

}

