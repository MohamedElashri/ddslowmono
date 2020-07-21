/*

  Run over monopole tree and generate reconstruction histograms 

  for MC.

*/

#include "Constants.hh"

#include "Event_Info.hh"

#include "MFRoot.hh"

#include <TFile.h>

#include <TH1.h>

#include <TLegend.h>

#include <TMath.h>

#include <TStyle.h>

#include <TTree.h>

#include <iomanip>

#include <iostream>

using namespace MFRoot;

Canvas_Manager cans;

void declare_hists         (hist_t & h);

void draw_beta             (hist_t const& h);

void draw_dEdx             (hist_t const& h);

void draw_dplane           (hist_t const& h, std::string view);

void draw_dplane_eff       (hist_t const& h, std::string view,

                            std::string type);

void draw_dxdz             (hist_t const& h);

void draw_dydz             (hist_t const& h);

void draw_length           (hist_t const& h);

void draw_length_comparison(hist_t const& h);

void draw_length_eff       (hist_t const& h, std::string type);

void draw_mc_fraction      (hist_t const& h, std::string hist_name);

void draw_n_hits           (hist_t const& h, std::string view);

void draw_n_tracks         (hist_t const& h);

void draw_best_track       (hist_t const& h);

void draw_theta            (hist_t const& h, std::string view);

void draw_theta_eff        (hist_t const& h, std::string view,

                            std::string type);

void fill_hists            (hist_t & h, tree_t const& t, TTree* tree);

void make_copies           (hist_t & h, std::string name);

void make_nice             (TH1* mc, TH1* mc_reco, TH1* reco,

                            TH1* mc_ddt = NULL);

void mc_reco()

{

  gStyle->SetOptStat("");

  TFile *file = open(Constants::MC_RECO_FILE);

  TTree *tree = dynamic_cast<TTree*>(file->Get("mono/Event"));

  std::map<std::string, double> t;

  for (auto const& name : Constants::BRANCH_NAMES)

    tree->SetBranchAddress(name.c_str(), &t[name]);

  hist_t h;

  std::cout << "Declaring Histograms ..." << std::endl;

  declare_hists(h);

  std::cout << "Filling Histograms ..." << std::endl;

  fill_hists(h, t, tree);  

  // std::cout << "Normalizing Histograms ..." << std::endl;

  

  // for (auto & hist : h)

  // {

  //   double n_entries = hist.second->GetEntries();

  //   hist.second->Scale(1.0 / n_entries);

  //   std::cout << hist.first << " " << n_entries << std::endl;

  // }

  

  std::cout << "Drawing Histograms ..." << std::endl;

  for (auto const& hist : h)

    hist.second->SetLineWidth(2);

  // draw_beta(h);

  // draw_dxdz(h);

  // draw_dydz(h);

  // draw_length_comparison(h);

  

  // draw_best_track(h);

  

  // draw_mc_fraction(h, "mc_fraction");

  // draw_mc_fraction(h, "mc_fraction_linear");

  // draw_n_hits(h, "x");

  // draw_n_hits(h, "y");

  // draw_n_hits(h, "all");

  // draw_n_hits(h, "ge_100_ADC");

  // draw_n_hits(h, "ge_100_ADC_fraction");

  // draw_n_tracks(h);

  // Main Plots:

  draw_theta(h, "x");

  draw_theta_eff(h, "x", "ddt");

  draw_theta_eff(h, "x", "reco");

  draw_theta(h, "y");

  draw_theta_eff(h, "y", "ddt");

  draw_theta_eff(h, "y", "reco");

  draw_dplane(h, "x");

  draw_dplane_eff(h, "x", "ddt");

  draw_dplane_eff(h, "x", "reco");

  draw_dplane(h, "y");

  draw_dplane_eff(h, "y", "ddt");

  draw_dplane_eff(h, "y", "reco");

  draw_length(h);

  draw_length_eff(h, "ddt");

  draw_length_eff(h, "reco");

  // draw_dEdx(h);

  

  std::cout << "Finished." << std::endl;

}

/////////////////////////////////////////////////////////////////////////////

//

// Declare Histograms

//

/////////////////////////////////////////////////////////////////////////////

void declare_hists(hist_t & h)

