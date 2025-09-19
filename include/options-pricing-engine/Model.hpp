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
    virtual Price calculatePrice() const = 0;
    virtual void setOption(const std::shared_ptr<options::Option> &option) = 0;
};

class BlackScholesModel : public Model {
  public:
    BlackScholesModel(const std::shared_ptr<options::Option> &option);
    Price calculatePrice() const override;
    Greek calculateDelta() const;
    Greek calculateGamma() const;
    Greek calculateTheta() const;
    Greek calculateVega() const;
    Greek calculateRho() const;
    Rate calculateIV(const Price marketPrice) const;
    void setOption(const std::shared_ptr<options::Option> &option) override {
        m_option = option;
    }

  private:
    std::shared_ptr<options::Option> m_option;
};

class BinomialModel : public Model {
  public:
    BinomialModel(const std::shared_ptr<options::Option> option,
                  const int &steps);
    int getSteps() const;
    double getUptick() const;
    double getDowntick() const;
    double getProbability() const;
    Price calculatePrice() const override;
    Greek calculateDelta(int i, int j) const;
    Greek calculateGamma(int i, int j) const;
    Greek calculateTheta(int i, int j) const;
    void setOption(const std::shared_ptr<options::Option> &option) override;

  private:
    std::shared_ptr<options::Option> m_option;
    int m_steps;
    double m_uptick;
    double m_downtick;
    double m_probability;
    std::vector<Price> getPayoffs() const;
    std::vector<Price> getUpdatedPayoffs(const int i) const;
};
class MonteCarloModel : public Model {
  public:
    MonteCarloModel(const std::shared_ptr<options::Option> &option,
                    const int &N);
    int getN() const { return m_N; }
    Price calculatePrice() const override;
    Greek calculateDelta() const;
    Greek calculateGamma() const;
    Greek calculateTheta() const;
    Greek calculateVega() const;
    Greek calculateRho() const;
    void setOption(const std::shared_ptr<options::Option> &option) override {
        m_option = option;
    }

  private:
    std::shared_ptr<options::Option> m_option;
    int m_N;
    std::vector<Price> getStockPrices() const;
    std::vector<Price> getPayoffs() const;
};
} // namespace model
