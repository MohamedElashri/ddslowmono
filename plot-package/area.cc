/*

  This program calculates the projected area for each monopole based on

  its MC direction.

 */

#include "Constants.hh"

#include "Event_Info.hh"

#include "MFRoot.hh"

#include <TFile.h>

#include <TTree.h>

#include <TVector3.h>

#include <iostream>

#include <map>

using namespace MFRoot;

double projected_area(TVector3 const& A, TVector3 const& v);

void area()

{

  // Horizontal monopoles (good for testing):

  // TFile *file = open("~/data/mono/"

  //                      "190618_Monopole_Tree_SingleP_Monopole_1e-3_TheoryE.root");

  TFile *file = open(Constants::MC_RECO_FILE);

  

  TTree *tree = dynamic_cast<TTree*>(file->Get("mono/Event"));

  tree_t t;

  std::string *input_file_name = new std::string("invalid");

  tree->SetBranchAddress("input_file_name", &input_file_name);

  for (auto const& name : Constants::BRANCH_NAMES)

    tree->SetBranchAddress(name.c_str(), &t[name]);

  // object to keep track of the different areas:

  std::map<std::string, double> aa =

    { { "total"           , 0.0 },

      { "projected"       , 0.0 },

      { "signal_total"    , 0.0 },

      { "signal_projected", 0.0 }

    };

  double n_monopoles(0.0), n_monopoles_signal(0.0);

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

    

    if (e.beta_name() == Constants::MC_BETA_NAME &&

        e.hit_detector())

    {

      ++n_monopoles;

      if (e.is_signal())

        ++n_monopoles_signal;

      for (auto const& area_normal : Constants::AREA_NORMALS)

      {

        TVector3 A = area_normal.second;

        aa.at("total") += A.Mag();

        if (e.is_signal())

          aa.at("signal_total") += A.Mag();

        TVector3 v = e.mc_velocity_unit_vector();

        

        if (A.Dot(v) < 0)

        {

          aa.at("projected") += projected_area(A, v);

          

          if (e.is_signal())

            aa.at("signal_projected") += projected_area(A, v);

        }

      }

    }

  }

    

  std::cout << "\nFinal Result"

            << "\nI. Totals:"

            << "\nN_monopoles:                       "

            << n_monopoles

            << "\ntotal area:                        "

            << aa.at("total")

            << "\ntotal projected area:              "

            << aa.at("projected")

            << "\ntotal area per monopole            "

            << aa.at("total") / n_monopoles

            << "\nprojected area per monopole:       "

            << aa.at("projected") / n_monopoles

            << "\ntotal projected area / total area: "

            << aa.at("projected") / aa.at("total")

                << std::endl;

  std::cout << "\n\nII. Signal"

                << "\nN_monopoles:                       "

            << n_monopoles_signal

            << "\ntotal area:                        "

            << aa.at("signal_total")

            << "\ntotal projected area:              "

            << aa.at("signal_projected")

            << "\ntotal area per monopole            "

            << aa.at("signal_total") / n_monopoles_signal

            << "\nprojected area per monopole:       "

            << aa.at("signal_projected") / n_monopoles_signal

            << "\ntotal projected area / total area: "

            << aa.at("signal_projected") / aa.at("signal_total")

                << std::endl;

}

double projected_area(TVector3 const& A, TVector3 const& v)

{

  return std::fabs(A.Dot(v));

}

