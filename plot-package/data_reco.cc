/*

  Run over monopole tree and generate reconstruction histograms 

  for triggered data.

*/

#include "Constants.hh"

#include "MFRoot.hh"

#include <TFile.h>

#include <TH1.h>

#include <TMath.h>

#include <TStyle.h>

#include <TTree.h>

#include <iostream>

typedef std::map<std::string, TH1*> hist_t;

using namespace MFRoot;

Canvas_Manager cans;

double theta(double dvdz)

{ return std::atan(dvdz) * (180.0 / TMath::Pi()); }

void draw_n_tracks(hist_t const& h);

void draw_beta_eq1_track(hist_t const& h);

void draw_beta_le3_tracks(hist_t const& h);

void draw_beta_good_events(hist_t const& h);

void draw_track_length(hist_t const& h);

bool good_event(std::map<std::string, double> const& t)

{

  if (t.at("reco_n_tracks") != 1)

    return false;

  if (t.at("reco_track_1_length") < 1000)

    return false;

  if (t.at("reco_track_1_r2_xt") < 0.1)

    return false;

  if (t.at("reco_track_1_r2_yt") < 0.1)

    return false;

  

  return true;

}

void data_reco()

{

  gStyle->SetOptStat("emroui");

  TFile *file = open(Constants::DATA_RECO_FILE);

  TTree *tree = dynamic_cast<TTree*>(file->Get("mono/Event"));

  std::vector<std::string> names =

    { "reco_n_tracks",

      "reco_track_1_beta", "reco_track_2_beta", "reco_track_3_beta",

      "reco_track_1_length",

      "reco_track_1_r2_xt", "reco_track_1_r2_yt"

    };

  std::map<std::string, double> t;

  for (auto const& name : names)

    tree->SetBranchAddress(name.c_str(), &t[name]);

  hist_t h;

  h["reco_n_tracks"] = new TH1D("reco_n_tracks", "", 5, -0.5, 4.5);

  h["track_length"] = new TH1D("track_length", "", 35, 0, 70);

  

  double xbins[] = { 1e-6, 3.16e-6, 

                     1e-5, 3.16e-5, 

                     1e-4, 3.16e-4, 

                     1e-3, 3.16e-3, 

                     1e-2, 3.16e-2, 

                     1e-1, 3.16e-1, 1 };

  h["beta_eq1_track"] = new TH1D("beta_eq1_track", "", 12, xbins);

  copy(h, "beta_eq1_track", "beta_le3_tracks");

  copy(h, "beta_eq1_track", "beta_good_events");

  

  

  for (int entry = 0; entry != tree->GetEntries(); ++entry)

  {

    tree->GetEntry(entry);

    if (t.at("reco_n_tracks") != -9999)

    {

      h.at("reco_n_tracks")->Fill(t.at("reco_n_tracks"));

      if (t.at("reco_n_tracks") <= 3)

      {

        unsigned n_tracks = std::min(t.at("reco_n_tracks"), 3.0);

        for (unsigned n_track = 1; n_track <= n_tracks; ++n_track)

          h.at("beta_le3_tracks")->

            Fill(t.at("reco_track_" + std::to_string(n_track) + "_beta"));

      }

      

      if (t.at("reco_n_tracks") == 1)

      {

        h.at("beta_eq1_track")->Fill(t.at("reco_track_1_beta"));

        h.at("track_length")->Fill(t.at("reco_track_1_length") / 100);

      }

      if (good_event(t))

        h.at("beta_good_events")->Fill(t.at("reco_track_1_beta"));

    }

  }

  for (auto const& hist : h)

    hist.second->SetLineWidth(2);

  draw_n_tracks(h);

  draw_beta_eq1_track(h);

  // draw_beta_le3_tracks(h);

  draw_beta_good_events(h);

  draw_track_length(h);

}

void draw_n_tracks(hist_t const& h)

{

  TH1* hist = h.at("reco_n_tracks");

  TAxis *x = hist->GetXaxis();

  TAxis *y = hist->GetYaxis();

  x->SetTitle("Number of Reconstructed Tracks");

  x->SetTitleOffset(0.);

  x->CenterTitle();

  

  y->SetTitle("Number of Events");

  y->CenterTitle();

  cans.new_canvas("Number of Tracks", false, true);

  hist->Draw();

}

void draw_beta_eq1_track(hist_t const& h)

{

  TH1* hist = h.at("beta_eq1_track");

  TAxis *x = hist->GetXaxis();

  TAxis *y = hist->GetYaxis();

  x->SetTitle("#beta_{reco}");

  x->SetTitleOffset(0.);

  

  y->SetTitle("Number of Tracks");

  y->CenterTitle();

  // y->SetRangeUser(0, 21);

  cans.new_canvas("Beta eq1 Track", true);

  hist->Draw();

  resize_stats_box(hist, 0.135, 0.335, 0.48, 0.88);

  hist->Draw();

}

void draw_beta_le3_tracks(hist_t const& h)

{

  TH1* hist = h.at("beta_le3_tracks");

  TAxis *x = hist->GetXaxis();

  TAxis *y = hist->GetYaxis();

  x->SetTitle("#beta_{reco}");

  x->SetTitleOffset(0.);

  

  y->SetTitle("Number of Tracks");

  y->CenterTitle();

  y->SetRangeUser(0, 26);

  cans.new_canvas("Beta le3 Tracks", true);

  hist->Draw();

  resize_stats_box(hist, 0.135, 0.335, 0.48, 0.88);

  hist->Draw();

}

void draw_beta_good_events(hist_t const& h)

{

  TH1* hist = h.at("beta_good_events");

  TAxis *x = hist->GetXaxis();

  TAxis *y = hist->GetYaxis();

  x->SetTitle("#beta_{reco}");

  x->SetTitleOffset(0.);

  

  y->SetTitle("Number of Tracks");

  y->CenterTitle();

  // y->SetRangeUser(0, 21);

  cans.new_canvas("Beta Good Events", true);

  hist->Draw();

  resize_stats_box(hist, 0.135, 0.335, 0.48, 0.88);

  hist->Draw();

}

void draw_track_length(hist_t const& h)

{

  TH1* hist = h.at("track_length");

  TAxis *x = hist->GetXaxis();

  TAxis *y = hist->GetYaxis();

  x->SetTitle("Track Length [m]");

  x->SetTitleOffset(0.);

  

  y->SetTitle("Number of Events / 2 m");

  y->CenterTitle();

  cans.new_canvas("Track Length");

  hist->Draw();

}

