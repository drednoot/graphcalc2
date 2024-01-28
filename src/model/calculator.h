#ifndef SMARTCALC_MODEL_CALCULATOR_H_
#define SMARTCALC_MODEL_CALCULATOR_H_

#include <stack>
#include <string>
#include <vector>

namespace s21 {

enum ErrorCode {
  INCORRECT_LEXEME,
  INCORRECT_NUMBER,
  UNCLOSED_PARENT,
  UNOPENED_PARENT,
  NOT_ENOUGH_OPERANDS,
  MORE_NUMBERS_THAN_EXPECTED,
  UNIMPLEMENTED_SOLVER_CALLED,
};

struct Lexeme {
  enum Type {
    NO_TYPE,
    UPLUS,
    UMINUS,
    ADD,
    SUB,
    XNUM,
    NUM,
    POW,
    MUL,
    DIV,
    MOD,
    LEFTPAR,
    RIGHTPAR,
    COS,
    SIN,
    TAN,
    COTAN,
    ACOS,
    ASIN,
    ATAN,
    SQRT,
    LN,
    LOG,
    END,
  };
  enum Associativity { NO_ASSOC, ASSOC_LEFT, ASSOC_RIGHT };
  enum FundamentalType {
    NO_FUNDAMENTAL_TYPE,
    NUMBER,
    FUNCTION,
    OPERATOR,
    PARENTHESIS,
  };

  enum Type type = NO_TYPE;
  size_t priority = -1;
  double num = 0;
  size_t operandCount = 0;
  enum Associativity assoc = NO_ASSOC;
  enum FundamentalType ftype = NO_FUNDAMENTAL_TYPE;
  double (*solver)(const std::vector<double> &);

  bool isNumber() const noexcept;
  bool isVar() const noexcept;
  bool isFunction() const noexcept;
  bool isOperator() const noexcept;
};

class CalculatorModel {
 public:
  bool isContainingX(const std::string &input);
  double Calculate(const std::string &input, double x = 0);
  std::pair<std::vector<double>, std::vector<double>> Calculate(
      const std::string &input, double low_x, double high_x, double low_y,
      double high_y, size_t points);

 private:
  void UpdateRpn(const std::string &input);

  std::vector<Lexeme> Parse(const std::string &input);
  Lexeme ParseNumber(const std::string::value_type *&cur,
                     const std::string::value_type *input_begin);
  Lexeme ParseType(const std::string::value_type *&cur,
                   const std::string::value_type *input_begin);
  void ContextDepententParse(std::vector<Lexeme> &parsed_string);
  size_t old_hash_ = 0;

  void ShuntingYard(const std::vector<Lexeme> &input);
  void ShuntingYardOperatorCase(std::stack<Lexeme> &stack, const Lexeme lex);
  void ShuntingYardRightParenthesisCase(std::stack<Lexeme> &stack,
                                        size_t right_parent_offset);
  void ShuntingYardEmptyStack(std::stack<Lexeme> &stack);
  std::vector<Lexeme> rpn_;

  double Solve(double x) const;
  double Apply(const Lexeme lexeme, const std::vector<double> &operands) const;

  void ThrowError(enum ErrorCode code, size_t position = 0) const;

  class Solver {
   public:
    static double uplus(const std::vector<double> &operands) noexcept;
    static double uminus(const std::vector<double> &operands) noexcept;
    static double add(const std::vector<double> &operands) noexcept;
    static double sub(const std::vector<double> &operands) noexcept;
    static double pow(const std::vector<double> &operands) noexcept;
    static double mul(const std::vector<double> &operands) noexcept;
    static double div(const std::vector<double> &operands) noexcept;
    static double mod(const std::vector<double> &operands) noexcept;
    static double cos(const std::vector<double> &operands) noexcept;
    static double sin(const std::vector<double> &operands) noexcept;
    static double tan(const std::vector<double> &operands) noexcept;
    static double cotan(const std::vector<double> &operands) noexcept;
    static double acos(const std::vector<double> &operands) noexcept;
    static double asin(const std::vector<double> &operands) noexcept;
    static double atan(const std::vector<double> &operands) noexcept;
    static double sqrt(const std::vector<double> &operands) noexcept;
    static double ln(const std::vector<double> &operands) noexcept;
    static double log(const std::vector<double> &operands) noexcept;
  };

