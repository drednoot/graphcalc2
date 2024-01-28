#ifndef SMARTCALC_VIEW_VIEW_H_
#define SMARTCALC_VIEW_VIEW_H_

#include <QAction>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QMenu>
#include <QMouseEvent>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>
#include <array>
#include <unordered_map>
#include <vector>

#include "../controller/controller.h"
#include "credit.h"
#include "graph.h"

namespace s21 {

enum NumberButtons {
  BUTTON_7,
  BUTTON_8,
  BUTTON_9,
  BUTTON_4,
  BUTTON_5,
  BUTTON_6,
  BUTTON_1,
  BUTTON_2,
  BUTTON_3,
  BUTTON_0,
  BUTTON_DOT,
  BUTTON_E,
};

enum ActionButtons {
  BUTTON_PLUS,
  BUTTON_MINUS,
  BUTTON_LEFTPAR,
  BUTTON_RIGHTPAR,
  BUTTON_CLEAR,
  BUTTON_DIVIDE,
  BUTTON_MULTIPLY,
  BUTTON_POWER,
  BUTTON_MOD,
  BUTTON_DEL,
  BUTTON_SQRT,
  BUTTON_X,
  BUTTON_COS,
  BUTTON_ACOS,
  BUTTON_LN,
  BUTTON_EQ,
};

// lineedit

class MainLineEdit : public QLineEdit {
  Q_OBJECT

 public:
  MainLineEdit(QWidget *parent = nullptr);

 public slots:
  void AddString(const QString &str);
};

// buttons

class SenderButton : public QPushButton {
  Q_OBJECT

 public:
  SenderButton(const QString &value, const QString &shown,
               QWidget *parent = nullptr);
  std::array<int, 4> position_;

 private:
  QString value_;
  QString shown_;

 public slots:
  void ClickedResend();

 signals:
  void ClickedSend(const QString &value);
};

class MenuSenderButton : public SenderButton {
  Q_OBJECT

 public:
  MenuSenderButton(const std::vector<QString> &values,
                   const std::vector<QString> &shown,
                   QWidget *parent = nullptr);
  void mousePressEvent(QMouseEvent *e) override;
  void mouseReleaseEvent(QMouseEvent *e) override;

 private:
  std::vector<QString> value_;
  std::vector<QString> shown_;
  int selected_;
  QMenu *menu_;
  std::vector<QAction *> actions_;

 public slots:
  void SetMenuItem(int id);

 public slots:
  void ClickedResend();
};

class MenuAction : public QAction {
  Q_OBJECT

 public:
  MenuAction(int id, const QString &text, QObject *parent = nullptr);

 private:
  int id_;

 public slots:
  void TriggeredResend();

 signals:
  void TriggeredId(int id);
};

// grids

class NumberGrid : public QGridLayout {
  Q_OBJECT

 public:
  NumberGrid(QWidget *parent = nullptr);
  void ConnectLineEdit(MainLineEdit *main_line_edit);

 private:
  std::vector<SenderButton *> numbers_;
};

class ActionGrid : public QGridLayout {
  Q_OBJECT

 public:
  ActionGrid(QWidget *parent = nullptr);
  void SetExpanded(bool value);
  void ConnectLineEdit(MainLineEdit *main_line_edit);

 private:
  std::vector<SenderButton *> actions_;
  const std::unordered_map<int, std::array<int, 4>> kVisibleTogglable{
      {BUTTON_LEFTPAR, {0, 2, 1, 1}}, {BUTTON_RIGHTPAR, {0, 3, 1, 1}},
      {BUTTON_POWER, {1, 2, 1, 1}},   {BUTTON_MOD, {1, 3, 1, 1}},
      {BUTTON_SQRT, {2, 0, 1, 1}},    {BUTTON_X, {2, 1, 1, 1}},
      {BUTTON_COS, {2, 2, 1, 2}},     {BUTTON_ACOS, {3, 2, 1, 2}},
      {BUTTON_LN, {3, 0, 1, 2}}};
  const std::map<int, std::array<int, 4>> kMoveCoordsHide{
      {BUTTON_CLEAR, {2, 0, 1, 1}},
      {BUTTON_DEL, {3, 0, 1, 1}},
      {BUTTON_EQ, {2, 1, 2, 1}}};
  const std::map<int, std::array<int, 4>> kMoveCoordsShow{
      {BUTTON_CLEAR, {0, 4, 1, 1}},
      {BUTTON_DEL, {1, 4, 1, 1}},
      {BUTTON_EQ, {2, 4, 2, 1}}};

 public slots:
  void EqClickedResend();

 signals:
  void EqClicked();
};

// view

class View : public QWidget {
  Q_OBJECT

 public:
  View(Controller &controller_);
  void closeEvent(QCloseEvent *event) override;

 private:
  void InitTopRow();
  void InitButtons();
  void PlaceItems();

  Controller &controller_;

  Graph *graph_;
  Credit *credit_;

  QVBoxLayout *main_vbox_;

  QHBoxLayout *top_hbox_;
  QHBoxLayout *button_hbox_;
  QPushButton *expand_button_;

  MainLineEdit *line_edit_;
  QPushButton *graph_button_;
  QPushButton *credit_button_;

  NumberGrid *number_grid_;
  ActionGrid *action_grid_;

  bool expanded_ = false;
  double x_ = 0;

 public slots:
  void Calculate();
  void ExpandToggle();
};

};  // namespace s21

#endif  // SMARTCALC_VIEW_VIEW_H_
