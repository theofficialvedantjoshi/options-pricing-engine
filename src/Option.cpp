#include <charconv>
#include <options-pricing-engine/Option.hpp>
#include <options-pricing-engine/Types.hpp>
#include <stdexcept>
#include <string_view>

namespace options {
Option::Option(Price spotPrice, Price strikePrice, Rate interestRate,
               std::string_view maturity, Rate volatility, OptionType type,
               ExerciseStyle style, Rate carry)
    : spotPrice_(spotPrice), strikePrice_(strikePrice),
      interestRate_(interestRate), volatility_(volatility), type_(type),
      style_(style), carry_(carry) {
    if (spotPrice <= 0 || strikePrice <= 0 || volatility <= 0) {
        throw std::invalid_argument("Spot price, strike price, and volatility "
                                    "must be positive values.");
    }
    if (maturity.length() < 3 ||
        maturity.substr(maturity.length() - 2, 2) != "mo" ||
        !std::isdigit(maturity[0])) {
        throw std::invalid_argument("Maturity must be a string in the format "
                                    "'Xm' where X is a positive "
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
    // try {
    //     months = std::stod(maturity.substr(0, maturity.length() - 2));
    // } catch (const std::invalid_argument &) {
    //     throw std::invalid_argument("Maturity must be a string in the
    //     format "
    //                                 "'Xm' where X is a positive "
    //                                 "integer representing months.");
    // }
    maturity_ = months / 12.0;
}
Price Option::getSpotPrice() const { return spotPrice_; }
Price Option::getStrikePrice() const { return strikePrice_; }
Rate Option::getInterestRate() const { return interestRate_; }
double Option::getMaturity() const { return maturity_; }
Rate Option::getVolatility() const { return volatility_; }
Rate Option::getCarry() const { return carry_; }
OptionType Option::getType() const { return type_; }
ExerciseStyle Option::getStyle() const { return style_; }

} // namespace options