#include <QApplication>

#include "controller/controller.h"
#include "model/calculator.h"
#include "view/view.h"

int main(int argc, char *argv[]) {
  QApplication a(argc, argv);
  s21::CalculatorModel model;

  s21::CalculatorModel calc;
  s21::CreditModel credit;
  s21::Controller controller(calc, credit);
  s21::View view(controller);
  view.show();
  return a.exec();
}