{

  h["mc_adc_mean"] = new TH1D("mc_adc_mean", "", 400, 0, 2000);

  h["mc_adc_per_dEdx"] = new TH1D("mc_adc_per_dEdx", "", 200, 0, 200);

  h["mc_beta"] = new TH1D("mc_beta", "", 10, 0, 0.01);

  copy(h, "mc_beta", "reco_track_1_beta");

  h["mc_dEdx"] = new TH1D("mc_dEdx", "", 400, 0, 40);

  copy(h, "mc_dEdx", "mc_dEdx_Birks");

  

  h["mc_dxdz"] = new TH1D("mc_dxdz", "", 101, -50.5, 50.5);

  copy(h, "mc_dxdz", "reco_track_1_dxdz");

  copy(h, "mc_dxdz", "mc_dydz");

  copy(h, "mc_dydz", "reco_track_1_dydz");

  

  h["mc_fraction"] = new TH1D("mc_fraction", "", 50, 0, 1);

  copy(h, "mc_fraction", "mc_fraction_linear");

  h["mc_length"] = new TH1D("mc_length", "", 35, 0, 70);

  make_copies(h, "length");

  

  h["mc_n_hits_all"] = new TH1D("mc_n_hits_all", "", 50, 0, 500);

  make_copies(h, "n_hits_all");

  copy(h, "mc_n_hits_all", "mc_n_hits_x");

  make_copies(h, "n_hits_x");

  copy(h, "mc_n_hits_all", "mc_n_hits_y");

  make_copies(h, "n_hits_y");

  copy(h, "mc_n_hits_all", "mc_n_hits_ge_100_ADC");

  make_copies(h, "n_hits_ge_100_ADC");

  h["mc_n_hits_ge_100_ADC_fraction"] =

    new TH1D("mc_n_hits_ge_100_ADC_fraction", "", 50, 0, 1);

  make_copies(h, "n_hits_ge_100_ADC_fraction");

  h["mc_n_tracks"] = new TH1D("mc_n_tracks", "", 10, -0.5, 9.5);

  make_copies(h, "n_tracks");

  

  h["mc_theta_x"] = new TH1D("mc_theta_x", "", 37, -92.5, 92.5);

  copy(h, "mc_theta_x", "mc_theta_y");

  make_copies(h, "theta_x");

  make_copies(h, "theta_y");

  copy(h, "mc_theta_x", "reco_track_1_theta_x");

  copy(h, "mc_theta_y", "reco_track_1_theta_y");

  // h["mc_dplane_x"] = new TH1D("mc_dplane_x", "", 30, 0, 300);

  h["mc_dplane_x"] = new TH1D("mc_dplane_x", "", 30, 0, 900);

  make_copies(h, "dplane_x");

  copy(h, "mc_dplane_x", "mc_dplane_y");

  make_copies(h, "dplane_y");

  h["best_reco_track"] = new TH1D("best_reco_track", "", 4, -0.5, 3.5);

}

void make_copies(hist_t & h, std::string name)

{

  copy(h, "mc_" + name, "mc_" + name + "_reco");

  copy(h, "mc_" + name, "reco_" + name);

  copy(h, "mc_" + name, "mc_" + name + "_ddt");

}

/////////////////////////////////////////////////////////////////////////////

//

// Fill Histograms

//

/////////////////////////////////////////////////////////////////////////////

void fill_hists(hist_t & h, tree_t const& t, TTree* tree)

