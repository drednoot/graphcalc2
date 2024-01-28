#ifndef SMARTCALC_CONTROLLER_CONTROLLER_H_
#define SMARTCALC_CONTROLLER_CONTROLLER_H_

#include <QString>
#include <QVector>

#include "../model/calculator.h"
#include "../model/credit.h"

namespace s21 {

class Controller {
 public:
  Controller(CalculatorModel &calc, CreditModel &credit);

  bool isContainingX(const QString &input);
  double Calculate(const QString &input, double x);
  std::pair<QVector<double>, QVector<double>> Calculate(
      const QString &input, double low_x, double high_x, double low_y,
      double high_y, size_t points);
  QVector<QStringList> Loan(double amount, double term, double interest,
                            bool is_annuity);

 private:
  CalculatorModel &calc_;
  CreditModel &credit_;
};

};  // namespace s21

#endif  // SMARTCALC_CONTROLLER_CONTROLLER_H_
