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
  double d1 = (std::log(S / K) + (r + 0.5 * sigma * sigma) * T) /
              (sigma * std::sqrt(T));
  double d2 = d1 - sigma * std::sqrt(T);
  switch (option_->getType()) {
    case options::OptionType::Call:
      return S * utils::normalCDF(d1) -
             K * std::exp(-r * T) * utils::normalCDF(d2);
    case options::OptionType::Put:
      return K * std::exp(-r * T) * utils::normalCDF(-d2) -
             S * utils::normalCDF(-d1);
    default:
      throw std::invalid_argument("Unknown option type.");
  }
}

BinomialModel::BinomialModel(const std::shared_ptr<options::Option> option,
                             const int &steps)
    : option_(option), steps_(steps) {
  if (!option_) {
    throw std::invalid_argument("Option cannot be null.");
  }
  uptick_ = exp(option->getVolatility() * sqrt(option->getMaturity() / steps));
  downtick_ = 1 / uptick_;
  probability_ =
      (exp(option->getInterestRate() -
           option->getCarry() * (sqrt(option->getMaturity() / steps))) -
       downtick_) /
      (uptick_ - downtick_);
}
double BinomialModel::getSteps() const { return steps_; }
double BinomialModel::getUptick() const { return uptick_; }
double BinomialModel::getDowntick() const { return downtick_; }
double BinomialModel::getProbability() const { return probability_; }
double BinomialModel::calculatePrice() const { return getUpdatedPayoffs()[0]; }
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
std::vector<double> BinomialModel::getUpdatedPayoffs() const {
  auto payoffs = getPayoffs();
  for (int step = steps_ - 1; step >= 0; --step) {
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
          payoffs[index] = std::max(exp(-option_->getInterestRate() *
                                        option_->getMaturity() / steps_) *
                                        (probability_ * payoffs[index + 1] +
                                         (1.0 - probability_) * payoffs[index]),
                                    0.0);
          break;
        default:
          throw std::invalid_argument("Unknown option exercise style.");
      }
    }
  }
  return payoffs;
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
