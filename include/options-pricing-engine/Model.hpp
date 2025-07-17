#pragma once
#include <cmath>
#include <memory>
#include <options-pricing-engine/Option.hpp>
#include <options-pricing-engine/Types.hpp>
#include <options-pricing-engine/Utils.hpp>
#include <stdexcept>
#include <string>
#include <vector>

namespace model {
class Model {
 public:
  virtual ~Model() = default;
  virtual double calculatePrice() const = 0;
  virtual void setOption(const std::shared_ptr<options::Option> &option) = 0;
};

class BlackScholesModel : public Model {
 public:
  BlackScholesModel(const std::shared_ptr<options::Option> &option);
  double calculatePrice() const override;
  double calculateDelta() const;
  double calculateGamma() const;
  double calculateTheta() const;
  double calculateVega() const;
  double calculateRho() const;
  void setOption(const std::shared_ptr<options::Option> &option) override {
    option_ = option;
  }

 private:
  std::shared_ptr<options::Option> option_;
};

class BinomialModel : public Model {
 public:
  BinomialModel(const std::shared_ptr<options::Option> option,
                const int &steps);
  double getSteps() const;
  double getUptick() const;
  double getDowntick() const;
  double getProbability() const;
  double calculatePrice() const override;
  double calculateDelta(int i, int j) const;
  double calculateGamma(int i, int j) const;
  double calculateTheta(int i, int j) const;
  void setOption(const std::shared_ptr<options::Option> &option) override;

 private:
  std::shared_ptr<options::Option> option_;
  int steps_;
  double uptick_;
  double downtick_;
  double probability_;
  std::vector<double> getPayoffs() const;
  std::vector<double> getUpdatedPayoffs(int i) const;
};
}  // namespace model