{

  std::string *input_file_name = new std::string("invalid");

  tree->SetBranchAddress("input_file_name", &input_file_name);

  // std::cout << "MF: event, mc_length, mc_fraction, ddt, "

  //             << "preselected mc, preselected reco, "

  //             << "n_track, linear, r2, r2_xt, r2_yt, f, slow, "

  //             << "mc_dx, mc_dy, mc_dz, mc_theta_x, mc_theta_y, "

  //             << "mc_n_hits_x, mc_n_hits_y, n_hits_x, n_hits_y, n_mc_hits, "

  //             << "non_mc_hits, beta "

  //             << std::endl;

  std::map<std::string, int> c =

    { {"all", 0},

      {"ddt", 0},

      {"failed presel reco", 0},

      {"no reco tracks", 0},

      {"one reco track", 0},

      {"length < 10 m", 0}

    };

  

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

    // Event_Info e(t);

    if (e.beta_name() == Constants::MC_BETA_NAME)

    {

      double mc_length = e.get("mc_length") / 100;

      if (mc_length > 10)

      // if (e.is_preselected_reco())

      {

        get(h, "mc_dplane_x")->Fill(e.get("mc_dplane_x"));

        get(h, "mc_dplane_y")->Fill(e.get("mc_dplane_y"));

        get(h, "mc_fraction")->Fill(e.mc_fraction());

        get(h, "mc_length")->Fill(mc_length);

        get(h, "mc_n_hits_all")->Fill(e.get("mc_n_hits_all"));

        get(h, "mc_n_hits_x")->Fill(e.get("mc_n_hits_x"));

        get(h, "mc_n_hits_y")->Fill(e.get("mc_n_hits_y"));

        // double ge_100_ADC_fraction =

        //   e.get("mc_n_hits_ge_100_ADC") / e.get("mc_n_hits_all_without_noise");

        // get(h, "mc_n_hits_ge_100_ADC")->Fill(e.get("mc_n_hits_ge_100_ADC"));

        // get(h, "mc_n_hits_ge_100_ADC_fraction")->Fill(ge_100_ADC_fraction);

        get(h, "mc_n_tracks")->Fill(e.hit_detector());

        get(h, "mc_theta_x")->Fill(e.mc_theta("xz"));

        get(h, "mc_theta_y")->Fill(e.mc_theta("yz"));

        get(h, "mc_adc_mean")->Fill(e.get("mc_adc_mean"));

        get(h, "mc_dEdx")->Fill(1e3 * e.get("mc_dEdx"));

        get(h, "mc_dEdx_Birks")->Fill(1e3 * e.get("mc_dEdx_Birks"));

        // units: ADC / (MeV/cm)

        double adc_per_dEdx =

          e.get("mc_adc_mean") / (1e3 * e.get("mc_dEdx_Birks"));

        get(h, "mc_adc_per_dEdx")->Fill(adc_per_dEdx);

        get(h, "reco_n_tracks")->Fill(e.get("reco_n_tracks"));

      

        // * fill histograms with information from all tracks:

        // for (int n_track = 1; n_track <= e.n_tracks_min(); ++n_track)

        // * fill histograms with information from first preselected reco track:

        //    * not a good idea since it implies reco preselection

        // int n_track = e.first_preselected_reco_track();

        // if (n_track > 0)

        // * simply use the first track

        {

          int n_track = 1;

        

          get(h, "reco_dplane_x")->Fill(e.get("dplane_x", n_track));

          get(h, "reco_dplane_y")->Fill(e.get("dplane_y", n_track));

          double reco_length = e.get("length", n_track) / 100;

          get(h, "reco_length")->Fill(reco_length);

          get(h, "reco_n_hits_all")->Fill(e.get("n_hits", n_track));

          get(h, "reco_n_hits_x")->Fill(e.get("n_hits_x", n_track));

          get(h, "reco_n_hits_y")->Fill(e.get("n_hits_y", n_track));

          get(h, "reco_track_1_theta_x")->Fill(e.theta(e.get("dxdz", n_track)));

          get(h, "reco_track_1_theta_y")->Fill(e.theta(e.get("dydz", n_track)));

        }

        if (mc_length > 14.5 && mc_length < 15.5)

        {

          ++c.at("all");

          

          int n_track = e.first_preselected_reco_track();

          if (e.is_triggered())

          {

            ++c.at("ddt");

            if (!e.is_preselected_reco())

            {

              ++c.at("failed presel reco");

              if (e.n_tracks() == 0)

                ++c.at("no reco tracks");

              if (e.n_tracks() == 1)

              {

                // std::cout << "File " << *input_file_name

                //           << "\tEvent " << e.event() << std::endl;

                

                ++c.at("one reco track");

                if (e.get("length", 1) / 100.0 < 10.0)

                  ++c.at("length < 10 m");

                // std::cout << e.get("n_hits_x", 1) << ", "

                //           << e.get("n_hits_y", 1) << ", "

                //           << e.get("dplane_x", 1) << ", "

                //           << e.get("dplane_y", 1) << ", "

                //           << e.get("length", 1) / 100.0

                //           << std::endl;

              }

            }

          }

        }

          

          // if (e.is_triggered() &&

          //     !e.is_preselected_reco())

          // {

          //   ++c.at("failed_reco_presel");

            

            // double r2_xt = e.get("r2_xt", n_track);

            // double r2_yt = e.get("r2_yt", n_track);

            // double mc_theta_x = e.mc_theta("xz");

            // double mc_theta_y = e.mc_theta("yz");

            // double non_mc_hits =

            //   e.get("n_hits_x", n_track) + e.get("n_hits_y", n_track)

            //   - e.get("n_mc_hits", n_track);

            

            // if (r2_xt < Constants::R2_MIN_CUT &&

            //         r2_yt < Constants::R2_MIN_CUT)            

              // std::cout << "MF: "<< std::boolalpha

              //                 << e.event() << ", "

              //                 << mc_length << ", "

              //                 << e.mc_fraction() << ", "

              //                 << e.is_triggered() << ", "

              //                 << e.is_preselected_mc() << ", "

              //                 << e.is_preselected_reco() << ", "

              //                 << n_track << ", "

              //                 << e.is_linear() << ", "

              //                 // << e.r2_min() << ", "

              //                 // << e.get("r2_xt", n_track) << ", "

              //                 // << e.get("r2_yt", n_track) << ", "

              //                 // << e.gap_max() << ", "

              //                 // << e.is_slow() << ", "

              //                 << e.get("mc_dx") << ", "

              //                 << e.get("mc_dy") << ", "

              //                 << e.get("mc_dz") << ", "

              //                 << mc_theta_x << ", "

              //                 << mc_theta_y << ", "

              //                 << e.get("mc_n_hits_x") << ", "

              //                 << e.get("mc_n_hits_y") << ", "

              //                 // << e.get("n_hits_x", n_track) << ", "

              //                 // << e.get("n_hits_y", n_track) << ", "

              //                 // << e.get("n_mc_hits", n_track) << ", "

              //                 // << non_mc_hits << ", "

              //                 // << e.beta()

              //                 << std::endl;

        //   }

        // }

          

        if (e.is_triggered())

        {

          get(h, "mc_dplane_x_ddt")->Fill(e.get("mc_dplane_x"));

          get(h, "mc_dplane_y_ddt")->Fill(e.get("mc_dplane_y"));

          get(h, "mc_length_ddt")->Fill(mc_length);

          get(h, "mc_n_hits_all_ddt")->Fill(e.get("mc_n_hits_all"));

          get(h, "mc_n_hits_x_ddt")->Fill(e.get("mc_n_hits_x"));

          get(h, "mc_n_hits_y_ddt")->Fill(e.get("mc_n_hits_y"));

          // get(h, "mc_n_hits_ge_100_ADC_ddt")

          //   ->Fill(e.get("mc_n_hits_ge_100_ADC"));

          // get(h, "mc_n_hits_ge_100_ADC_fraction_ddt")

          //   ->Fill(ge_100_ADC_fraction);

          

          get(h, "mc_n_tracks_ddt")->Fill(e.hit_detector());

          get(h, "mc_theta_x_ddt")->Fill(e.mc_theta("xz"));

          get(h, "mc_theta_y_ddt")->Fill(e.mc_theta("yz"));

        }

        // if (e.high_mc_fraction())

        if (e.is_triggered() &&

            e.is_linear()    &&

            e.is_slow())

        {

          get(h, "mc_dplane_x_reco")->Fill(e.get("mc_dplane_x"));

          get(h, "mc_dplane_y_reco")->Fill(e.get("mc_dplane_y"));

          get(h, "mc_length_reco")->Fill(mc_length);

          get(h, "mc_n_hits_all_reco")->Fill(e.get("mc_n_hits_all"));

          get(h, "mc_n_hits_x_reco")->Fill(e.get("mc_n_hits_x"));

          get(h, "mc_n_hits_y_reco")->Fill(e.get("mc_n_hits_y"));

          // get(h, "mc_n_hits_ge_100_ADC_reco")

          //   ->Fill(e.get("mc_n_hits_ge_100_ADC"));

          // get(h, "mc_n_hits_ge_100_ADC_fraction_reco")

          //   ->Fill(ge_100_ADC_fraction);

          

          get(h, "mc_n_tracks_reco")->Fill(e.hit_detector());

          get(h, "mc_theta_x_reco")->Fill(e.mc_theta("xz"));

          get(h, "mc_theta_y_reco")->Fill(e.mc_theta("yz"));

        }

        if (e.is_linear())

          get(h, "mc_fraction_linear")->Fill(e.mc_fraction());

        

        double best_mc_fraction = 0;

        unsigned best_track = 0;

        for (unsigned n = 1; n <= Constants::MAX_RECO_N_TRACKS; ++n)

        {

          if (e.mc_fraction(n) > best_mc_fraction)

          {

            best_mc_fraction = e.mc_fraction(n);

            best_track = n;

          }

        }

      

        get(h, "best_reco_track")->Fill(best_track);

      }

    }

  }

  for (auto const& count : c)

    std::cout << std::setw(20) << count.first

              << std::setw(8) << count.second

              << std::endl;

}

