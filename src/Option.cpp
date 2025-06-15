#include <options-pricing-engine/Option.hpp>
#include <options-pricing-engine/Types.hpp>
#include <stdexcept>
#include <string>

namespace options {
Option::Option(double spotPrice, double strikePrice, double interestRate,
               std::string maturity, double volatility, OptionType type,
               ExerciseStyle style, double carry)
    : spotPrice_(spotPrice),
      strikePrice_(strikePrice),
      interestRate_(interestRate),
      volatility_(volatility),
      type_(type),
      style_(style),
      carry_(carry) {
  if (spotPrice <= 0 || strikePrice <= 0 || volatility <= 0) {
    throw std::invalid_argument(
        "Spot price, strike price, and volatility must be positive values.");
  }
  if (maturity.length() < 3 ||
      maturity.substr(maturity.length() - 2, 2) != "mo" ||
      !std::isdigit(maturity[0])) {
    throw std::invalid_argument(
        "Maturity must be a string in the format 'Xm' where X is a positive "
        "integer representing months.");
  }
  double months = 0.0;
  try {
    months = std::stod(maturity.substr(0, maturity.length() - 2));
  } catch (const std::invalid_argument &) {
    throw std::invalid_argument(
        "Maturity must be a string in the format 'Xm' where X is a positive "
        "integer representing months.");
  }
  maturity_ = months / 12.0;
}
double Option::getSpotPrice() const { return spotPrice_; }
double Option::getStrikePrice() const { return strikePrice_; }
double Option::getInterestRate() const { return interestRate_; }
double Option::getMaturity() const { return maturity_; }
double Option::getVolatility() const { return volatility_; }
double Option::getCarry() const { return carry_; }
OptionType Option::getType() const { return type_; }
ExerciseStyle Option::getStyle() const { return style_; }

}  // namespace options