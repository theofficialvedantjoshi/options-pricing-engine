#pragma once
#include <options-pricing-engine/Types.hpp>
#include <stdexcept>
#include <string_view>

namespace options {

class Option {
  public:
    Option(Price spotPrice, Price strikePrice, Rate interestRate,
           std::string_view maturity, Rate volatility, OptionType type,
           ExerciseStyle style = ExerciseStyle::European, Rate yield = 0.0);
    Price getSpotPrice() const;
    Price getStrikePrice() const;
    Rate getInterestRate() const;
    double getMaturity() const;
    Rate getVolatility() const;
    Rate getYield() const;
    OptionType getType() const;
    ExerciseStyle getStyle() const;
    void setVolatility(Rate sigma);
    void setSpotPrice(Price spotPrice);
    void setInterestRate(Rate interestRate);
    void setMaturity(double maturity);

  private:
    Price m_spotPrice;
    Price m_strikePrice;
    Rate m_interestRate;
    double m_maturity;
    Rate m_volatility;
    Rate m_yield;
    OptionType m_type;
    ExerciseStyle m_style;
};
} // namespace options
