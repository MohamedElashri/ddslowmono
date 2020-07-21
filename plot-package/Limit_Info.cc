#include "Limit_Info.hh"

#include "Constants.hh"

#include <cmath>

#include <iomanip>

#include <iostream>

Limit_Info::Limit_Info(double beta) :

  area_(0.0),

  beta_(beta),

  n_(0.0),

  n_signal_(0.0)

{

}

void Limit_Info::add_area(double area)

{

  area_ += area;

}

double Limit_Info::beta() const

{

  return beta_;

}

double Limit_Info::efficiency() const

{

  return n_signal_ / n_;

}

void Limit_Info::increment_counts(bool is_signal)

{

  ++n_;

  if (is_signal)

    ++n_signal_;

}

double Limit_Info::limit(std::string coverage) const

{

  double result = 0.0;

  double omega = M_PI;

  if (coverage == "half")

    omega *= 2;

  else if (coverage == "full")

    omega *= 4;

  else

    std::cerr << "Coverage value (" << coverage << ") is not valid."

              << std::endl;

  double T = Constants::LIVE_TIME_DDT_CORRECTED;

  if (projected_area_total() > 0)

  {

    // convert area to cm^2

    double A_cm2 = projected_area_total() * 1e4;

    double eA    = A_cm2 / n_monopoles_total();

    result       = 2.3 / (omega * T * eA);

  }

  return result;

}

double Limit_Info::log_beta() const

{

  return std::log10(beta_);

}

double Limit_Info::n_monopoles_total() const

{

  return n_;

}

double Limit_Info::n_monopoles_signal() const

{

  return n_signal_;

}

void Limit_Info::print() const

{

  std::cout << std::setw(12) << log_beta() << ", "

            << std::setw(12) << efficiency() << ", "

            << std::setw(12) << limit("half") << ", "

            << std::setw(12) << limit("full")

            << std::endl;

}

double Limit_Info::projected_area_total() const

{

  return area_;

}

