#include "credit.h"

#include <QDoubleSpinBox>
#include <QFormLayout>
#include <QLabel>
#include <QPushButton>
#include <QRadioButton>
#include <QSpinBox>
#include <QString>
#include <QTableWidget>
#include <QVector>
#include <QWidget>

namespace s21 {

Credit::Credit(Controller &controller_, QWidget *parent)
    : QWidget(parent), controller_(controller_) {
  QLabel *loan_amount_label = new QLabel("Loan amount", this);
  QLabel *loan_term_label = new QLabel("Loan term (months)", this);
  QLabel *interest_label = new QLabel("Interest rate per year (%)", this);

  loan_amount_ = new QDoubleSpinBox();
  loan_amount_->setRange(0.01, 100000000);
  loan_amount_->setDecimals(2);
  loan_term_ = new QSpinBox(this);
  loan_term_->setRange(1, 1000);
  interest_ = new QDoubleSpinBox(this);
  interest_->setRange(0.01, 200);
  interest_->setDecimals(2);

  QLabel *loan_type_label = new QLabel("Loan type:", this);
  loan_type_label->setAlignment(Qt::AlignCenter);

  annuity_rb_ = new QRadioButton("Annuity", this);
  annuity_rb_->setChecked(true);
  differentiated_rb_ = new QRadioButton("Differentiated", this);

  QPushButton *calculate_button = new QPushButton("Calculate", this);
  QObject::connect(calculate_button, &QPushButton::pressed, this,
                   &Credit::Calculate);

  QStringList header = {"Total Payment", "Principal Payment",
                        "Interest Payment", "Loan Remainder"};
  table_ = new QTableWidget(1, 4, this);
  table_->setHorizontalHeaderLabels(header);
  table_->setVisible(false);

  layout_ = new QFormLayout(this);
  layout_->addRow(loan_amount_label, loan_amount_);
  layout_->addRow(loan_term_label, loan_term_);
  layout_->addRow(interest_label, interest_);
  layout_->addRow(loan_type_label);
  layout_->addRow(annuity_rb_, differentiated_rb_);
  layout_->addRow(calculate_button);
  setLayout(layout_);
}

void Credit::ToggleVisibility() { setVisible(isHidden()); }

void Credit::Calculate() {
  bool is_annuity = annuity_rb_->isChecked();
  QVector<QVector<QString>> table_data =
      controller_.Loan(loan_amount_->value(), loan_term_->value(),
                       interest_->value(), is_annuity);

  items_.clear();
  items_.resize(table_data.size());
  table_->setRowCount(table_data.size());

  for (qsizetype row = 0; row < table_data.size(); ++row) {
    items_[row].resize(4);
    for (size_t col = 0; col < 4; ++col) {
      items_[row][col] = QTableWidgetItem(table_data[row][col]);
      items_[row][col].setFlags(Qt::ItemIsSelectable | Qt::ItemIsUserCheckable |
                                Qt::ItemIsEnabled);
      table_->setItem(row, col, &items_[row][col]);
    }
  }

  if (table_->isHidden()) {
    table_->setVisible(true);
    layout_->addRow(table_);
  }
}

};  // namespace s21
