/*

  Run over Mono_Tree.root and generate some simple histograms.

*/



#include "Constants.hh"

#include "MFRoot.hh"

#include <TFile.h>

#include <TH1.h>

#include <TStyle.h>

#include <TTree.h>

#include <iostream>

using namespace MFRoot;

Canvas_Manager ss;

void mono()

{

  unsigned plot_feb = 2;

  unsigned plot_channel = 2;

  TFile *file = open("Monopole_Tree_trivial.root");

  TTree *tree = dynamic_cast<TTree*>(file->Get("mono/Event"));

  double length;

  tree->SetBranchAddress("ADC", &length);

  TH1 *h_length = new TH1D("length", "", 50, 0, 5000);

  for (int entry = 0; entry != tree->GetEntries(); ++entry)

  {

    tree->GetEntry(entry);

    h_length->Fill(length / Constants::SAMPLES_PER_MICROSECOND);

  }

  ss.new_canvas("Triggered Event Length");

  h_length->Draw();

  TAxis *x = h_length->GetXaxis();

  x->SetTitle("Event Length [#mus]");

  // x->SetRangeUser(0, 2100);

  TAxis *y = h_length->GetYaxis();

  y->SetTitle("Number of Events / 100 #mus");

  y->CenterTitle();

}

