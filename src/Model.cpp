#include <cmath>
#include <iostream>
#include <memory>
#include <options-pricing-engine/Model.hpp>
#include <options-pricing-engine/Option.hpp>
#include <options-pricing-engine/Types.hpp>
#include <options-pricing-engine/Utils.hpp>
#include <stdexcept>
#include <vector>

namespace model {
// Black-Scholes Model Implementation
BlackScholesModel::BlackScholesModel(
    const std::shared_ptr<options::Option> &option)
    : m_option(option) {
    if (!m_option) {
        throw std::invalid_argument("Option cannot be null.");
    }
    if (option->getStyle() == options::ExerciseStyle::American) {
        throw std::invalid_argument("Option exercise style must be European");
    }
}

Price BlackScholesModel::calculatePrice() const {
    Price S = m_option->getSpotPrice();
    Price K = m_option->getStrikePrice();
    Rate r = m_option->getInterestRate();
    double T = m_option->getMaturity();
    Rate sigma = m_option->getVolatility();
    Rate carryRate = m_option->getCarry();
    double d1 = utils::d1(S, K, r, sigma, T, carryRate);
    double d2 = utils::d2(d1, sigma, T);
    switch (m_option->getType()) {
    case options::OptionType::Call:
        return S * utils::normalCDF(d1) * std::exp(-carryRate * T) -
               K * std::exp(-r * T) * utils::normalCDF(d2);
    case options::OptionType::Put:
        return K * std::exp(-r * T) * utils::normalCDF(-d2) -
               S * utils::normalCDF(-d1) * std::exp(-carryRate * T);
    default:
        throw std::invalid_argument("Unknown option type.");
    }
}

Greek BlackScholesModel::calculateDelta() const {
    Price S = m_option->getSpotPrice();
    Price K = m_option->getStrikePrice();
    Rate r = m_option->getInterestRate();
    double T = m_option->getMaturity();
    Rate sigma = m_option->getVolatility();
    Rate carryRate = m_option->getCarry();
    double d1 = utils::d1(S, K, r, sigma, T, carryRate);
    switch (m_option->getType()) {
    case options::OptionType::Call:
        return utils::normalCDF(d1) * std::exp(-carryRate * T);
    case options::OptionType::Put:
        return (utils::normalCDF(d1) - 1) * std::exp(-carryRate * T);
    default:
        throw std::invalid_argument("Unknown option type.");
    }
}

Greek BlackScholesModel::calculateGamma() const {
    double S = m_option->getSpotPrice();
    double K = m_option->getStrikePrice();
    double r = m_option->getInterestRate();
    double T = m_option->getMaturity();
    double sigma = m_option->getVolatility();
    double carryRate = m_option->getCarry();
    double d1 = utils::d1(S, K, r, sigma, T, carryRate);
    return std::exp(-carryRate * T) * utils::normalPDF(d1) /
           (S * sigma * std::sqrt(T));
}

Greek BlackScholesModel::calculateTheta() const {
    Price S = m_option->getSpotPrice();
    Price K = m_option->getStrikePrice();
    Rate r = m_option->getInterestRate();
    double T = m_option->getMaturity();
    Rate sigma = m_option->getVolatility();
    Rate carryRate = m_option->getCarry();
    double d1 = utils::d1(S, K, r, sigma, T, carryRate);
    double d2 = utils::d2(d1, sigma, T);
    switch (m_option->getType()) {
    case options::OptionType::Call:
        return (-S * std::exp(-carryRate * T) * utils::normalPDF(d1) * sigma /
                (2 * std::sqrt(T))) +
               (carryRate * S * std::exp(-carryRate * T) *
                utils::normalCDF(d1)) -
               (r * K * std::exp(-carryRate * T) * utils::normalCDF(d2));
    case options::OptionType::Put:
        return (-S * std::exp(-carryRate * T) * utils::normalPDF(d1) * sigma /
                (2 * std::sqrt(T))) +
               (carryRate * S * std::exp(-carryRate * T) *
                utils::normalCDF(d1)) +
               (r * K * std::exp(-carryRate * T) * utils::normalCDF(d2));
    default:
        throw std::invalid_argument("Unknown option type.");
    }
}

Greek BlackScholesModel::calculateVega() const {
    Price S = m_option->getSpotPrice();
    Price K = m_option->getStrikePrice();
    Rate r = m_option->getInterestRate();
    double T = m_option->getMaturity();
    Rate sigma = m_option->getVolatility();
    Rate carryRate = m_option->getCarry();
    double d1 = utils::d1(S, K, r, sigma, T, carryRate);
    return std::exp(-carryRate * T) * utils::normalPDF(d1) * S * std::sqrt(T);
}

Greek BlackScholesModel::calculateRho() const {
    double S = m_option->getSpotPrice();
    double K = m_option->getStrikePrice();
    double r = m_option->getInterestRate();
    double T = m_option->getMaturity();
    double sigma = m_option->getVolatility();
    double carryRate = m_option->getCarry();
    double d1 = utils::d1(S, K, r, sigma, T, carryRate);
    double d2 = utils::d2(d1, sigma, T);
    switch (m_option->getType()) {
    case options::OptionType::Call:
        return T * K * std::exp(-carryRate * T) * utils::normalCDF(d2);
    case options::OptionType::Put:
        return -T * K * std::exp(-carryRate * T) * utils::normalCDF(-d2);
    default:
        throw std::invalid_argument("Unknown option type.");
    }
}

Rate BlackScholesModel::calculateIV(const Price marketPrice) const {
    Rate sigma = m_option->getVolatility();
    Rate IV = 0.2;
    constexpr int MAX_ITER = 1e3;
    constexpr double tolerance = 1e-6;
    for (int i = 0; i < MAX_ITER; ++i) {
        m_option->setVolatility(IV);
        Price theoreticalPrice = calculatePrice();
        double delta = theoreticalPrice - marketPrice;
        if (std::fabs(delta) < tolerance) {
            m_option->setVolatility(sigma);
            return IV;
        }
        Greek vega = calculateVega();
        if (vega < 1e-8) {
            break;
        }
        IV -= delta / vega;
        if (IV <= 0.0) {
            IV = tolerance;
        }
    }
    m_option->setVolatility(sigma);
    throw std::runtime_error(
        "Implied Volatility did not converge. Verify parameters.");
}
// Binomial Model Implementation
BinomialModel::BinomialModel(const std::shared_ptr<options::Option> option,
                             const int &steps)
    : m_option(option), m_steps(steps) {
    if (!m_option) {
        throw std::invalid_argument("Option cannot be null.");
    }
    if (steps <= 0) {
        throw std::invalid_argument(
            "Number of steps must be a positive integer.");
    }
    m_uptick =
        exp(option->getVolatility() * sqrt(option->getMaturity() / steps));
    m_downtick = 1 / m_uptick;
    m_probability = (exp((option->getInterestRate() - option->getCarry()) *
                         (option->getMaturity() / steps)) -
                     m_downtick) /
                    (m_uptick - m_downtick);
}
int BinomialModel::getSteps() const { return m_steps; }
double BinomialModel::getUptick() const { return m_uptick; }
double BinomialModel::getDowntick() const { return m_downtick; }
double BinomialModel::getProbability() const { return m_probability; }
Price BinomialModel::calculatePrice() const { return getUpdatedPayoffs(0)[0]; }
std::vector<Price> BinomialModel::getPayoffs() const {
    std::vector<Price> payoffs(m_steps + 1);
    for (int i = 0; i <= m_steps; i++) {
        Price ST = m_option->getSpotPrice() * pow(m_uptick, i) *
                   pow(m_downtick, m_steps - i);
        if (m_option->getType() == options::OptionType::Call) {
            payoffs[i] = std::max(ST - m_option->getStrikePrice(), 0.0);
        } else {
            payoffs[i] = std::max(m_option->getStrikePrice() - ST, 0.0);
        }
    }
    return payoffs;
}
std::vector<Price> BinomialModel::getUpdatedPayoffs(int i) const {
    auto payoffs = getPayoffs();
    for (int step = m_steps - 1; step >= i; --step) {
        for (int index = 0; index <= step; ++index) {
            Price ST = m_option->getSpotPrice() * pow(m_uptick, index) *
                       pow(m_downtick, step - index);
            switch (m_option->getStyle()) {
            case (options::ExerciseStyle::American):
                if (m_option->getType() == options::OptionType::Call) {
                    payoffs[index] = std::max(
                        std::max(exp(-m_option->getInterestRate() *
                                     m_option->getMaturity() / m_steps) *
                                     (m_probability * payoffs[index + 1] +
                                      (1.0 - m_probability) * payoffs[index]),
                                 0.0),
                        ST - m_option->getStrikePrice());
                } else {
                    payoffs[index] = std::max(
                        std::max(exp(-m_option->getInterestRate() *
                                     m_option->getMaturity() / m_steps) *
                                     (m_probability * payoffs[index + 1] +
                                      (1.0 - m_probability) * payoffs[index]),
                                 0.0),
                        m_option->getStrikePrice() - ST);
                }
                break;
            case (options::ExerciseStyle::European):
                payoffs[index] = exp(-m_option->getInterestRate() *
                                     m_option->getMaturity() / m_steps) *
                                 (m_probability * payoffs[index + 1] +
                                  (1.0 - m_probability) * payoffs[index]);
                break;
            default:
                throw std::invalid_argument("Unknown option exercise style.");
            }
        }
    }
    return payoffs;
}

Greek BinomialModel::calculateDelta(int i, int j) const {
    if (i < 0 || i >= m_steps || j < 0 || j > i) {
        throw std::out_of_range("Invalid indices for delta calculation.");
    }
    auto updatedPayoffs = getUpdatedPayoffs(i + 1);
    Price cU = updatedPayoffs[j + 1];
    Price cD = updatedPayoffs[j];
    Price sU = m_option->getSpotPrice() * pow(m_uptick, j + 1) *
               pow(m_downtick, i - j - 1);
    Price sD =
        m_option->getSpotPrice() * pow(m_uptick, j) * pow(m_downtick, i - j);
    return (cU - cD) / (sU - sD);
}

Greek BinomialModel::calculateGamma(int i, int j) const {
    if (i < 0 || i >= m_steps - 1 || j < 0 || j > i) {
        throw std::out_of_range("Invalid indices for gamma calculation.");
    }
    Greek deltaU = calculateDelta(i + 1, j + 1);
    Greek deltaD = calculateDelta(i + 1, j);
    Price sU = m_option->getSpotPrice() * pow(m_uptick, j + 1) *
               pow(m_downtick, i - j - 1);
    Price sD =
        m_option->getSpotPrice() * pow(m_uptick, j) * pow(m_downtick, i - j);
    return (deltaU - deltaD) / (sU - sD);
}

Greek BinomialModel::calculateTheta(int i, int j) const {
    if (i < 0 || i >= m_steps - 1 || j < 0 || j > i) {
        throw std::out_of_range("Invalid indices for theta calculation.");
    }
    auto payoffs1 = getUpdatedPayoffs(i);
    auto payoffs2 = getUpdatedPayoffs(i + 2);
    Price cU = payoffs1[j];
    Price cD = payoffs1[j + 1];
    return (cU - cD) / (365 * m_option->getMaturity() / m_steps);
}

void BinomialModel::setOption(const std::shared_ptr<options::Option> &option) {
    m_option = option;
    if (!m_option) {
        throw std::invalid_argument("Option cannot be null.");
    }
    m_uptick =
        exp(option->getVolatility() * sqrt(option->getMaturity() / m_steps));
    m_downtick = 1 / m_uptick;
    m_probability =
        (exp(option->getInterestRate() -
             option->getCarry() * (sqrt(option->getMaturity() / m_steps))) -
         m_downtick) /
        (m_uptick - m_downtick);
}

MonteCarloModel::MonteCarloModel(const std::shared_ptr<options::Option> &option,
                                 const int &N)
    : m_option(option), m_N(N) {
    if (N < 0) {
        throw std::invalid_argument(
            "N.o of iterations must be a positive integer.");
    }
    if (!m_option) {
        throw std::invalid_argument("Option cannot be null.");
    }
    if (option->getStyle() == options::ExerciseStyle::American) {
        throw std::invalid_argument("Option exercise style must be European");
    }
}
std::vector<Price> MonteCarloModel::getStockPrices() const {
    std::vector<Price> stockPrices(m_N);
    auto Z = utils::generateSamples(m_N);
    Price S0 = m_option->getSpotPrice();
    Rate sigma = m_option->getVolatility();
    Rate carry = m_option->getCarry();
    Rate r = m_option->getInterestRate();
    double T = m_option->getMaturity();
    for (int i = 0; i < m_N; ++i) {
        stockPrices[i] = S0 * std::exp((r - carry - 0.5 * sigma * sigma) * T +
                                       sigma * std::sqrt(T) * Z[i]);
    }
    return stockPrices;
}
std::vector<Price> MonteCarloModel::getPayoffs() const {
    std::vector<Price> payoffs(m_N);
    auto stockPrices = getStockPrices();
    Price K = m_option->getStrikePrice();
    switch (m_option->getType()) {
    case (options::OptionType::Call):
        for (int i = 0; i < m_N; ++i) {
            payoffs[i] = std::max(stockPrices[i] - K, 0.0);
        }
        break;
    case (options::OptionType::Put):
        for (int i = 0; i < m_N; ++i) {
            payoffs[i] = std::max(K - stockPrices[i], 0.0);
        }
        break;
    default:
        throw std::invalid_argument("Unknown option type.");
    }
    return payoffs;
}
Price MonteCarloModel::calculatePrice() const {
    auto payoffs = getPayoffs();
    Rate r = m_option->getInterestRate();
    double T = m_option->getMaturity();
    return std::exp(-r * T) *
           (std::accumulate(payoffs.begin(), payoffs.end(), 0.0) / m_N);
}
} // namespace model
