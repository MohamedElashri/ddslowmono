/*

  Run over Data and MC monopole trees and print event counts.

*/

#include "Constants.hh"

#include "Event_Info.hh"

#include "Event_List.hh"

#include "MFRoot.hh"

#include <TFile.h>

#include <TH1.h>

#include <TMath.h>

#include <TStyle.h>

#include <TTree.h>

#include <iomanip>

#include <iostream>

using namespace MFRoot;

typedef std::map<std::string, unsigned> count_t;

void count_events(count_t & c, tree_t const& t, TTree* tree,

                  std::string type, std::string beta_name);

void print       (count_t const& c, std::string type);

void cuts()

{

  std::map<std::string, TFile*> files;

   files[Constants::DATA_SAMPLE_NAME] = open(Constants::DATA_RECO_FILE);

   files["MC"]  = open(Constants::MC_RECO_FILE);

  // files["MC_new_light_levels"] = open("~/data/mono/191218_Monopole_Tree_MC_1e-3_new_light_levels.root");

  // files["MC_original"] = open("~/data/mono/191218_Monopole_Tree_MC_1e-3_original.root");

  // files["MC_reasonable"] = open("~/data/mono/200128_Monopole_Tree_MC_1e-3_reasonable_light_levels.root");

  // files["MC_reasonable_overlay"] = open("~/data/mono/200130_Monopole_Tree_MC_Overlay_1e-3_reasonable_light_levels.root");

  // files["SlowParticle_1e-3 TimeWindow=10us"] =

  // open("~/data/mono/200303_SlowParticle_Tree_Beta1e-3.root");

  // files["SlowParticle_1e-4 TimeWindow=10us"] =

  // open("~/data/mono/200303_SlowParticle_Tree_Beta1e-4.root");

  // files["SlowParticle_1e-3 TimeWindow=100us"] =

    // open("~/data/mono/200305_SlowParticle_Tree_Beta1e-3.root");

  // files["SlowParticle_1e-4 TimeWindow=100us"] =

    // open("~/data/mono/200305_SlowParticle_Tree_Beta1e-4.root");

  std::map<std::string, TTree*> trees;

  for (auto const& file : files)

    trees[file.first] = dynamic_cast<TTree*>(file.second->Get("mono/Event"));

  trees_t t;

  for (auto const& tree : trees)

    for (auto const& name : Constants::BRANCH_NAMES)

      tree.second->SetBranchAddress(name.c_str(), &(t[tree.first])[name]);

  bool print_title = true;

  for (auto const& tree : trees)

  {

    count_t counts =

      {

        { "1 all", 0 },

        { "2 hit det.", 0 },

        { "3 presel mc", 0 },

        { "4 ddt", 0},

        { "5 presel reco", 0 },

        { "6 linear", 0 },

        { "7 slow", 0 },

        { "hits", 0 },

        { "dplane", 0 },

        { "length", 0 }

      };

    std::string type = tree.first;

    count_events(counts, t.at(type), tree.second,

                 type, Constants::MC_BETA_NAME);

    if (print_title)

    {

      print_title = false;

      std::cout << std::setw(13) << "Type" << " |";

      for (auto const& count : counts)

        std::cout << std::setw(15) << count.first << " |";

      std::cout << std::endl;

    }

    std::cout << "beta = " << Constants::MC_BETA_NAME << std::endl;

    print(counts, type);

  }

}

void count_events

(count_t & c, tree_t const& t, TTree* tree,

 std::string type, std::string beta_name)

{

  std::string *input_file_name = new std::string("invalid");

  if (type != Constants::DATA_SAMPLE_NAME)

    tree->SetBranchAddress("input_file_name", &input_file_name);

  Event_List elist(type);

  for (int entry = 0; entry != tree->GetEntries(); ++entry)

  // for (int entry = 0; entry != 100; ++entry)

  {

    tree->GetEntry(entry);

    // Event_Info e(t, *input_file_name);

    Event_Info e(t);

    bool duplicate_event = elist.is_duplicate(e.run(), e.subrun(), e.event());

    if (!duplicate_event)

    {

      // if (e.is_data() || e.beta_name() == beta_name)

      {

        ++c.at("1 all");

        if (e.hit_detector() || e.is_data())

          ++c.at("2 hit det.");

        if (e.get("mc_n_hits_x") >= 20 && e.get("mc_n_hits_y") >= 20)

          ++c.at("hits");

      

        if (e.get("mc_dplane_x") >= 10 && e.get("mc_dplane_y") >= 10)

          ++c.at("dplane");

      

        if (e.get("mc_length") >= 1000)

          ++c.at("length");

        if (e.is_preselected_mc() || e.is_data())

          ++c.at("3 presel mc");

        if (e.is_triggered() || e.is_data())

        {

          ++c.at("4 ddt");

          if (e.is_preselected_reco())

          {

            ++c.at("5 presel reco");

            if (type == Constants::DATA_SAMPLE_NAME)

              if (e.low_gap_max() &&

                  e.r2_min() > 0.5 &&

                  e.beta() < 1e-2)

              {

                double dxdz = e.theta(e.first_preselected_reco_track(), "xz");

                double dydz = e.theta(e.first_preselected_reco_track(), "yz");

                std::cout << "Interesting Event: "

                          << e.run() << ", "

                          << e.subrun() << ", "

                          << e.event() << ", "

                          << e.beta() << ", "

                          << e.r2_min() << ", "

                          << e.gap_max() << ", "

                          << e.first_preselected_reco_track() << ", "

                          << dxdz << ", "

                          << dydz << std::endl;

              }

    

            if (e.is_linear())

            {

              ++c.at("6 linear");

              if (e.is_slow())

              {

                ++c.at("7 slow");

                if (e.is_data())

                  std::cout << "Slow Event! (run, event, beta, "

                            << "r2_min, gap_max) = ("

                            << std::fixed

                            << e.run() << ", "

                            << e.subrun() << ", "

                            << e.event() << ", "

                            << e.beta() << ", "

                            << e.r2_min() << ", "

                            << e.gap_max() << ")" << std::endl;

              }

            }

            // } else {

            //   double length = e.get("mc_length");

            //   if (length > 3000 && length < 4000)

            //     std::cout << "Failed Event (" << type << ") "

            //               << std::fixed

            //               << e.run() << ", "

            //               << e.subrun() << ", "

            //               << e.event() << ", "

            //               << length

            //               << std::endl;

          }

        }

      }

    }

  }

  if (type == Constants::DATA_SAMPLE_NAME)

    std::cout << "Tree " << type << " contains "

              << elist.n_duplicate_events()

              << " duplicate events."

              << std::endl;

  delete input_file_name;

}

void print(count_t const& c, std::string type)

{

  std::cout << std::setw(7) << type << " |";

  for (auto const& count : c)

    std::cout << std::setw(15) << count.second << " |";

  // Poisson Efficiency

  double k = c.at("7 slow");

  double n = c.at("1 all");

  double efficiency(0.0), error(0.0);

  if (n > 0)

  {

    efficiency = k            / n;

    error      = std::sqrt(k) / n;

  }

  std::cout << efficiency << " +- " << error << std::endl;

}

