#include "credit.h"

#include <array>

namespace s21 {

std::vector<std::array<double, 4>> CreditModel::DifferentiatedLoan(
    int period_count, double loan, double annual_perc) {
  double principal = loan / period_count;
  double percent = 0;
  double overpay = 0;
  double monthly_perc = annual_perc / (12 * 100);
  std::vector<std::array<double, 4>> rows;

  for (int i = 0; i < period_count - 1; ++i) {
    percent = loan * monthly_perc;
    overpay += percent;
    loan -= principal;
    rows.push_back({principal + percent, principal, percent, loan});
  }

  percent = loan * monthly_perc;
  overpay += percent;
  rows.push_back({loan + percent, loan, percent, 0});

  rows.push_back({loan + overpay, loan, overpay, 0});
  return rows;
}

std::vector<std::array<double, 4>> CreditModel::AnnuityLoan(
    int period_count, double loan, double annual_perc) {
  double principal = 0;
  double percent = 0;
  double overpay = 0;
  double base_loan = loan;
  double monthly_perc = annual_perc / (12 * 100);
  double annuity_payment =
      base_loan *
      (monthly_perc +
       (monthly_perc) / (SimplePow(1 + monthly_perc, period_count - 1) - 1));
  std::vector<std::array<double, 4>> rows;

  for (int i = 0; i < period_count - 1; ++i) {
    percent = loan * monthly_perc;
    overpay += percent;
    principal = annuity_payment - percent;
    loan -= principal;
    rows.push_back({principal + percent, principal, percent, loan});
  }

  percent = loan * monthly_perc;
  overpay += percent;
  rows.push_back({loan + percent, loan, percent, 0});

  rows.push_back({loan + overpay, loan, overpay, 0});
  return rows;
}

double CreditModel::SimplePow(double value, int pow) {
  double mult = value;
  for (int i = 0; i < pow; ++i) {
    value *= mult;
  }
  return value;
}

};  // namespace s21