/////////////////////////////////////////////////////////////////////////////

//

// Draw Histograms

//

/////////////////////////////////////////////////////////////////////////////

void draw_beta(hist_t const& h)

{

  TH1* mc = get(h, "mc_beta");

  TH1* reco = get(h, "reco_track_1_beta");

  TAxis *x = mc->GetXaxis();

  TAxis *y = mc->GetYaxis();

  mc->SetLineColor(kRed);

  

  x->SetTitle("#beta");

  x->SetTitleOffset(1.2);

  // x->CenterTitle();

  // y->SetTitle("Number of Events");

  // y->CenterTitle();

  cans.new_canvas("Beta");

  mc->Draw();

  reco->Draw("SAMES");

}

void draw_dEdx(hist_t const& h)

{

  gStyle->SetOptStat("emroui");

  cans.new_canvas("mc_adc_mean");

  get(h, "mc_adc_mean")->Draw();

  get(h, "mc_adc_mean")->GetXaxis()->SetTitle("ADC_{avg}");

  get(h, "mc_adc_mean")->GetYaxis()->SetTitle("Number of Events");

  get(h, "mc_adc_mean")->GetYaxis()->CenterTitle();

  cans.new_canvas("mc_dEdx");

  get(h, "mc_dEdx")->Draw();

  get(h, "mc_dEdx")->GetXaxis()->SetTitle("dE/dx [MeV/cm]");

  get(h, "mc_dEdx")->GetYaxis()->SetTitle("Number of Events");

  get(h, "mc_dEdx")->GetYaxis()->CenterTitle();

  

  cans.new_canvas("mc_dEdx_Birks");

  get(h, "mc_dEdx_Birks")->Draw();

  get(h, "mc_dEdx_Birks")->GetXaxis()->SetTitle("dE/dx (Birks) [MeV/cm]");

  get(h, "mc_dEdx_Birks")->GetYaxis()->SetTitle("Number of Events");

  get(h, "mc_dEdx_Birks")->GetYaxis()->CenterTitle();

  cans.new_canvas("mc_adc_per_dEdx");

  get(h, "mc_adc_per_dEdx")->Draw();

  get(h, "mc_adc_per_dEdx")->GetXaxis()->SetTitle("ADC per MeV/cm");

  get(h, "mc_adc_per_dEdx")->GetYaxis()->SetTitle("Number of Events");

  get(h, "mc_adc_per_dEdx")->GetYaxis()->CenterTitle();

}

void draw_dplane(hist_t const& h, std::string view)

