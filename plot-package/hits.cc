/*

  Plot the Number of Hits and Tracks

 */

#include "MFRoot.hh"

#include <TAxis.h>

#include <TFile.h>

#include <TH1.h>

#include <TLegend.h>

#include <TStyle.h>

#include <iostream>

using namespace MFRoot;

Canvas_Manager hitss;

void normalize(TH1* hist);

  

void hits()

{

  gStyle->SetOptStat("");

  std::string file_name = "Monopole_Tree_trivial.root";

  TFile* file = open(file_name);

  std::vector<std::string> names =

    { "x_hits", "y_hits", "x_contained_hits", "x_surface_hits",

      "n_tracks", "n_good_tracks" };

  std::map<std::string, TH1*> h;

  for (auto const& name : names)

  {

    h[name] = get(file, name);

    normalize(h[name]);

  }

  h["x_surface_hits"]->Scale(0.2);

  //

  // Hit Counts

  //

  h["x_hits"]->SetTitle("");

  h["x_hits"]->GetXaxis()->SetTitle("Number of Hits");

  h["x_hits"]->GetXaxis()->SetTitleOffset(1.2);

  h["x_hits"]->GetYaxis()->SetTitle("Arbitrary Units");

  h["x_hits"]->GetYaxis()->CenterTitle();

  h["x_hits"]->GetYaxis()->SetTitleOffset(1.2);

  h["x_hits"]->GetYaxis()->SetRangeUser(0, 0.25);

  hitss.new_canvas("Hit Counts");

  h["x_hits"]->Draw();

  h["x_hits"]->SetLineColor(kBlack);

  h["x_hits"]->SetFillColor(kBlack);

  

  h["y_hits"]->Draw("SAME");

  h["y_hits"]->SetLineColor(kGreen + 2);

  h["x_contained_hits"]->Draw("SAME");

  h["x_contained_hits"]->SetLineColor(kOrange + 8);

  h["x_contained_hits"]->SetFillColor(kOrange + 8);

  h["x_surface_hits"]->Draw("SAME");

  h["x_surface_hits"]->SetLineColor(kRed);

  h["x_surface_hits"]->SetFillColor(kRed);

  h["x_hits"]->Draw("SAME");

  TLegend* l = new TLegend(0.27, 0.65, 0.85, 0.85);

  l->AddEntry(h["y_hits"], Form("Y Hits (Mean = %.0f)",

                                h["y_hits"]->GetMean()));

  l->AddEntry(h["x_hits"], Form("X Hits (Mean = %.0f)",

                                h["x_hits"]->GetMean()));

  l->AddEntry(h["x_contained_hits"], Form("Matched X Hits (Mean = %.0f)",

                                          h["x_contained_hits"]->GetMean()));

  l->AddEntry(h["x_surface_hits"], Form("Surface X Hits (Mean = %.0f)",

                                        h["x_surface_hits"]->GetMean()));

  l->Draw();

  //

  // Track Counts

  //

  h["n_tracks"]->SetTitle("");

  h["n_tracks"]->GetXaxis()->SetTitle("Number of Tracks");

  h["n_tracks"]->GetXaxis()->SetTitleOffset(1.2);

  h["n_tracks"]->GetYaxis()->SetTitle("Arbitrary Units");

  h["n_tracks"]->GetYaxis()->CenterTitle();

  h["n_tracks"]->GetYaxis()->SetTitleOffset(1.2);

  h["n_tracks"]->GetYaxis()->SetRangeUser(0, 0.25);

  hitss.new_canvas("Track Counts");

  h["n_tracks"]->Draw();

  h["n_tracks"]->SetLineColor(kBlack);

  h["n_good_tracks"]->Draw("SAME");

  h["n_good_tracks"]->SetLineColor(kRed);

  TLegend* lt = new TLegend(0.35, 0.7, 0.8, 0.85);

  lt->AddEntry(h["n_tracks"], Form("All Tracks (Mean = %.0f)",

                                   h["n_tracks"]->GetMean()));

  lt->AddEntry(h["n_good_tracks"], Form("Good Tracks (Mean = %.0f)",

                                        h["n_good_tracks"]->GetMean()));

  lt->Draw();

}

void normalize(TH1* hist)

{

  double area = hist->Integral();

  hist->Scale(1.0 / area);

}

