//

// Run over DataCheck tree and calculate live time

//

#include "Constants.hh"

#include "Data_Quality.hh"

#include "Event_List.hh"

#include "MFRoot.hh"

#include <TFile.h>

#include <TTree.h>

#include <iostream>

#include <map>

using MFRoot::get;

void live_time()

{

  // Read in tree and set up branches:

  TFile *file = MFRoot::open(Constants::DATA_CHECK_FILE);

  TTree *tree = dynamic_cast<TTree*>(file->Get("dc/Subrun"));

  MFRoot::tree_t t;

  for (auto const& name : Constants::DATA_CHECK_BRANCH_NAMES)

    tree->SetBranchAddress(name.c_str(), &t[name]);

  // Read in data quality information:

  dq_t dq = parse_data_quality_csv_file();

  

  // Initialize counters:

  std::map<std::string, double> c;

  c["unique subruns"]   = 0.0;

  c["run time - total"] = 0.0;

  c["run time - 14 DB"] = 0.0;

  c["run time - 14 DB - DQ Good"] = 0.0;

  c["run time - 14 DB - DQ 14 Good DB"] = 0.0;

  c["run time - 14 DB - DQ Scaled"] = 0.0;

  c["n subruns - unique"]      = 0.0;

  c["n subruns - total"]    = tree->GetEntries();

  c["n events"]         = 0.0;

  c["prescale total"]    = 0.0;

  c["n subruns - 14 DB - DQ"] = 0.0;

  c["n subruns - 14 DB - DQ Good"] = 0.0;

  c["n subruns - 14 DB - DQ 14 Good DB"] = 0.0;

  

  Event_List run_list, subrun_list;

  std::map<int, double> run_times;

  for (int entry = 0; entry != tree->GetEntries(); ++entry)

  {

    tree->GetEntry(entry);

    int run    = get(t, "run_number");

    int subrun = get(t, "subrun_number");

    bool duplicate_run = run_list.is_duplicate(run, 0, 0);

    if (!duplicate_run)

    {

      int n_diblocks  = get(t, "run_n_active_diblocks");

      int n_subruns   = get(t, "run_n_subruns");

      double run_time = get(t, "subrun_duration_in_seconds");

      ++c.at("n subruns - unique");

      c.at("run time - total") += run_time;

      if (n_diblocks == 14)

      {

        c.at("run time - 14 DB") += run_time;

        // let's interrogate the dq object

        bool run_exists_in_dq_db = false;

        int n_dq_subruns = 0;

        int n_dq_subruns_good = 0;

        int n_dq_subruns_14_db_good = 0;

        auto dq_run = dq.find(run);

        if (dq_run != dq.end())

        {

          run_exists_in_dq_db = true;

          for (auto const& dq_subrun : dq_run->second)

          {

            ++c["n subruns - 14 DB - DQ"];

            ++n_dq_subruns;

            if (dq_subrun.second.is_good())

            {

              ++c["n subruns - 14 DB - DQ Good"];

              ++n_dq_subruns_good;

            }

            if (dq_subrun.second.is_good_with_14_diblocks())

            {

              ++c["n subruns - 14 DB - DQ 14 Good DB"];

              ++n_dq_subruns_14_db_good;

              // std::cout << "DQMF: " << dq_subrun.second.run() << " "

              //                 << dq_subrun.second.subrun() << std::endl;

            }

          }

        }

        if (n_subruns != n_dq_subruns)

          std::cout << "Subrun Counts: " << n_subruns << ", "

                    << n_dq_subruns << ", "

                    << n_dq_subruns_14_db_good

                    << std::endl;

        if (n_subruns == n_dq_subruns_good)

          c.at("run time - 14 DB - DQ Good") += run_time;

        if (n_subruns == n_dq_subruns_14_db_good)

          c.at("run time - 14 DB - DQ 14 Good DB") += run_time;

        if (run_exists_in_dq_db &&

            n_subruns > 0)

        {

          double scale_factor =

            double(n_dq_subruns_14_db_good) / double(n_subruns);

          c.at("run time - 14 DB - DQ Scaled") += scale_factor * run_time;

        }

      }

    }

    

    // we should embed the subrun calculations within the runs

    // so that we only count the prescale for runs that we

    // are actually considering

    bool duplicate_subrun = subrun_list.is_duplicate(run, subrun, 0);

    if (!duplicate_subrun)

    {

      ++c.at("unique subruns");

      c.at("n events") += get(t, "subrun_n_events");

      c.at("prescale total") += get(t, "subrun_prescale_total");

    }

  }

  for (auto const& count : c)

    std::cout << count.first << ": " << count.second << std::endl;

  

  double prescale = 0.0;

  if (get(c, "n events") > 0)

    prescale = get(c, "prescale total") / get(c, "n events");

  std::cout << "overall prescale: " << prescale << std::endl;

  if (prescale > 0)

    std::cout << "prescale efficiency: " << 1.0 / prescale << std::endl;

}

