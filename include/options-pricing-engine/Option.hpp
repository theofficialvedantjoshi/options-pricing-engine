#pragma once
#include <options-pricing-engine/Types.hpp>
#include <stdexcept>
#include <string_view>

namespace options {

class Option {
  public:
    Option(Price spotPrice, Price strikePrice, Rate interestRate,
           std::string_view maturity, Rate volatility, OptionType type,
           ExerciseStyle style = ExerciseStyle::European, Rate carry = 0.0);
    Price getSpotPrice() const;
    Price getStrikePrice() const;
    Rate getInterestRate() const;
    double getMaturity() const;
    Rate getVolatility() const;
    Rate getCarry() const;
    OptionType getType() const;
    ExerciseStyle getStyle() const;
    void setVolatility(Rate sigma);

  private:
    Price m_spotPrice;
    Price m_strikePrice;
    Rate m_interestRate;
    double m_maturity;
    Rate m_volatility;
    Rate m_carry;
    OptionType m_type;
    ExerciseStyle m_style;
};
} // namespace options
