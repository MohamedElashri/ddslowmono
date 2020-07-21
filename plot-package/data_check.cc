/*

  Run over the data check tree and generate data quality histograms. 

*/

#include "Constants.hh"

#include "MFRoot.hh"

#include "Run_Info.hh"

#include "Subrun_Info.hh"

#include <TFile.h>

#include <TGraph.h>

#include <TStyle.h>

#include <TTree.h>

#include <iostream>

typedef std::map<int, Run_Info> runs_t;

typedef std::map<std::string, TGraph*> graphs_t;

using MFRoot::get;

MFRoot::Canvas_Manager MEE;

void data_check();

void draw(graphs_t::value_type graph);

#ifndef __CINT__

#include <boost/format.hpp>

void data_check()

{

  // Read in tree and set up branches:

  TFile *file = MFRoot::open(Constants::DATA_CHECK_FILE);

  TTree *tree = dynamic_cast<TTree*>(file->Get("dc/Subrun"));

  MFRoot::tree_t t;

  for (auto const& name : Constants::DATA_CHECK_BRANCH_NAMES)

    tree->SetBranchAddress(name.c_str(), &t[name]);

  // Run over tree and fill graphs and Run_Info:

  runs_t runs;

  for (int entry = 0; entry != tree->GetEntries(); ++entry)

  {

    tree->GetEntry(entry);

    int r = MFRoot::get(t, "run_number");

    Subrun_Info s_info(t);

    

    if (runs.find(r) == runs.end())

    {

      Run_Info r_info(t);

      r_info.add(s_info);

      runs.emplace(r, r_info);

    } else {

      runs[r].add(s_info);

    }

    if (MFRoot::get(t, "subrun_n_events") !=

        MFRoot::get(t, "subrun_n_events_slowmono"))

    {

      std::cerr << "Total number of events not equal slowmonopole "

                << "triggered events! "

                << "\n(run, subrun, n_events, n_events_slowmonopole) ="

                << "\n\t(" << r << ", "

                << s_info.subrun() << ", "

                << t.at("subrun_n_events") << ", "

                << t.at("subrun_n_events_slowmono")

                << std::endl;

    }

  }

  

  boost::format form("%5s %11s %14s %18s %25s");

  std::cout << form % "Run " % "N_SR (DB)" % "N_SR (Found)"

    % "Run Duration [s]" % "Sum Subrun Duration [s]" << std::endl;

  for (auto const& run : runs)

    run.second.print();

  // Set up graphs:

  graphs_t g;

  std::vector<std::string> graph_names =

    {

      "farm_70",

      "n_bn",

      "n_channels",

      "n_dcm",

      "n_diblocks",

      "n_events",

      "n_events_empty",

      "n_events_ddt",

      "n_hits",

      "n_subruns_db",

      "n_subruns_found",

      "n_triggers",

      "partition",

      "prescale_max",

      "subrun_duration"

    };

  for (auto const& name : graph_names)

    g[name] = new TGraph();

  // Fill graphs

  // * Run-level quantities:

  unsigned nr = 0;

  for (auto const& r : runs)

  {

    auto run = r.first;

    auto info = r.second;

    get(g, "farm_70")->

      SetPoint(nr, run, info.get("run_farm70_in_configuration"));

    get(g, "n_bn")->

      SetPoint(nr, run, info.get("run_n_buffer_nodes"));

    get(g, "n_channels")->

      SetPoint(nr, run, info.get("run_n_active_channels"));

    get(g, "n_dcm")->

      SetPoint(nr, run, info.get("run_n_active_dcms"));

    get(g, "n_diblocks")->

      SetPoint(nr, run, info.get("run_n_active_diblocks"));

    get(g, "n_subruns_db")->

      SetPoint(nr, run, info.get("run_n_subruns"));

    get(g, "n_subruns_found")->

      SetPoint(nr, run, info.subruns().size());

    get(g, "n_triggers")->

      SetPoint(nr, run, info.get("run_n_triggers"));

    get(g, "partition")->

      SetPoint(nr, run, info.get("run_partition"));

    

    ++nr;

  }

  // * Subrun-level quantities:

  unsigned ns = 0;

  for (auto const& r : runs)

  {

    for (auto const& s : r.second.subruns())

    {

      double run_number = r.first;

      double subrun_number = s.first;

      double partial_run =

        run_number + subrun_number / Constants::N_SUBRUNS_PER_RUN;

      auto info = s.second;

      

      get(g, "n_events")->

        SetPoint(ns, partial_run, info.get("subrun_n_events"));

      get(g, "n_events_empty")->

        SetPoint(ns, partial_run, info.get("subrun_n_events_empty"));

      get(g, "n_events_ddt")->

        SetPoint(ns, partial_run, info.get("subrun_n_events_slowmono"));

      get(g, "n_hits")->

        SetPoint(ns, partial_run, info.get("subrun_n_hits"));

      get(g, "prescale_max")->

        SetPoint(ns, partial_run, info.get("subrun_prescale_max"));

      get(g, "subrun_duration")->

        SetPoint(ns, partial_run, info.get("subrun_duration_in_seconds"));

      ++ns;

    }

  }

  for (auto const& graph : g)

    draw(graph);

}

void draw(graphs_t::value_type graph)

{

  std::string title = graph.first;

  TGraph *g = graph.second;

  MEE.new_canvas(title);

  g->Draw("AP");

  g->SetMarkerStyle(kMultiply);

  TAxis *x = g->GetXaxis();

  x->SetTitle("Run");

  x->SetTitleOffset(1.2);

  x->SetRangeUser(19700, 20800);

  

  TAxis *y = g->GetYaxis();

  y->SetTitle(title.c_str());

  y->SetTitleOffset(1.2);

  y->CenterTitle();

}

#endif

