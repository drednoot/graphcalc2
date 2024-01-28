#include "calculator.h"

#include <cmath>
#include <cstring>
#include <stack>
#include <stdexcept>
#include <string>

namespace s21 {

bool CalculatorModel::isContainingX(const std::string &input) {
  UpdateRpn(input);
  for (auto lex : rpn_) {
    if (lex.isVar()) {
      return true;
    }
  }
  return false;
}

double CalculatorModel::Calculate(const std::string &input, double x) {
  UpdateRpn(input);
  return Solve(x);
}

std::pair<std::vector<double>, std::vector<double>> CalculatorModel::Calculate(
    const std::string &input, double low_x, double high_x, double low_y,
    double high_y, size_t points) {
  UpdateRpn(input);
  std::vector<double> xv(points), yv(points);

  double d = (high_x - low_x) / points;
  double x = low_x;
  for (size_t i = 0; i < points; ++i, x += d) {
    xv[i] = x;
    double y = Solve(x);
    if ((y >= low_y && y <= high_y) || (low_y == 0 && high_y == 0)) {
      yv[i] = Solve(x);
    } else {
      yv[i] = NAN;
    }
  }

  return std::pair<std::vector<double>, std::vector<double>>(xv, yv);
}

void CalculatorModel::UpdateRpn(const std::string &input) {
  size_t hash = std::hash<std::string>{}(input);

  if (hash != old_hash_) {
    rpn_.clear();
    std::vector<Lexeme> parsed = Parse(input);
    ShuntingYard(parsed);
    old_hash_ = hash;
  }
}

std::vector<Lexeme> CalculatorModel::Parse(const std::string &input) {
  std::vector<Lexeme> result;

  for (auto cur = input.data(); *cur;) {
    if (std::isspace(*cur)) {
      ++cur;
      continue;
    } else if (*cur == '.' || isdigit(*cur)) {
      Lexeme number_lexeme = ParseNumber(cur, input.data());
      result.push_back(number_lexeme);
    } else {
      Lexeme type_lexeme = ParseType(cur, input.data());
      result.push_back(type_lexeme);
    }
  }

  ContextDepententParse(result);

  return result;
}

Lexeme CalculatorModel::ParseNumber(
    const std::string::value_type *&cur,
    const std::string::value_type *input_begin) {
  std::string::size_type off = 0;
  double num = 0;
  try {
    num = std::stod(cur, &off);
  } catch (std::invalid_argument &e) {
    ThrowError(INCORRECT_NUMBER, cur - input_begin);
  }
  cur += off;
  Lexeme number_lexeme = kLexemeProperties[Lexeme::NUM];
  number_lexeme.num = num;
  return number_lexeme;
}

Lexeme CalculatorModel::ParseType(const std::string::value_type *&cur,
                                  const std::string::value_type *input_begin) {
  auto it = kStringToLexeme.begin();
  for (; it != kStringToLexeme.end(); ++it) {
    if (!strncmp(cur, it->first.data(), it->first.length())) {
      cur += it->first.length();
      return kLexemeProperties[it->second];
    }
  }
  if (it == kStringToLexeme.end()) {
    ThrowError(INCORRECT_LEXEME, cur - input_begin);
  }
  return Lexeme{};
}

void CalculatorModel::ContextDepententParse(
    std::vector<Lexeme> &parsed_string) {
  if (parsed_string.size() <= 1) return;
  for (auto prev = parsed_string.begin(), it = prev + 1;
       it != parsed_string.end(); prev = it, ++it) {
    bool condition = (prev->type == Lexeme::RIGHTPAR || prev->isNumber());
    if (it->type == Lexeme::UPLUS && condition) {
      *it = kLexemeProperties[Lexeme::ADD];
    } else if (it->type == Lexeme::UMINUS && condition) {
      *it = kLexemeProperties[Lexeme::SUB];
    }
  }
}

void CalculatorModel::ShuntingYard(const std::vector<Lexeme> &input) {
  std::stack<Lexeme> stack;

  for (auto lex = input.begin(); lex != input.end(); ++lex) {
    if (lex->isNumber()) {
      rpn_.push_back(*lex);
    } else if (lex->isFunction() || lex->type == Lexeme::LEFTPAR) {
      stack.push(*lex);
    } else if (lex->isOperator()) {
      ShuntingYardOperatorCase(stack, *lex);
    } else if (lex->type == Lexeme::RIGHTPAR) {
      ShuntingYardRightParenthesisCase(stack, lex - input.begin());
    }
  }
  ShuntingYardEmptyStack(stack);
}

void CalculatorModel::ShuntingYardOperatorCase(std::stack<Lexeme> &stack,
                                               const Lexeme lexeme) {
  while (!stack.empty() && stack.top().isOperator() &&
         (stack.top().priority > lexeme.priority ||
          (stack.top().priority == lexeme.priority &&
           lexeme.assoc == Lexeme::ASSOC_LEFT))) {
    rpn_.push_back(stack.top());
    stack.pop();
  }
  stack.push(lexeme);
}

void CalculatorModel::ShuntingYardRightParenthesisCase(
    std::stack<Lexeme> &stack, size_t right_parent_offset) {
  while (!stack.empty() && stack.top().type != Lexeme::LEFTPAR) {
    rpn_.push_back(stack.top());
    stack.pop();
  }
  if (!stack.empty() && stack.top().type == Lexeme::LEFTPAR) {
    stack.pop();
  } else {
    ThrowError(UNOPENED_PARENT, right_parent_offset);
  }
  if (!stack.empty() && stack.top().isFunction()) {
    rpn_.push_back(stack.top());
    stack.pop();
  }
}

void CalculatorModel::ShuntingYardEmptyStack(std::stack<Lexeme> &stack) {
  while (!stack.empty()) {
    if (stack.top().type == Lexeme::LEFTPAR) {
      ThrowError(UNCLOSED_PARENT);
    }
    rpn_.push_back(stack.top());
    stack.pop();
  }
}

double CalculatorModel::Solve(double x) const {
  if (rpn_.empty()) return 0;
  std::stack<double> numstack;

  for (auto lex : rpn_) {
    if (lex.isVar()) {
      numstack.push(x);
    } else if (lex.isNumber()) {
      numstack.push(lex.num);
    } else if (lex.isFunction() || lex.isOperator()) {
      if (numstack.size() < lex.operandCount) {
        ThrowError(NOT_ENOUGH_OPERANDS);
      }
      std::vector<double> operands(lex.operandCount);
      for (size_t i = 0; i < lex.operandCount; ++i) {
        operands[lex.operandCount - i - 1] = numstack.top();
        numstack.pop();
      }
      numstack.push(Apply(lex, operands));
    }
  }

  if (numstack.size() > 1) {
    ThrowError(MORE_NUMBERS_THAN_EXPECTED);
  }
  return numstack.top();
}

double CalculatorModel::Apply(const Lexeme lex,
                              const std::vector<double> &operands) const {
  if (!lex.solver) {
    ThrowError(UNIMPLEMENTED_SOLVER_CALLED);
  }
  return lex.solver(operands);
}

double CalculatorModel::Solver::uplus(
    const std::vector<double> &operands) noexcept {
  return operands[0];
}

double CalculatorModel::Solver::uminus(
    const std::vector<double> &operands) noexcept {
  return -operands[0];
}

double CalculatorModel::Solver::add(
    const std::vector<double> &operands) noexcept {
  return operands[0] + operands[1];
}

double CalculatorModel::Solver::sub(
    const std::vector<double> &operands) noexcept {
  return operands[0] - operands[1];
}

double CalculatorModel::Solver::pow(
    const std::vector<double> &operands) noexcept {
  return ::pow(operands[0], operands[1]);
}

double CalculatorModel::Solver::mul(
    const std::vector<double> &operands) noexcept {
  return operands[0] * operands[1];
}

double CalculatorModel::Solver::div(
    const std::vector<double> &operands) noexcept {
  return operands[0] / operands[1];
}

double CalculatorModel::Solver::mod(
    const std::vector<double> &operands) noexcept {
  return fmod(operands[0], operands[1]);
}

double CalculatorModel::Solver::cos(
    const std::vector<double> &operands) noexcept {
  return ::cos(operands[0]);
}

double CalculatorModel::Solver::sin(
    const std::vector<double> &operands) noexcept {
  return ::sin(operands[0]);
}

double CalculatorModel::Solver::tan(
    const std::vector<double> &operands) noexcept {
  return ::tan(operands[0]);
}

double CalculatorModel::Solver::cotan(
    const std::vector<double> &operands) noexcept {
  return 1 / ::tan(operands[0]);
}

double CalculatorModel::Solver::acos(
    const std::vector<double> &operands) noexcept {
  return ::acos(operands[0]);
}

double CalculatorModel::Solver::asin(
    const std::vector<double> &operands) noexcept {
  return ::asin(operands[0]);
}

double CalculatorModel::Solver::atan(
    const std::vector<double> &operands) noexcept {
  return ::atan(operands[0]);
}

double CalculatorModel::Solver::sqrt(
    const std::vector<double> &operands) noexcept {
  return ::sqrt(operands[0]);
}

double CalculatorModel::Solver::ln(
    const std::vector<double> &operands) noexcept {
  return ::log(operands[0]);
}

double CalculatorModel::Solver::log(
    const std::vector<double> &operands) noexcept {
  return ::log(operands[0]) / ::log(10);
}

void CalculatorModel::ThrowError(enum ErrorCode code, size_t position) const {
  if (code == INCORRECT_LEXEME) {
    throw std::invalid_argument(std::string("Incorrect Lexeme Type at char ") +
                                std::to_string(position));
  } else if (code == INCORRECT_NUMBER) {
    throw std::invalid_argument(std::string("Incorrect Number Value at char ") +
                                std::to_string(position));
  } else if (code == UNCLOSED_PARENT) {
    throw std::invalid_argument(
        std::string("Found parenthesis (unclosed) without a pair"));
  } else if (code == UNOPENED_PARENT) {
    throw std::invalid_argument(
        std::string("Found parenthesis (unopened) without a pair at lexem #") +
        std::to_string(position + 1));
  } else if (code == NOT_ENOUGH_OPERANDS) {
    throw std::invalid_argument(
        std::string("Some operator/function had not enough operands"));
  } else if (code == MORE_NUMBERS_THAN_EXPECTED) {
    throw std::invalid_argument(
        std::string("Found two or more numbers in a row without an operator"));
  } else if (code == UNIMPLEMENTED_SOLVER_CALLED) {
    throw std::invalid_argument(
        std::string("Operator that wasn't implemented can't be applied"));
  }
}

bool Lexeme::isNumber() const noexcept { return ftype == NUMBER; }
bool Lexeme::isVar() const noexcept { return type == XNUM; }
bool Lexeme::isFunction() const noexcept { return ftype == FUNCTION; }
bool Lexeme::isOperator() const noexcept { return ftype == OPERATOR; }

};  // namespace s21