{

  TH1* mc      = get(h, "mc_dplane_" + view);

  TH1* mc_reco = get(h, "mc_dplane_" + view + "_reco");

  TH1* mc_ddt  = get(h, "mc_dplane_" + view + "_ddt");

  TH1* reco    = get(h, "reco_dplane_" + view);

  TAxis *x = mc->GetXaxis();

  TAxis *y = mc->GetYaxis();

  TH1* mc_ddt_copy = dynamic_cast<TH1*>(mc_ddt->Clone("mc_ddt_copy"));

  make_nice(mc, mc_reco, reco, mc_ddt);

  std::string axis_name = "#DeltaPlane (xz View)";

  std::string canvas_name = "DPlane_XZ";

  if (view == "y")

  {

    axis_name = "#DeltaPlane (yz View)";

    canvas_name = "DPlane_YZ";

  }

  

  x->SetTitle(axis_name.c_str());

  // x->SetTitleOffset(1.2);

  // x->SetRangeUser(0, 100);

  x->CenterTitle();

  

  y->SetTitle("Number of Events");

  // y->SetTitleOffset(1.2);

  y->CenterTitle();

  y->SetRangeUser(0, 1.2 * mc->GetMaximum());

  mc_ddt_copy->GetXaxis()->SetTitle(axis_name.c_str());

  mc_ddt_copy->GetXaxis()->CenterTitle();

  mc_ddt_copy->GetYaxis()->SetTitle("Number of Events");

  mc_ddt_copy->GetYaxis()->CenterTitle();

  mc_ddt_copy->GetYaxis()->SetRangeUser(0, 1.2 * mc_ddt_copy->GetMaximum());

  

  cans.new_canvas(canvas_name);

  mc->Draw("");

  mc_reco->Draw("SAME");

  mc->Draw("SAME AXIS");

  // reco->Draw("SAME");

  nova::Simulation();

  TLegend *l = new TLegend(0.5, 0.7, 0.88, 0.88);

  l->SetTextSizePixels(20);

  std::string mc_entry =

    "Monopole MC (#beta = " + Constants::MC_BETA_NICE_NAME + ")";

  l->AddEntry(mc, mc_entry.c_str(), "l");

  l->AddEntry(mc_reco, "Pass Signal Selection", "f");

  l->Draw();

  cans.new_canvas("DDT_" + canvas_name);

  mc->Draw("");

  mc_ddt->Draw("SAME");

  mc->Draw("SAME AXIS");

  // reco->Draw("SAME");

  nova::Simulation();

  TLegend *l_ddt = new TLegend(0.5, 0.7, 0.88, 0.88);

  l_ddt->SetTextSizePixels(20);

  l_ddt->AddEntry(mc, mc_entry.c_str(), "l");

  l_ddt->AddEntry(mc_ddt, "Pass Trigger", "f");

  l_ddt->Draw();

  cans.new_canvas("Signal_and_DDT_" + canvas_name);

  mc_ddt_copy->Draw("");

  mc_reco->Draw("SAME");

  mc_ddt_copy->Draw("SAME AXIS");

  nova::Simulation();

  

  TLegend *l_s_ddt = new TLegend(0.5, 0.7, 0.88, 0.88);

  l_s_ddt->SetTextSizePixels(20);

  l_s_ddt->SetHeader(mc_entry.c_str());

  l_s_ddt->AddEntry(mc_ddt_copy, "Pass Trigger", "l");

  l_s_ddt->AddEntry(mc_reco, "Pass Signal Selection", "f");

  l_s_ddt->Draw();

}

void draw_dplane_eff(hist_t const& h, std::string view, std::string type)

{

  TH1* eff = MFRoot::divide_with_poisson_errors

    (get(h, "mc_dplane_" + view + "_" + type),

     get(h, "mc_dplane_" + view),

     "eff_dplane_" + view);

  TAxis *x = eff->GetXaxis();

  TAxis *y = eff->GetYaxis();

  eff->SetLineWidth(2);

  eff->SetLineColor(kBlack);

  eff->SetFillColor(kWhite);

  

  std::string axis_name = "#DeltaPlane (xz View)";

  std::string canvas_name = type + "_DPlane_XZ_Efficiency";

  if (view == "y")

  {

    axis_name = "#DeltaPlane (yz View)";

    canvas_name = type + "_DPlane_YZ_Efficiency";

  }

  

  x->SetTitle(axis_name.c_str());

  x->CenterTitle();

  

  y->SetTitle("Efficiency");

  y->CenterTitle();

  y->SetRangeUser(0, 1.1);

  cans.new_canvas(canvas_name);

  eff->Draw("HIST");

  nova::Simulation();

}

void draw_dxdz(hist_t const& h)

{

  TH1* mc = get(h, "mc_dxdz");

  TH1* reco = get(h, "reco_track_1_dxdz");

  TAxis *x = mc->GetXaxis();

  TAxis *y = mc->GetYaxis();

  mc->SetLineColor(kRed);

  

  x->SetTitle("dx/dz");

  x->SetTitleOffset(1.2);

  // y->SetTitle("Number of Events");

  // y->CenterTitle();

  cans.new_canvas("dxdz");

  mc->Draw();

  reco->Draw("SAMES");

  cans.new_canvas("dxdz log", false, true);

  mc->Draw();

  reco->Draw("SAMES");

}

void draw_dydz(hist_t const& h)

{

  TH1* mc = get(h, "mc_dydz");

  TH1* reco = get(h, "reco_track_1_dydz");

  TAxis *x = mc->GetXaxis();

  TAxis *y = mc->GetYaxis();

  mc->SetLineColor(kRed);

  

  x->SetTitle("dy/dz");

  x->SetTitleOffset(1.2);

  // y->SetTitle("Number of Events");

  // y->CenterTitle();

  cans.new_canvas("dydz");

  mc->Draw();

  reco->Draw("SAMES");

  cans.new_canvas("dydz log", false, true);

  mc->Draw();

  reco->Draw("SAMES");

}

