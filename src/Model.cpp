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
    : option_(option) {
  if (!option_) {
    throw std::invalid_argument("Option cannot be null.");
  }
  if (option->getStyle() == options::ExerciseStyle::American) {
    throw std::invalid_argument("Option exercise style must be European");
  }
}

double BlackScholesModel::calculatePrice() const {
  double S = option_->getSpotPrice();
  double K = option_->getStrikePrice();
  double r = option_->getInterestRate();
  double T = option_->getMaturity();
  double sigma = option_->getVolatility();
  double carryRate = option_->getCarry();
  double d1 = utils::d1(S, K, r, sigma, T);
  double d2 = utils::d2(d1, sigma, T);
  switch (option_->getType()) {
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

double BlackScholesModel::calculateDelta() const {
  double S = option_->getSpotPrice();
  double K = option_->getStrikePrice();
  double r = option_->getInterestRate();
  double T = option_->getMaturity();
  double sigma = option_->getVolatility();
  double carryRate = option_->getCarry();
  double d1 = utils::d1(S, K, r, sigma, T);
  switch (option_->getType()) {
    case options::OptionType::Call:
      return utils::normalCDF(d1) * std::exp(-carryRate * T);
    case options::OptionType::Put:
      return (utils::normalCDF(d1) - 1) * std::exp(-carryRate * T);
    default:
      throw std::invalid_argument("Unknown option type.");
  }
}

double BlackScholesModel::calculateGamma() const {
  double S = option_->getSpotPrice();
  double K = option_->getStrikePrice();
  double r = option_->getInterestRate();
  double T = option_->getMaturity();
  double sigma = option_->getVolatility();
  double carryRate = option_->getCarry();
  double d1 = utils::d1(S, K, r, sigma, T);
  return std::exp(-carryRate * T) * utils::normalPDF(d1) /
         (S * sigma * std::sqrt(T));
}

double BlackScholesModel::calculateTheta() const {
  double S = option_->getSpotPrice();
  double K = option_->getStrikePrice();
  double r = option_->getInterestRate();
  double T = option_->getMaturity();
  double sigma = option_->getVolatility();
  double carryRate = option_->getCarry();
  double d1 = utils::d1(S, K, r, sigma, T);
  double d2 = utils::d2(d1, sigma, T);
  switch (option_->getType()) {
    case options::OptionType::Call:
      return (-S * std::exp(-carryRate * T) * utils::normalPDF(d1) * sigma /
              (2 * std::sqrt(T))) +
             (carryRate * S * std::exp(-carryRate * T) * utils::normalCDF(d1)) -
             (r * K * std::exp(-carryRate * T) * utils::normalCDF(d2));
    case options::OptionType::Put:
      return (-S * std::exp(-carryRate * T) * utils::normalPDF(d1) * sigma /
              (2 * std::sqrt(T))) +
             (carryRate * S * std::exp(-carryRate * T) * utils::normalCDF(d1)) +
             (r * K * std::exp(-carryRate * T) * utils::normalCDF(d2));
    default:
      throw std::invalid_argument("Unknown option type.");
  }
}

double BlackScholesModel::calculateVega() const {
  double S = option_->getSpotPrice();
  double K = option_->getStrikePrice();
  double r = option_->getInterestRate();
  double T = option_->getMaturity();
  double sigma = option_->getVolatility();
  double carryRate = option_->getCarry();
  double d1 = utils::d1(S, K, r, sigma, T);
  return std::exp(-carryRate * T) * utils::normalPDF(d1) * S * std::sqrt(T);
}

double BlackScholesModel::calculateRho() const {
  double S = option_->getSpotPrice();
  double K = option_->getStrikePrice();
  double r = option_->getInterestRate();
  double T = option_->getMaturity();
  double sigma = option_->getVolatility();
  double carryRate = option_->getCarry();
  double d1 = utils::d1(S, K, r, sigma, T);
  double d2 = utils::d2(d1, sigma, T);
  switch (option_->getType()) {
    case options::OptionType::Call:
      return T * K * std::exp(-carryRate * T) * utils::normalCDF(d2);
    case options::OptionType::Put:
      return -T * K * std::exp(-carryRate * T) * utils::normalCDF(-d2);
    default:
      throw std::invalid_argument("Unknown option type.");
  }
}

// Binomial Model Implementation
BinomialModel::BinomialModel(const std::shared_ptr<options::Option> option,
                             const int &steps)
    : option_(option), steps_(steps) {
  if (!option_) {
    throw std::invalid_argument("Option cannot be null.");
  }
  if (steps <= 0) {
    throw std::invalid_argument("Number of steps must be a positive integer.");
  }
  uptick_ = exp(option->getVolatility() * sqrt(option->getMaturity() / steps));
  downtick_ = 1 / uptick_;
  probability_ = (exp((option->getInterestRate() - option->getCarry()) *
                      (option->getMaturity() / steps)) -
                  downtick_) /
                 (uptick_ - downtick_);
}
double BinomialModel::getSteps() const { return steps_; }
double BinomialModel::getUptick() const { return uptick_; }
double BinomialModel::getDowntick() const { return downtick_; }
double BinomialModel::getProbability() const { return probability_; }
double BinomialModel::calculatePrice() const { return getUpdatedPayoffs(0)[0]; }
std::vector<double> BinomialModel::getPayoffs() const {
  std::vector<double> payoffs(steps_ + 1);
  for (int i = 0; i <= steps_; i++) {
    double ST =
        option_->getSpotPrice() * pow(uptick_, i) * pow(downtick_, steps_ - i);
    if (option_->getType() == options::OptionType::Call) {
      payoffs[i] = std::max(ST - option_->getStrikePrice(), 0.0);
    } else {
      payoffs[i] = std::max(option_->getStrikePrice() - ST, 0.0);
    }
  }
  return payoffs;
}
std::vector<double> BinomialModel::getUpdatedPayoffs(int i) const {
  auto payoffs = getPayoffs();
  for (int step = steps_ - 1; step >= i; --step) {
    for (int index = 0; index <= step; ++index) {
      double ST = option_->getSpotPrice() * pow(uptick_, index) *
                  pow(downtick_, step - index);
      switch (option_->getStyle()) {
        case (options::ExerciseStyle::American):
          if (option_->getType() == options::OptionType::Call) {
            payoffs[index] =
                std::max(std::max(exp(-option_->getInterestRate() *
                                      option_->getMaturity() / steps_) *
                                      (probability_ * payoffs[index + 1] +
                                       (1.0 - probability_) * payoffs[index]),
                                  0.0),
                         ST - option_->getStrikePrice());
          } else {
            payoffs[index] =
                std::max(std::max(exp(-option_->getInterestRate() *
                                      option_->getMaturity() / steps_) *
                                      (probability_ * payoffs[index + 1] +
                                       (1.0 - probability_) * payoffs[index]),
                                  0.0),
                         option_->getStrikePrice() - ST);
          }
          break;
        case (options::ExerciseStyle::European):
          payoffs[index] = exp(-option_->getInterestRate() *
                               option_->getMaturity() / steps_) *
                           (probability_ * payoffs[index + 1] +
                            (1.0 - probability_) * payoffs[index]);
          break;
        default:
          throw std::invalid_argument("Unknown option exercise style.");
      }
    }
  }
  return payoffs;
}

double BinomialModel::calculateDelta(int i, int j) const {
  if (i < 0 || i >= steps_ || j < 0 || j > i) {
    throw std::out_of_range("Invalid indices for delta calculation.");
  }
  auto updatedPayoffs = getUpdatedPayoffs(i + 1);
  double cU = updatedPayoffs[j + 1];
  double cD = updatedPayoffs[j];
  double sU =
      option_->getSpotPrice() * pow(uptick_, j + 1) * pow(downtick_, i - j - 1);
  double sD = option_->getSpotPrice() * pow(uptick_, j) * pow(downtick_, i - j);
  return (cU - cD) / (sU - sD);
}

double BinomialModel::calculateGamma(int i, int j) const {
  if (i < 0 || i >= steps_ - 1 || j < 0 || j > i) {
    throw std::out_of_range("Invalid indices for gamma calculation.");
  }
  double deltaU = calculateDelta(i + 1, j + 1);
  double deltaD = calculateDelta(i + 1, j);
  double sU =
      option_->getSpotPrice() * pow(uptick_, j + 1) * pow(downtick_, i - j - 1);
  double sD = option_->getSpotPrice() * pow(uptick_, j) * pow(downtick_, i - j);
  return (deltaU - deltaD) / (sU - sD);
}

double BinomialModel::calculateTheta(int i, int j) const {
  if (i < 0 || i >= steps_ - 1 || j < 0 || j > i) {
    throw std::out_of_range("Invalid indices for theta calculation.");
  }
  auto payoffs1 = getUpdatedPayoffs(i);
  auto payoffs2 = getUpdatedPayoffs(i + 2);
  double cU = payoffs1[j];
  double cD = payoffs1[j + 1];
  return (cU - cD) / (365 * option_->getMaturity() / steps_);
}

void BinomialModel::setOption(const std::shared_ptr<options::Option> &option) {
  option_ = option;
  if (!option_) {
    throw std::invalid_argument("Option cannot be null.");
  }
  uptick_ = exp(option->getVolatility() * sqrt(option->getMaturity() / steps_));
  downtick_ = 1 / uptick_;
  probability_ =
      (exp(option->getInterestRate() -
           option->getCarry() * (sqrt(option->getMaturity() / steps_))) -
       downtick_) /
      (uptick_ - downtick_);
}
}  // namespace model
