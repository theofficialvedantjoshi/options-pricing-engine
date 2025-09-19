#include <charconv>
#include <options-pricing-engine/Option.hpp>
#include <options-pricing-engine/Types.hpp>
#include <stdexcept>
#include <string_view>

namespace options {
Option::Option(Price spotPrice, Price strikePrice, Rate interestRate,
               std::string_view maturity, Rate volatility, OptionType type,
               ExerciseStyle style, Rate yield)
    : m_spotPrice(spotPrice), m_strikePrice(strikePrice),
      m_interestRate(interestRate), m_volatility(volatility), m_type(type),
      m_style(style), m_yield(yield) {
    if (spotPrice <= 0.0 || strikePrice <= 0.0 || volatility <= 0.0) {
        throw std::invalid_argument(
            "Spot price, strike price, and volatility "
            "must be positive values. Verify "
            "configuration before creating or loading an option.");
    }
    if (maturity.length() < 3 ||
        maturity.substr(maturity.length() - 2, 2) != "mo" ||
        !std::isdigit(maturity[0])) {
        throw std::invalid_argument("Maturity must be a string in the format "
                                    "'Xmo' where X is a positive "
                                    "integer representing months.");
    }
    double months = 0.0;
    std::string_view num = maturity.substr(0, maturity.size() - 2);
    auto [ptr, ec] =
        std::from_chars(num.data(), num.data() + num.size(), months);
    if (ec != std::errc()) {
        throw std::invalid_argument(
            "Maturity must be a string in the format "
            "'Xmo' where X is a positive integer representing months.");
    }
    m_maturity = months / 12.0;
}
Price Option::getSpotPrice() const { return m_spotPrice; }
Price Option::getStrikePrice() const { return m_strikePrice; }
Rate Option::getInterestRate() const { return m_interestRate; }
double Option::getMaturity() const { return m_maturity; }
Rate Option::getVolatility() const { return m_volatility; }
Rate Option::getYield() const { return m_yield; }
OptionType Option::getType() const { return m_type; }
ExerciseStyle Option::getStyle() const { return m_style; }
void Option::setVolatility(Rate sigma) { m_volatility = sigma; }
void Option::setSpotPrice(Price spotPrice) { m_spotPrice = spotPrice; }
void Option::setInterestRate(Rate interestRate) {
    m_interestRate = interestRate;
}
void Option::setMaturity(double maturity) { m_maturity = maturity; }

} // namespace options