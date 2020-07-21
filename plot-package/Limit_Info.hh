#ifndef MF_LIMIT_INFO_MONO

#define MF_LIMIT_INFO_MONO

/*

  This class keeps track of the cumulative information needed to calculate

  the final 90% C.L. upper limits.

 */

class Limit_Info

{

public:

  Limit_Info() = default;

  Limit_Info(double beta);

  void   add_area(double area);

  double area() const;

  double beta() const;

  double efficiency() const;

  void   increment_counts(bool is_signal);

  double limit(std::string coverage) const;

  double log_beta() const;

  double n_monopoles_total() const;

  double n_monopoles_signal() const;

  void   print() const;

  double projected_area_total() const;

  

private:

  double area_;

  double beta_;

  double n_;

  double n_signal_;

};

#endif