void draw_length(hist_t const& h)

{

  TH1* mc      = get(h, "mc_length");

  TH1* mc_reco = get(h, "mc_length_reco");

  TH1* mc_ddt  = get(h, "mc_length_ddt");

  TH1* reco    = get(h, "reco_length");

  TAxis *x = mc->GetXaxis();

  TAxis *y = mc->GetYaxis();

  TH1* mc_ddt_copy = dynamic_cast<TH1*>(mc_ddt->Clone("mc_ddt_copy"));

  make_nice(mc, mc_reco, reco, mc_ddt);

  

  x->SetTitle("Track Length [m]");

  // x->SetTitleSize(0.03);

  // x->SetLabelSize(0.03);

  // x->SetTitleOffset(1.2);

  x->CenterTitle();

  y->SetTitle("Number of Events");

  // y->SetTitleSize(0.03);

  // y->SetLabelSize(0.03);

  // y->SetTitleOffset(1.65);

  y->CenterTitle();

  y->SetRangeUser(0, 1.1 * mc->GetMaximum());

  mc_ddt_copy->GetXaxis()->SetTitle("Track Length [m]");

  mc_ddt_copy->GetXaxis()->CenterTitle();

  mc_ddt_copy->GetYaxis()->SetTitle("Number of Events");

  mc_ddt_copy->GetYaxis()->CenterTitle();

  mc_ddt_copy->GetYaxis()->SetRangeUser(0, 1.1 * mc_ddt_copy->GetMaximum());

  

  cans.new_canvas("Length");

  mc->Draw("");

  mc_reco->Draw("SAME");

  mc->Draw("SAME AXIS");

  

  nova::Simulation();

  

  TLegend *l = new TLegend(0.5, 0.7, 0.88, 0.88);

  l->SetTextSizePixels(20);

  std::string mc_entry =

    "Monopole MC (#beta = " + Constants::MC_BETA_NICE_NAME + ")";

  l->AddEntry(mc, mc_entry.c_str(), "l");

  l->AddEntry(mc_reco, "Pass Signal Selection", "f");

  l->Draw();

  cans.new_canvas("DDT_Length");

  mc->Draw("");

  mc_ddt->Draw("SAME");

  mc->Draw("SAME AXIS");

  // reco->Draw("SAME");

  nova::Simulation();

  

  TLegend *l_ddt = new TLegend(0.5, 0.7, 0.88, 0.88);

  l_ddt->SetTextSizePixels(20);

  l_ddt->AddEntry(mc, mc_entry.c_str(), "l");

  l_ddt->AddEntry(mc_ddt, "Pass Trigger", "f");

  l_ddt->Draw();

  cans.new_canvas("Signal_and_DDT_Length");

  mc_ddt_copy->Draw("");

  mc_reco->Draw("SAME");

  mc_ddt_copy->Draw("SAME AXIS");

  nova::Simulation();

  

  TLegend *l_s_ddt = new TLegend(0.4, 0.7, 0.88, 0.88);

  l_s_ddt->SetTextSizePixels(20);

  l_s_ddt->SetHeader(mc_entry.c_str());

  l_s_ddt->AddEntry(mc_ddt_copy, "Pass Trigger", "l");

  l_s_ddt->AddEntry(mc_reco, "Pass Signal Selection", "f");

  l_s_ddt->Draw();

}

void draw_length_eff(hist_t const& h, std::string type)

{

  TH1* eff =

    MFRoot::divide_with_poisson_errors

    (get(h, "mc_length_" + type), get(h, "mc_length"), "eff_length");

  TAxis *x = eff->GetXaxis();

  TAxis *y = eff->GetYaxis();

  eff->SetLineWidth(2);

  eff->SetLineColor(kBlack);

  eff->SetFillColor(kWhite);

  eff->SetMarkerStyle(kFullTriangleUp);

  eff->Scale(100.0);

  

  x->SetTitle("Track Length [m]");

  x->CenterTitle();

  y->SetTitle("Efficiency [%]");

  y->CenterTitle();

  y->SetRangeUser(0, 105);

  

  cans.new_canvas(type + "_Length_Efficiency");

  eff->Draw("HIST");

  nova::Simulation();

  TLegend *l = new TLegend(0.4, 0.7, 0.88, 0.88);

  l->SetTextSizePixels(20);

  std::string mc_entry =

    "Monopole MC (#beta = " + Constants::MC_BETA_NICE_NAME + ")";

  l->SetHeader(mc_entry.c_str());

  // l->AddEntry(mc_ddt_copy, "Pass Requirements 1. - 3.", "l");

  // l->AddEntry(mc_reco, "Pass Requirements 1. - 6.", "f");

  // l->Draw();

}

void draw_length_comparison(hist_t const& h)

