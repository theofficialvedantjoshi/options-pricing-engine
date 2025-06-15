#pragma once
#include <options-pricing-engine/Types.hpp>
#include <stdexcept>
#include <string>

namespace options {

class Option {
 public:
  Option(double spotPrice, double strikePrice, double interestRate,
         std::string maturity, double volatility, OptionType type,
         ExerciseStyle style = ExerciseStyle::European, double carry = 0.0);
  double getSpotPrice() const;
  double getStrikePrice() const;
  double getInterestRate() const;
  double getMaturity() const;
  double getVolatility() const;
  double getCarry() const;
  OptionType getType() const;
  ExerciseStyle getStyle() const;

 private:
  double spotPrice_;
  double strikePrice_;
  double interestRate_;
  double maturity_;
  double volatility_;
  double carry_;
  OptionType type_;
  ExerciseStyle style_;
};
}  // namespace options
