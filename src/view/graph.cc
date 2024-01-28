#include "graph.h"

#include <QDoubleSpinBox>
#include <QLabel>
#include <QSpinBox>
#include <QString>
#include <QVBoxLayout>
#include <QVector>
#include <QWidget>

namespace s21 {

NumberLimit::NumberLimit(const QString &label_text, double low, double high,
                         double default_low, double default_high,
                         QWidget *parent)
    : QWidget(parent) {
  QVBoxLayout *layout = new QVBoxLayout(this);

  label_ = new QLabel(label_text, this);
  label_->setAlignment(Qt::AlignHCenter);
  label_->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Maximum);
  layout->addWidget(label_);

  low_ = new QDoubleSpinBox(this);
  low_->setRange(low, high);
  low_->setValue(default_low);
  layout->addWidget(low_);

  high_ = new QDoubleSpinBox(this);
  high_->setRange(low, high);
  high_->setValue(default_high);
  layout->addWidget(high_);

  QObject::connect(low_, &QDoubleSpinBox::textChanged, this,
                   &NumberLimit::textChanged);
  QObject::connect(high_, &QDoubleSpinBox::textChanged, this,
                   &NumberLimit::textChanged);
}

double NumberLimit::Low() { return low_->value(); }
double NumberLimit::High() { return high_->value(); }

Graph::Graph(Controller &controller, QWidget *parent)
    : QWidget(parent), controller_(controller) {
  main_vbox_ = new QVBoxLayout(this);

  InitQCustomPlot();
  InitLimits();
  InitPointsBox();
  PlaceItems();

  setMaximumSize(800, 600);
  setMinimumSize(600, 400);
}

void Graph::InitQCustomPlot() {
  plot_ = new QCustomPlot(this);
  plot_->addGraph();
  plot_->graph(0)->setPen(QPen(Qt::blue));
  connect(plot_->xAxis, SIGNAL(rangeChanged(QCPRange)), plot_->xAxis2,
          SLOT(setRange(QCPRange)));
  connect(plot_->yAxis, SIGNAL(rangeChanged(QCPRange)), plot_->yAxis2,
          SLOT(setRange(QCPRange)));
  plot_->xAxis2->setVisible(true);
  plot_->xAxis2->setTickLabels(false);
  plot_->yAxis2->setVisible(true);
  plot_->yAxis2->setTickLabels(false);
  plot_->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom |
                         QCP::iSelectPlottables);
}

void Graph::InitLimits() {
  limits_ = new QHBoxLayout(this);
  x_limits_ = new NumberLimit("x low, high", -1e6, 1e6, -100.0, 100.0, this);
  y_limits_ = new NumberLimit("y low, high", -1e6, 1e6, 0.0, 0.0, this);
  QObject::connect(x_limits_, &NumberLimit::textChanged, this,
                   &Graph::PlotFromMemory);
  QObject::connect(y_limits_, &NumberLimit::textChanged, this,
                   &Graph::PlotFromMemory);
}

void Graph::InitPointsBox() {
  points_vbox_ = new QVBoxLayout(this);
  points_label_ = new QLabel("points count", this);
  points_label_->setAlignment(Qt::AlignHCenter | Qt::AlignTop);
  points_label_->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Maximum);
  points_vbox_->addWidget(points_label_);
  points_ = new QSpinBox(this);
  points_->setRange(1, 1e5);
  points_->setValue(4000);
  points_vbox_->addWidget(points_);
  QObject::connect(points_, &QSpinBox::textChanged, this,
                   &Graph::PlotFromMemory);
}

void Graph::PlaceItems() {
  setLayout(main_vbox_);

  main_vbox_->addWidget(plot_);
  main_vbox_->addLayout(limits_);

  limits_->addWidget(x_limits_);
  limits_->addWidget(y_limits_);
  limits_->addLayout(points_vbox_);
}

void Graph::ToggleVisibility() {
  setVisible(isHidden());
  if (!isHidden() && pending_draw_) {
    emit PlotFromMemory();
    pending_draw_ = false;
  }
}

void Graph::PlotFromInput(const QString &input) {
  if (isHidden()) {
    pending_draw_ = true;
    expression_ = input;
    return;
  }
  try {
    std::pair<QVector<double>, QVector<double>> xy = controller_.Calculate(
        input, x_limits_->Low(), x_limits_->High(), y_limits_->Low(),
        y_limits_->High(), points_->value());
    expression_ = input;
    plot_->graph(0)->setData(xy.first, xy.second);
    plot_->replot();
  } catch (std::invalid_argument &e) {
  }
}

void Graph::PlotFromMemory() { emit PlotFromInput(expression_); }

};  // namespace s21