{

  TH1* mc = get(h, "mc_length");

  TH1* reco = get(h, "reco_length");

  TAxis *x = mc->GetXaxis();

  TAxis *y = mc->GetYaxis();

  mc->SetLineWidth(2);

  reco->SetMarkerSize(1.5);

  reco->SetMarkerColor(kGreen + 1);

  reco->SetMarkerStyle(kFullTriangleUp);

  

  x->SetTitle("Track Length [m]");

  x->SetTitleOffset(1.2);

  // y->SetTitle("Number of Events / 2 m");

  // y->CenterTitle();

  cans.new_canvas("length_comparison");

  mc->Draw("");

  reco->Draw("P SAMES");

  

  TLegend *l = new TLegend(0.5, 0.75, 0.88, 0.88);

  l->SetTextSizePixels(16);

  l->AddEntry

    (mc, Form("MC (N_{events} = %.0f)", mc->GetEntries()), "l");

  l->AddEntry

    (reco, Form("Reconstructed (N_{events} = %.0f)", reco->GetEntries()), "p");

  l->Draw();

}

void draw_mc_fraction(hist_t const& h, std::string hist_name)

{

  TH1* mc = get(h, hist_name);

  TAxis *x = mc->GetXaxis();

  TAxis *y = mc->GetYaxis();

  std::string canvas_name = "MC_Fraction";

  if (hist_name == "mc_fraction_linear")

    canvas_name = "MC_Fraction_Linear";

  x->SetTitle("MC Fraction");

  x->SetTitleOffset(1.2);

  // y->SetTitle("Number of Events");

  // y->SetTitleOffset(1.2);

  // y->CenterTitle();

  // y->SetRangeUser(0, 100);

  cans.new_canvas(canvas_name);

  mc->Draw();

}

void draw_n_tracks(hist_t const& h)

{

  TH1* mc = get(h, "mc_n_tracks");

  TH1* mc_reco = get(h, "mc_n_tracks_reco");

  TH1* reco = get(h, "reco_n_tracks");

  TAxis *x = mc->GetXaxis();

  TAxis *y = mc->GetYaxis();

  make_nice(mc, mc_reco, reco);

  

  x->SetTitle("Number of Tracks");

  x->SetTitleOffset(1.2);

  // y->SetTitle("Number of Events / 2 m");

  // y->SetTitleOffset(1.2);

  // y->CenterTitle();

  y->SetRangeUser(0, 1.1 * mc->GetMaximum());

  cans.new_canvas("Number of Tracks");

  mc->Draw("");

  mc_reco->Draw("SAMES");

  mc->Draw("SAME AXIS");

  reco->Draw("SAMES");

}

void draw_best_track(hist_t const& h)

{

  TH1* hist = get(h, "best_reco_track");

  TAxis *x = hist->GetXaxis();

  TAxis *y = hist->GetYaxis();

  x->SetTitle("Best Reco Track Number");

  x->SetTitleOffset(1.2);

  x->SetNdivisions(4, 0, 0);

  // y->SetTitle("Number of Events");

  // y->SetTitleOffset(1.2);

  // y->CenterTitle();

  cans.new_canvas("Best Reco Track");

  hist->Draw();

}

void draw_n_hits(hist_t const& h, std::string label)

{

  TH1* mc      = get(h, "mc_n_hits_" + label);

  TH1* mc_reco = get(h, "mc_n_hits_" + label + "_reco");

  TH1* reco    = get(h, "reco_n_hits_" + label);

  TAxis *x = mc->GetXaxis();

  TAxis *y = mc->GetYaxis();

  make_nice(mc, mc_reco, reco);

  std::string axis_name;

  std::string canvas_name;

  if (label == "x")

  {

    axis_name = "Number of Hits (xz View)";

    canvas_name = "Number_Hits_XZ";

  } else if (label == "y") {

    axis_name = "Number of Hits (yz View)";

    canvas_name = "Number_Hits_YZ";

  } else if (label == "all") {

    axis_name = "Number of Hits (Both Views)";

    canvas_name = "Number_Hits_All";

  } else if (label == "ge_100_ADC") {

    axis_name = "Number of Hits (ADC #geq 100 ADC)";

    canvas_name = "Number_Hits_100_ADC";

  } else if (label == "ge_100_ADC_fraction") {

    axis_name = "Fraction of MC Hits (ADC #geq 100 ADC)";

    canvas_name = "Number_Hits_100_ADC_Fraction";

  }    

  

  x->SetTitle(axis_name.c_str());

  // x->SetTitleOffset(1.2);

  x->CenterTitle();

  y->SetTitle("Number of Events");

  // y->SetTitleOffset(1.2);

  y->CenterTitle();

  y->SetRangeUser(0, 1.1 * mc->GetMaximum());

  // y->SetRangeUser(0, 60);

                  

  cans.new_canvas(canvas_name);

  mc->Draw("");

  mc_reco->Draw("SAME");

  mc->Draw("SAME AXIS");

  // reco->Draw("SAME");

  nova::Simulation();

  TLegend *l = new TLegend(0.45, 0.7, 0.88, 0.88);

  l->SetTextSizePixels(20);

  std::string mc_entry =

    "Monopole MC (#beta = " + Constants::MC_BETA_NICE_NAME + ")";

  l->AddEntry(mc, mc_entry.c_str(), "l");

  l->AddEntry(mc_reco, "Pass Signal Selection", "f");

  l->Draw();

}

void draw_theta(hist_t const& h, std::string view)

