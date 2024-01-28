#include "controller.h"

#include <QString>
#include <QVector>
#include <vector>

namespace s21 {

Controller::Controller(CalculatorModel &calc, CreditModel &credit)
    : calc_(calc), credit_(credit) {}

bool Controller::isContainingX(const QString &input) {
  if (input.isEmpty()) {
    return false;
  }
  return calc_.isContainingX(input.toStdString());
}

double Controller::Calculate(const QString &input, double x) {
  if (input.isEmpty()) {
    return 0;
  }
  return calc_.Calculate(input.toStdString(), x);
}

std::pair<QVector<double>, QVector<double>> Controller::Calculate(
    const QString &input, double low_x, double high_x, double low_y,
    double high_y, size_t points) {
  if (input.isEmpty()) {
    return std::pair<QVector<double>, QVector<double>>(QVector<double>(),
                                                       QVector<double>());
  }
  std::pair<std::vector<double>, std::vector<double>> pair = calc_.Calculate(
      input.toStdString(), low_x, high_x, low_y, high_y, points);
  return std::pair<QVector<double>, QVector<double>>(
      QVector<double>(pair.first.begin(), pair.first.end()),
      QVector<double>(pair.second.begin(), pair.second.end()));
}

QVector<QVector<QString>> Controller::Loan(double amount, double term,
                                           double interest, bool is_annuity) {
  std::vector<std::array<double, 4>> rows;
  if (is_annuity) {
    rows = credit_.AnnuityLoan(term, amount, interest);
  } else {
    rows = credit_.DifferentiatedLoan(term, amount, interest);
  }
  QVector<QVector<QString>> table(rows.size() + 1);
  for (size_t i = 0; i < rows.size() - 1; ++i) {
    table[i].resize(4);
    for (size_t j = 0; j < 4; ++j) {
      table[i][j] = QString::number(rows[i][j], 'f', 2);
    }
  }

  table[rows.size() - 1] = {"Paid in total", "Amount of debt paid",
                            "Amount of interest paid", "Loan remainder"};
  table[rows.size()].resize(4);
  for (size_t j = 0; j < 4; ++j) {
    table[rows.size()][j] = QString::number(rows[rows.size() - 1][j], 'f', 2);
  }

  return table;
}

};  // namespace s21
