#ifndef SMARTCALC_MODEL_CREDIT_H_
#define SMARTCALC_MODEL_CREDIT_H_

#include <array>
#include <vector>

namespace s21 {

class CreditModel {
 public:
  std::vector<std::array<double, 4>> DifferentiatedLoan(int preiod_count,
                                                        double loan,
                                                        double annual_perc);
  std::vector<std::array<double, 4>> AnnuityLoan(int preiod_count, double loan,
                                                 double annual_perc);

 private:
  double SimplePow(double value, int pow);
};

};  // namespace s21

#endif  // SMARTCALC_MODEL_CREDIT_H_
