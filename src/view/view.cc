#include "view.h"

#include <QAction>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QIcon>
#include <QInputDialog>
#include <QLineEdit>
#include <QMenu>
#include <QMessageBox>
#include <QMouseEvent>
#include <QPushButton>
#include <QString>
#include <QVBoxLayout>
#include <QWidget>
#include <vector>

namespace s21 {

View::View(Controller &controller) : QWidget(nullptr), controller_(controller) {
  main_vbox_ = new QVBoxLayout(this);

  graph_ = new Graph(controller_);
  credit_ = new Credit(controller_);

  InitTopRow();
  InitButtons();
  PlaceItems();

  setMaximumWidth(400);
  setMaximumHeight(400);
}

void View::InitTopRow() {
  top_hbox_ = new QHBoxLayout(this);

  line_edit_ = new MainLineEdit(this);
  QObject::connect(line_edit_, &QLineEdit::textChanged, graph_,
                   &Graph::PlotFromInput);
  QObject::connect(line_edit_, &QLineEdit::returnPressed, this,
                   &View::Calculate);

  graph_button_ = new QPushButton(QIcon(":/graph.svg"), "", this);
  graph_button_->setIconSize(QSize(30, 30));
  QObject::connect(graph_button_, &QPushButton::clicked, graph_,
                   &Graph::ToggleVisibility);

  credit_button_ = new QPushButton(QIcon(":/credit.svg"), "", this);
  credit_button_->setIconSize(QSize(30, 30));
  QObject::connect(credit_button_, &QPushButton::clicked, credit_,
                   &Credit::ToggleVisibility);
}

void View::InitButtons() {
  button_hbox_ = new QHBoxLayout(this);
  number_grid_ = new NumberGrid(this);
  action_grid_ = new ActionGrid(this);
  number_grid_->ConnectLineEdit(line_edit_);
  action_grid_->ConnectLineEdit(line_edit_);
  QObject::connect(action_grid_, &ActionGrid::EqClicked, this,
                   &View::Calculate);

  expand_button_ = new QPushButton("+");
  QObject::connect(expand_button_, &QPushButton::clicked, this,
                   &View::ExpandToggle);
}

void View::PlaceItems() {
  setLayout(main_vbox_);

  main_vbox_->addLayout(top_hbox_);
  top_hbox_->addWidget(line_edit_);
  top_hbox_->addWidget(graph_button_);
  top_hbox_->addWidget(credit_button_);

  main_vbox_->addLayout(button_hbox_);
  button_hbox_->addLayout(number_grid_);
  button_hbox_->addSpacing(7);
  button_hbox_->addLayout(action_grid_);

  main_vbox_->addWidget(expand_button_);
}

void View::ExpandToggle() {
  if (expanded_) {
    action_grid_->SetExpanded(false);
    expanded_ = false;
    expand_button_->setText("+");
    setMaximumWidth(400);
    adjustSize();
  } else {
    action_grid_->SetExpanded(true);
    expanded_ = true;
    expand_button_->setText("-");
    setMaximumWidth(600);
  }
}

void View::Calculate() {
  try {
    double new_x;
    if (controller_.isContainingX(line_edit_->text())) {
      bool ok;
      new_x =
          QInputDialog::getDouble(this, "Input X value", "input x value:", x_,
                                  -1.79769e+308, 1.79769e+308, 6, &ok);
      if (ok) {
        x_ = new_x;
      } else {
        return;
      }
    }
    double res = controller_.Calculate(line_edit_->text(), x_);
    line_edit_->setText(QString::number(res));
  } catch (std::invalid_argument &e) {
    QMessageBox::warning(this, "Incorrect input", e.what());
  }
}

void View::closeEvent(QCloseEvent *event) {
  delete graph_;
  delete credit_;
  event->accept();
}

NumberGrid::NumberGrid(QWidget *parent) : QGridLayout(parent) {
  numbers_.push_back(new SenderButton("7", "7", parent));
  numbers_.push_back(new SenderButton("8", "8", parent));
  numbers_.push_back(new SenderButton("9", "9", parent));
  numbers_.push_back(new SenderButton("4", "4", parent));
  numbers_.push_back(new SenderButton("5", "5", parent));
  numbers_.push_back(new SenderButton("6", "6", parent));
  numbers_.push_back(new SenderButton("1", "1", parent));
  numbers_.push_back(new SenderButton("2", "2", parent));
  numbers_.push_back(new SenderButton("3", "3", parent));
  numbers_.push_back(new SenderButton("0", "0", parent));
  numbers_.push_back(new SenderButton(".", ".", parent));
  numbers_.push_back(new SenderButton("e", "e", parent));

  for (std::vector<SenderButton>::size_type i = 0; i < numbers_.size(); ++i) {
    addWidget(numbers_.at(i), i / 3, i % 3);
  }
}

void NumberGrid::ConnectLineEdit(MainLineEdit *main_line_edit) {
  for (auto it : numbers_) {
    QObject::connect(it, &SenderButton::ClickedSend, main_line_edit,
                     &MainLineEdit::AddString);
  }
}

ActionGrid::ActionGrid(QWidget *parent) : QGridLayout(parent) {
  actions_.push_back(new SenderButton("+", "+", parent));
  actions_.push_back(new SenderButton("-", "-", parent));
  actions_.push_back(new SenderButton("(", "(", parent));
  actions_.push_back(new SenderButton(")", ")", parent));
  actions_.push_back(new SenderButton("C", "C", parent));
  actions_.push_back(new SenderButton("/", "÷", parent));
  actions_.push_back(new SenderButton("*", "*", parent));
  actions_.push_back(new SenderButton("^", "^", parent));
  actions_.push_back(new SenderButton("mod", "mod", parent));
  actions_.push_back(new SenderButton("<-", "←", parent));
  actions_.push_back(new SenderButton("sqrt(", "√", parent));
  actions_.push_back(new SenderButton("x", "𝑥", parent));
  actions_.push_back(new MenuSenderButton({"cos(", "sin(", "tan(", "cotan("},
                                          {"cos", "sin", "tan", "cotan"},
                                          parent));
  actions_.push_back(new MenuSenderButton({"acos(", "asin(", "atan("},
                                          {"acos", "asin", "atan"}, parent));
  actions_.push_back(
      new MenuSenderButton({"ln(", "log("}, {"ln", "log"}, parent));
  actions_.push_back(new SenderButton("=", "=", parent));

  for (std::vector<SenderButton>::size_type i = 0; i < 12; ++i) {
    addWidget(actions_.at(i), i / 5, i % 5);
  }
  addWidget(actions_.at(BUTTON_COS), 2, 2, 1, 2);
  addWidget(actions_.at(BUTTON_ACOS), 3, 2, 1, 2);
  addWidget(actions_.at(BUTTON_LN), 3, 0, 1, 2);
  addWidget(actions_.at(BUTTON_EQ), 2, 4, 2, 1);

  SetExpanded(false);
}

void ActionGrid::ConnectLineEdit(MainLineEdit *main_line_edit) {
  for (int i = 0; i <= BUTTON_EQ; ++i) {
    if (i != BUTTON_CLEAR && i != BUTTON_DEL && i != BUTTON_EQ) {
      QObject::connect(actions_[i], &SenderButton::ClickedSend, main_line_edit,
                       &MainLineEdit::AddString);
    }
  }

  QObject::connect(actions_[BUTTON_CLEAR], &SenderButton::ClickedSend,
                   main_line_edit, &MainLineEdit::clear);
  QObject::connect(actions_[BUTTON_DEL], &SenderButton::ClickedSend,
                   main_line_edit, &MainLineEdit::backspace);
  QObject::connect(actions_[BUTTON_EQ], &SenderButton::clicked, this,
                   &ActionGrid::EqClickedResend);
}

void ActionGrid::EqClickedResend() { emit EqClicked(); }

void ActionGrid::SetExpanded(bool value) {
  if (!value) {
    for (auto it : kVisibleTogglable) {
      actions_.at(it.first)->setVisible(false);
      removeWidget(actions_.at(it.first));
    }
    for (auto it : kMoveCoordsHide) {
      removeWidget(actions_.at(it.first));
      addWidget(actions_.at(it.first), it.second[0], it.second[1], it.second[2],
                it.second[3]);
    }
  } else {
    for (auto it : kVisibleTogglable) {
      actions_.at(it.first)->setVisible(true);
      addWidget(actions_.at(it.first), it.second[0], it.second[1], it.second[2],
                it.second[3]);
    }
    for (auto it : kMoveCoordsShow) {
      removeWidget(actions_.at(it.first));
      addWidget(actions_.at(it.first), it.second[0], it.second[1], it.second[2],
                it.second[3]);
    }
  }
}

SenderButton::SenderButton(const QString &value, const QString &shown,
                           QWidget *parent)
    : QPushButton(shown, parent), value_(value), shown_(shown) {
  setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  setMinimumWidth(50);
  setMinimumHeight(50);
  QObject::connect(this, &SenderButton::clicked, this,
                   &SenderButton::ClickedResend);
}

void SenderButton::ClickedResend() { emit ClickedSend(value_); }

MenuSenderButton::MenuSenderButton(const std::vector<QString> &values,
                                   const std::vector<QString> &shown,
                                   QWidget *parent)
    : SenderButton(values[0], shown[0], parent),
      value_(values),
      shown_(shown),
      selected_(0),
      menu_(new QMenu(this)) {
  for (std::vector<QString>::size_type i = 0; i < shown.size(); ++i) {
    MenuAction *action = new MenuAction(i, shown_[i], this);
    actions_.push_back(action);
    menu_->addAction(actions_[i]);
    QObject::connect(action, &MenuAction::TriggeredId, this,
                     &MenuSenderButton::SetMenuItem);
  }
  setMenu(menu_);
  QObject::disconnect(this, &SenderButton::clicked, this,
                      &SenderButton::ClickedResend);
  QObject::connect(this, &MenuSenderButton::clicked, this,
                   &MenuSenderButton::ClickedResend);
}

void MenuSenderButton::ClickedResend() { emit ClickedSend(value_[selected_]); }

void MenuSenderButton::SetMenuItem(int id) {
  selected_ = id;
  setText(shown_[id]);
}

void MenuSenderButton::mousePressEvent(QMouseEvent *event) {
  if (event->button() == Qt::RightButton) {
    emit showMenu();
  } else if (event->button() == Qt::LeftButton) {
    setDown(true);
  } else {
    QPushButton::mousePressEvent(event);
  }
}

void MenuSenderButton::mouseReleaseEvent(QMouseEvent *event) {
  emit released();
  setMenu(menu_);
  QPushButton::mouseReleaseEvent(event);
}

MenuAction::MenuAction(int id, const QString &text, QObject *parent)
    : QAction(text, parent), id_(id) {
  QObject::connect(this, &QAction::triggered, this,
                   &MenuAction::TriggeredResend);
}

void MenuAction::TriggeredResend() { emit TriggeredId(id_); }

MainLineEdit::MainLineEdit(QWidget *parent) : QLineEdit(parent) {
  setPlaceholderText("0");
  setAlignment(Qt::AlignRight);
  QFont lefont("Courier New");
  lefont.setStyleHint(QFont::Monospace);
  lefont.setPointSize(QFont().pointSize() + 5);
  setFont(lefont);
  setMaxLength(256);
}

void MainLineEdit::AddString(const QString &value) {
  if (text() == "0") {
    clear();
  }
  insert(value);
}

};  // namespace s21
