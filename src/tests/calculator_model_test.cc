#include <gtest/gtest.h>

#include <cmath>

#include "../model/calculator.h"

class CalcTest : public testing::Test {
 protected:
  s21::CalculatorModel m;
};

TEST_F(CalcTest, caseNoInput) { EXPECT_EQ(m.Calculate(""), 0); }

TEST_F(CalcTest, zeroOutput) { EXPECT_EQ(m.Calculate("()"), 0); }

TEST_F(CalcTest, caseBasic1) { EXPECT_EQ(m.Calculate("5+10"), 15); }

TEST_F(CalcTest, caseBasic2) { EXPECT_EQ(m.Calculate("5-10"), -5); }

TEST_F(CalcTest, caseBasic3) { EXPECT_EQ(m.Calculate("5 * 10"), 50); }

TEST_F(CalcTest, caseBasic4) { EXPECT_EQ(m.Calculate("5 / -10"), -0.5); }

TEST_F(CalcTest, caseBasic5) { EXPECT_EQ(m.Calculate("2 ^ 3"), 8); }

TEST_F(CalcTest, caseBasic6) { EXPECT_EQ(m.Calculate("100 mod 3"), 1); }

TEST_F(CalcTest, caseBasic7) { EXPECT_EQ(m.Calculate("100 % 3"), 1); }

TEST_F(CalcTest, caseBasic8) { EXPECT_EQ(m.Calculate("+5"), 5); }

TEST_F(CalcTest, caseBasic9) { EXPECT_EQ(m.Calculate("-5"), -5); }

TEST_F(CalcTest, caseBasic10) {
  EXPECT_NEAR(m.Calculate("sin(3.1415/2)"), 1, 1e-4);
}

TEST_F(CalcTest, caseBasic11) { EXPECT_EQ(m.Calculate("cos0"), 1); }

TEST_F(CalcTest, caseBasic12) { EXPECT_EQ(m.Calculate("tg0"), 0); }

TEST_F(CalcTest, caseBasic13) { EXPECT_EQ(m.Calculate("tan0"), 0); }

TEST_F(CalcTest, caseBasic14) {
  EXPECT_NEAR(m.Calculate("cotan(3.1415/2)"), 0, 1e-4);
}

TEST_F(CalcTest, caseBasic15) {
  EXPECT_NEAR(m.Calculate("ctg(3.1415/2)"), 0, 1e-4);
}

TEST_F(CalcTest, caseBasic16) {
  EXPECT_NEAR(m.Calculate("asin(1)"), 3.1415 / 2, 1e-4);
}

TEST_F(CalcTest, caseBasic17) { EXPECT_NEAR(m.Calculate("acos(1)"), 0, 1e-4); }

TEST_F(CalcTest, caseBasic18) { EXPECT_NEAR(m.Calculate("atan(0)"), 0, 1e-4); }

TEST_F(CalcTest, caseBasic19) { EXPECT_NEAR(m.Calculate("sqrt(4)"), 2, 1e-4); }

TEST_F(CalcTest, caseBasic20) { EXPECT_NEAR(m.Calculate("ln(1)"), 0, 1e-4); }

TEST_F(CalcTest, caseBasic21) { EXPECT_NEAR(m.Calculate("log(1)"), 0, 1e-4); }

TEST_F(CalcTest, casePrecedence) {
  EXPECT_NEAR(m.Calculate("3 + 4 * 2 / ( 1 - 5 ) ^ 2 ^ 3"),
              3.0 + 4.0 * 2.0 / pow((1.0 - 5.0), pow(2, 3)), 1e-4);
}

TEST_F(CalcTest, caseParenthesis) {
  EXPECT_NEAR(m.Calculate("(1)*((2))-((3)/(-5*2))"),
              1.0 * 2.0 - (3.0 / (-5.0 * 2.0)), 1e-4);
}

TEST_F(CalcTest, caseSinCosSinCos) {
  EXPECT_NEAR(m.Calculate("sincossincos0.5"), sin(cos(sin(cos(0.5)))), 1e-4);
}

TEST_F(CalcTest, unary) {
  EXPECT_NEAR(m.Calculate("-(5--5)"), -(5 - (-5)), 1e-4);
}

TEST_F(CalcTest, twoNumbers1) {
  EXPECT_THROW(m.Calculate("5 5"), std::logic_error);
}

TEST_F(CalcTest, twoNumbers2) {
  EXPECT_THROW(m.Calculate("1.1.1"), std::logic_error);
}

TEST_F(CalcTest, correctFloatParsing) {
  EXPECT_NEAR(m.Calculate(".1e+2+.1-1."), 0.1e+2 + 0.1 - 1.0, 1e-4);
}

TEST_F(CalcTest, incorrectFloatParsing) {
  EXPECT_THROW(m.Calculate(".e+2"), std::logic_error);
}

TEST_F(CalcTest, incorrectLexeme1) {
  EXPECT_THROW(m.Calculate("asdf"), std::logic_error);
}

TEST_F(CalcTest, incorrectLexeme2) {
  EXPECT_THROW(m.Calculate("sina"), std::logic_error);
}

TEST_F(CalcTest, unclosedParenthesis) {
  EXPECT_THROW(m.Calculate("sin(5"), std::logic_error);
}

TEST_F(CalcTest, unopenedParenthesis) {
  EXPECT_THROW(m.Calculate("5*(sin5))"), std::logic_error);
}

TEST_F(CalcTest, notEnoughOperands1) {
  EXPECT_THROW(m.Calculate("10+"), std::logic_error);
}

TEST_F(CalcTest, xSubstitution1) {
  EXPECT_NEAR(m.Calculate("5 + x + 10", 5), 20, 1e-4);
}

TEST_F(CalcTest, xSubstitution2) {
  EXPECT_NEAR(m.Calculate("5 + X + 10", 5), 20, 1e-4);
}

TEST_F(CalcTest, hashing) {
  double expected = sin(cos(sin(cos(5))));
  EXPECT_NEAR(m.Calculate("sin(cos(sin(cos(x))))", 5), expected, 1e-4);
  EXPECT_NEAR(m.Calculate("sin(cos(sin(cos(x))))", 5), expected, 1e-4);
}

TEST_F(CalcTest, isContainingX1) {
  EXPECT_EQ(m.isContainingX("sin(cos(sin(cos(x))))"), true);
}

TEST_F(CalcTest, isContainingX2) {
  EXPECT_EQ(m.isContainingX("sin(cos(sin(cos(1))))"), false);
}

TEST_F(CalcTest, isContainingX3) {
  EXPECT_EQ(m.isContainingX("sin(cos(sin(cos(X))))"), true);
}

TEST_F(CalcTest, isContainingX4) {
  EXPECT_THROW(m.isContainingX("asdf x X x xd"), std::logic_error);
}

TEST_F(CalcTest, CalculatePlot) {
  std::pair<std::vector<double>, std::vector<double>> xy =
      m.Calculate("2*x", -1, 1, 0, 2, 4);

  EXPECT_EQ(xy.first.size(), 4);
  EXPECT_EQ(xy.second.size(), 4);

  EXPECT_EQ(xy.first[0], -1);
  EXPECT_EQ(xy.first[1], -0.5);
  EXPECT_EQ(xy.first[2], 0);
  EXPECT_EQ(xy.first[3], 0.5);

  EXPECT_NE(xy.second[0], xy.second[0]);
  EXPECT_NE(xy.second[1], xy.second[1]);
  EXPECT_EQ(xy.second[2], 0);
  EXPECT_EQ(xy.second[3], 1);
}

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
