#ifndef SMARTCALC_VIEW_CREDIT_H_
#define SMARTCALC_VIEW_CREDIT_H_

#include <QDoubleSpinBox>
#include <QFormLayout>
#include <QRadioButton>
#include <QSpinBox>
#include <QTableWidget>
#include <QVector>
#include <QWidget>

#include "../controller/controller.h"

namespace s21 {

class Credit : public QWidget {
  Q_OBJECT

 public:
  Credit(Controller &controller_, QWidget *parent = nullptr);
  void ToggleVisibility();
  void Calculate();

 private:
  Controller &controller_;

  QFormLayout *layout_;

  QDoubleSpinBox *loan_amount_;
  QSpinBox *loan_term_;
  QDoubleSpinBox *interest_;

  QRadioButton *annuity_rb_;
  QRadioButton *differentiated_rb_;

  QTableWidget *table_;
  QVector<QVector<QTableWidgetItem>> items_;
};
};  // namespace s21

#endif  // SMARTCALC_VIEW_CREDIT_H_