{

  TH1* mc      = get(h, "mc_theta_" + view);

  TH1* mc_reco = get(h, "mc_theta_" + view + "_reco");

  TH1* mc_ddt  = get(h, "mc_theta_" + view + "_ddt");

  TH1* reco    = get(h, "reco_track_1_theta_" + view);

  TAxis *x = mc->GetXaxis();

  TAxis *y = mc->GetYaxis();

  TH1* mc_ddt_copy = dynamic_cast<TH1*>(mc_ddt->Clone("mc_ddt_copy"));

  

  make_nice(mc, mc_reco, reco, mc_ddt);

  

  std::string axis_name = "#theta_{x}";

  std::string canvas_name = "Theta_X";

  if (view == "y")

  {

    axis_name = "#theta_{Y}";

    canvas_name = "Theta_Y";

  }

  

  x->SetTitle(axis_name.c_str());

  // x->SetTitleOffset(1.2);

  x->CenterTitle();

  x->SetNdivisions(20210);

  y->SetTitle("Number of Events");

  // y->SetTitleOffset(1.2);

  y->CenterTitle();

  y->SetRangeUser(0, 1.4 * mc->GetMaximum());

  mc_ddt_copy->GetXaxis()->SetTitle(axis_name.c_str());

  mc_ddt_copy->GetXaxis()->CenterTitle();

  mc_ddt_copy->GetYaxis()->SetTitle("Number of Events");

  mc_ddt_copy->GetYaxis()->CenterTitle();

  mc_ddt_copy->GetYaxis()->SetRangeUser(0, 1.4 * mc_ddt_copy->GetMaximum());

                  

  cans.new_canvas(canvas_name);

  mc->Draw("");

  mc_reco->Draw("SAME");

  mc->Draw("SAME AXIS");

  // reco->Draw("SAME");

  nova::Simulation();

  TLegend *l = new TLegend(0.5, 0.7, 0.88, 0.88);

  l->SetTextSizePixels(20);

  std::string mc_entry =

    "Monopole MC (#beta = " + Constants::MC_BETA_NICE_NAME + ")";

  l->AddEntry(mc, mc_entry.c_str(), "l");

  l->AddEntry(mc_reco, "Pass Signal Selection", "f");

  l->Draw();

  cans.new_canvas("DDT_" + canvas_name);

  mc->Draw("");

  mc_ddt->Draw("SAME");

  mc->Draw("SAME AXIS");

  // reco->Draw("SAME");

  nova::Simulation();

  TLegend *l_ddt = new TLegend(0.5, 0.7, 0.88, 0.88);

  l_ddt->SetTextSizePixels(20);

  l_ddt->AddEntry(mc, mc_entry.c_str(), "l");

  l_ddt->AddEntry(mc_ddt, "Pass Trigger", "f");

  l_ddt->Draw();

  cans.new_canvas("Signal_and_DDT_" + canvas_name);

  mc_ddt_copy->Draw("");

  mc_reco->Draw("SAME");

  mc_ddt_copy->Draw("SAME AXIS");

  nova::Simulation();

  

  TLegend *l_s_ddt = new TLegend(0.5, 0.7, 0.88, 0.88);

  l_s_ddt->SetTextSizePixels(20);

  l_s_ddt->SetHeader(mc_entry.c_str());

  l_s_ddt->AddEntry(mc_ddt_copy, "Pass Trigger", "l");

  l_s_ddt->AddEntry(mc_reco, "Pass Signal Selection", "f");

  l_s_ddt->Draw();

}

void draw_theta_eff(hist_t const& h, std::string view, std::string type)

{

  TH1 *eff = MFRoot::divide_with_poisson_errors

    (get(h, "mc_theta_" + view + "_" + type),

     get(h, "mc_theta_" + view),

     "eff_view_" + view);

  TAxis *x = eff->GetXaxis();

  TAxis *y = eff->GetYaxis();

  eff->SetLineWidth(2);

  eff->SetLineColor(kBlack);

  eff->SetFillColor(kWhite);

  

  std::string axis_name = "#theta_{x}";

  std::string canvas_name = type + "_Theta_X_Efficiency";

  if (view == "y")

  {

    axis_name = "#theta_{Y}";

    canvas_name = type + "_Theta_Y_Efficiency";

  }

  x->SetTitle(axis_name.c_str());

  x->CenterTitle();

  x->SetNdivisions(20210);

  y->SetTitle("Efficiency");

  y->CenterTitle();

  y->SetRangeUser(0, 1.1);

                  

  cans.new_canvas(canvas_name);

  eff->Draw("HIST");

  nova::Simulation();

}

void make_nice(TH1* mc, TH1* mc_reco, TH1* reco, TH1* mc_ddt)

{

  mc->SetLineWidth(2);

  mc->SetLineColor(kBlack);

  mc_reco->SetLineWidth(1);

  // mc_reco->SetLineColor(kGreen + 1);

  mc_reco->SetFillColor(kGreen + 1);

  // mc_reco->SetLineColor(kGray + 1);

  // mc_reco->SetFillColor(kGray + 1);

  // mc_reco->SetFillStyle(3013);

  if (mc_ddt)

  {

    mc_ddt->SetLineWidth(1);

    mc_ddt->SetFillColor(kBlue);

  }

  

  reco->SetLineColor(kRed);

  reco->SetLineStyle(2);

}