  const std::vector<std::pair<std::string, Lexeme::Type>> kStringToLexeme{
      {"+", Lexeme::UPLUS},   {"-", Lexeme::UMINUS},   {"x", Lexeme::XNUM},
      {"X", Lexeme::XNUM},    {"^", Lexeme::POW},      {"*", Lexeme::MUL},
      {"/", Lexeme::DIV},     {"%", Lexeme::MOD},      {"mod", Lexeme::MOD},
      {"(", Lexeme::LEFTPAR}, {")", Lexeme::RIGHTPAR}, {"cos", Lexeme::COS},
      {"sin", Lexeme::SIN},   {"ctg", Lexeme::COTAN},  {"cotan", Lexeme::COTAN},
      {"tg", Lexeme::TAN},    {"tan", Lexeme::TAN},    {"acos", Lexeme::ACOS},
      {"asin", Lexeme::ASIN}, {"atan", Lexeme::ATAN},  {"sqrt", Lexeme::SQRT},
      {"ln", Lexeme::LN},     {"log", Lexeme::LOG},
  };

  const Lexeme kLexemeProperties[Lexeme::END]{
      {Lexeme::NO_TYPE, 0, 0, 0, Lexeme::NO_ASSOC, Lexeme::NO_FUNDAMENTAL_TYPE,
       nullptr},
      {Lexeme::UPLUS, 3, 0, 1, Lexeme::ASSOC_LEFT, Lexeme::OPERATOR,
       &Solver::uplus},
      {Lexeme::UMINUS, 3, 0, 1, Lexeme::ASSOC_LEFT, Lexeme::OPERATOR,
       &Solver::uminus},
      {Lexeme::ADD, 1, 0, 2, Lexeme::ASSOC_LEFT, Lexeme::OPERATOR,
       &Solver::add},
      {Lexeme::SUB, 1, 0, 2, Lexeme::ASSOC_LEFT, Lexeme::OPERATOR,
       &Solver::sub},
      {Lexeme::XNUM, 0, 0, 0, Lexeme::NO_ASSOC, Lexeme::NUMBER, nullptr},
      {Lexeme::NUM, 0, 0, 0, Lexeme::NO_ASSOC, Lexeme::NUMBER, nullptr},
      {Lexeme::POW, 4, 0, 2, Lexeme::ASSOC_RIGHT, Lexeme::OPERATOR,
       &Solver::pow},
      {Lexeme::MUL, 2, 0, 2, Lexeme::ASSOC_LEFT, Lexeme::OPERATOR,
       &Solver::mul},
      {Lexeme::DIV, 2, 0, 2, Lexeme::ASSOC_LEFT, Lexeme::OPERATOR,
       &Solver::div},
      {Lexeme::MOD, 2, 0, 2, Lexeme::ASSOC_LEFT, Lexeme::OPERATOR,
       &Solver::mod},
      {Lexeme::LEFTPAR, 0, 0, 0, Lexeme::NO_ASSOC, Lexeme::PARENTHESIS,
       nullptr},
      {Lexeme::RIGHTPAR, 0, 0, 0, Lexeme::NO_ASSOC, Lexeme::PARENTHESIS,
       nullptr},
      {Lexeme::COS, 0, 0, 1, Lexeme::NO_ASSOC, Lexeme::FUNCTION, &Solver::cos},
      {Lexeme::SIN, 0, 0, 1, Lexeme::NO_ASSOC, Lexeme::FUNCTION, &Solver::sin},
      {Lexeme::TAN, 0, 0, 1, Lexeme::NO_ASSOC, Lexeme::FUNCTION, &Solver::tan},
      {Lexeme::COTAN, 0, 0, 1, Lexeme::NO_ASSOC, Lexeme::FUNCTION,
       &Solver::cotan},
      {Lexeme::ACOS, 0, 0, 1, Lexeme::NO_ASSOC, Lexeme::FUNCTION,
       &Solver::acos},
      {Lexeme::ASIN, 0, 0, 1, Lexeme::NO_ASSOC, Lexeme::FUNCTION,
       &Solver::asin},
      {Lexeme::ATAN, 0, 0, 1, Lexeme::NO_ASSOC, Lexeme::FUNCTION,
       &Solver::atan},
      {Lexeme::SQRT, 0, 0, 1, Lexeme::NO_ASSOC, Lexeme::FUNCTION,
       &Solver::sqrt},
      {Lexeme::LN, 0, 0, 1, Lexeme::NO_ASSOC, Lexeme::FUNCTION, &Solver::ln},
      {Lexeme::LOG, 0, 0, 1, Lexeme::NO_ASSOC, Lexeme::FUNCTION, &Solver::log},
  };
};

};  // namespace s21

#endif  // SMARTCALC_MODEL_CALCULATOR_H_
