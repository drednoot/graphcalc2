#ifndef SMARTCALC_VIEW_GRAPH_H_
#define SMARTCALC_VIEW_GRAPH_H_

#include <QDoubleSpinBox>
#include <QLabel>
#include <QSpinBox>
#include <QVBoxLayout>
#include <QWidget>

#include "../controller/controller.h"
#include "../qcustomplot/qcustomplot.h"

namespace s21 {

class NumberLimit : public QWidget {
  Q_OBJECT

 public:
  NumberLimit(const QString &label_text, double low, double high,
              double default_low, double default_high,
              QWidget *parent = nullptr);
  double Low();
  double High();

 private:
  QLabel *label_;
  QDoubleSpinBox *low_;
  QDoubleSpinBox *high_;

 signals:
  void textChanged();
};

class Graph : public QWidget {
  Q_OBJECT

 public:
  Graph(Controller &controller, QWidget *parent = nullptr);
  void ToggleVisibility();

 private:
  void InitQCustomPlot();
  void InitLimits();
  void InitPointsBox();
  void PlaceItems();

  QVBoxLayout *main_vbox_;

  QCustomPlot *plot_;
  QHBoxLayout *limits_;

  NumberLimit *x_limits_;
  NumberLimit *y_limits_;

  QVBoxLayout *points_vbox_;
  QLabel *points_label_;
  QSpinBox *points_;

  Controller &controller_;

  QString expression_;
  bool pending_draw_ = false;

 public slots:
  void PlotFromInput(const QString &input);
  void PlotFromMemory();
};
};  // namespace s21

#endif  // SMARTCALC_VIEW_GRAPH_H_
