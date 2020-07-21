/*

  This program calculates the 90% C.L. upper limit on the monopole flux

  for a given velocity point.

 */

#include "Constants.hh"

#include "Event_Info.hh"

#include "Limit_Info.hh"

#include "MFRoot.hh"

#include <TFile.h>

#include <TGraph.h>

#include <TLegend.h>

#include <TTree.h>

#include <iostream>

#include <map>

typedef std::map<double, Limit_Info> lim_t;

using namespace MFRoot;

Canvas_Manager vi;

void draw_limits(lim_t lims, std::string name);

lim_t extract_limits(std::string file_name);

void limit()

{

  std::map<std::string, lim_t> l;

  // l["low"] = extract_limits(Constants::MC_RECO_FILE_LOW_ENERGY);

  // l["high"] = extract_limits(Constants::MC_RECO_FILE_HIGH_ENERGY);

  l["0.9dEdx"] = extract_limits(Constants::MC_RECO_FILE);

  // l["0.9dEdx"] = extract_limits("~/data/mono/200220_Monpole_Tree_MC_0.9dEdx_Extra_Beta_Points.root");

  // draw_limits(l["low"], "LowE");

  // draw_limits(l["high"], "HighE");

  draw_limits(l["0.9dEdx"], "0.9dEdx");

  // for (auto const& lim : l["0.9dEdx"])

  //   lim.second.print();

  // determine the maximum limit

  // for (auto const& lim : l["low"])

  // {

  //   double beta = lim.first;

  //   if (l["low"].at(beta).limit("half") > l["high"].at(beta).limit("half"))

  //     l["max"][beta] = l["low"].at(beta);

  //   else

  //     l["max"][beta] = l["high"].at(beta);

  // }

  // draw_limits(l["max"], "Max");

  // std::cout << "\n\nFinal Limits" << std::endl;

  // for (auto const& lim : l["max"])

  //   lim.second.print();

}

void draw_limits(lim_t lims, std::string name)

{

  std::map<std::string, TGraph*> g;

  g["half"] = new TGraph();

  g["full"] = new TGraph();

  int n_point = 0;

  for (auto const& lim : lims)

  {

    Limit_Info info = lim.second;

    // if (info.log_beta() > -3.6 &&

    //         info.log_beta() < -2.2)

    if (info.log_beta() > -3.8 &&

        info.log_beta() < -1.9)

    {

      g.at("half")->SetPoint(n_point, info.log_beta(), info.limit("half"));

      g.at("full")->SetPoint(n_point, info.log_beta(), info.limit("full"));

      ++n_point;

    }

  }

  std::string canvas_name = "Limits_" + name;

  vi.new_canvas(canvas_name, false, true, false, true);

  TCanvas *can = vi.get(canvas_name);

  vi.get(canvas_name)->SetLeftMargin(0.12);

  vi.get(canvas_name)->SetRightMargin(0.05);

  vi.get(canvas_name)->SetBottomMargin(0.12);

  vi.get(canvas_name)->SetTopMargin(0.09);

  

  g.at("half")->Draw("APC");

  g.at("full")->Draw("SAME PC");

  

  g.at("half")->SetMarkerStyle(kOpenSquare);

  g.at("full")->SetMarkerStyle(kFullSquare);

  g.at("half")->SetLineStyle(kDashed);

  g.at("full")->SetLineStyle(kSolid);

  

  TAxis *x = g.at("half")->GetXaxis();

  x->SetTitle("Monopole Velocity (#beta)");

  x->SetTitleOffset(1.2);

  x->SetTitleSize(0.04);

  x->CenterTitle();

  x->SetNdivisions(6, 5, 0, true);

  x->SetLabelOffset(0.012);

  x->ChangeLabel(1, -1, -1, -1, -1, -1, "10^{-3.5}");

  x->ChangeLabel(2, -1, -1, -1, -1, -1, "10^{-3}");

  x->ChangeLabel(3, -1, -1, -1, -1, -1, "10^{-2.5}");

  TAxis *y = g.at("half")->GetYaxis();

  y->SetTitle("90% C.L. Upper Flux Limit (cm^{-2}s^{-1}sr^{-1})");

  y->CenterTitle();

  y->SetTitleOffset(1.3);

  y->SetTitleSize(0.04);

  y->SetRangeUser(1e-16, 1e-11);

  nova::Preliminary();

  TLegend *l = new TLegend(0.5, 0.73, 0.91, 0.87);

  l->AddEntry

    (g.at("half"), "#Omega = 2#pi, m > 5 #times 10^{8} GeV/c^{2}", "pl");

  l->AddEntry

    (g.at("full"), "#Omega = 4#pi, m > 2 #times 10^{15} GeV/c^{2}", "pl");

  l->Draw();

}

lim_t extract_limits(std::string file_name)

{

  std::cout << "extracting limits from " << file_name

            << std::endl;

  TFile *file = open(file_name);

  TTree *tree = dynamic_cast<TTree*>(file->Get("mono/Event"));

  tree_t t;

  std::string *input_file_name = new std::string("invalid");

  tree->SetBranchAddress("input_file_name", &input_file_name);

  for (auto const& name : Constants::BRANCH_NAMES)

    tree->SetBranchAddress(name.c_str(), &t[name]);

  lim_t lims;

  

  for (int entry = 0; entry != tree->GetEntries(); ++entry)

  // for (int entry = 0; entry != 10000; ++entry)

  {

    if (entry % 100000 == 0)

      std::cout << "processing entry " << entry << " ..." << std::endl;

    tree->GetEntry(entry);

    if (*input_file_name == "invalid")

    {

      std::cerr << "\n\n\t\tThe input_file_name branch is not set!\n"

                << std::endl;

      

      assert(false);

    }

    Event_Info e(t, *input_file_name);

    double beta_f = e.beta_from_file_name();

    if (lims.find(beta_f) == lims.end())

      lims[beta_f] = Limit_Info(beta_f);

    lims.at(beta_f).increment_counts(e.is_signal());

    if (e.is_signal())

      lims.at(beta_f).add_area(e.area_projected());

  }

  for (auto const& lim : lims)

    lim.second.print();

  return lims;

}

