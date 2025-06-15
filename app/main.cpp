#include <iostream>
#include <options-pricing-engine/Model.hpp>
#include <options-pricing-engine/Option.hpp>
#include <options-pricing-engine/Types.hpp>
#include <options-pricing-engine/Utils.hpp>

void printOptionDetails(const std::shared_ptr<options::Option>& option) {
  std::cout << "OPTION:" << std::endl;
  std::cout << "-> Spot Price: $" << option->getSpotPrice() << std::endl;
  std::cout << "-> Strike Price: $" << option->getStrikePrice() << std::endl;
  std::cout << "-> Interest Rate: " << option->getInterestRate() * 100 << "%"
            << std::endl;
  std::cout << "-> Maturity: " << option->getMaturity() * 12 << " months"
            << std::endl;
  std::cout << "-> Volatility: " << option->getVolatility() * 100 << "%"
            << std::endl;
  std::cout << "-> Type: "
            << (option->getType() == options::OptionType::Call ? "Call" : "Put")
            << std::endl;
  std::cout << "-> Style: "
            << (option->getStyle() == options::ExerciseStyle::American
                    ? "American"
                    : "European")
            << std::endl;
  std::cout << "-> Carry: " << option->getCarry() * 100 << "%" << std::endl;
  std::cout << std::endl;
}

void printModelDetails(model::BlackScholesModel& BSM,
                       model::BinomialModel& BM) {
  std::cout << "Black-Scholes Model Price: $" << BSM.calculatePrice()
            << std::endl;
  std::cout << "Binomial Model: " << std::endl;
  std::cout << "-> Steps: " << BM.getSteps() << std::endl;
  std::cout << "-> Uptick: " << BM.getUptick() << std::endl;
  std::cout << "-> Downtick: " << BM.getDowntick() << std::endl;
  std::cout << "-> Probability: " << BM.getProbability() << std::endl;
  std::cout << "Binomial Model Price: $" << BM.calculatePrice() << std::endl;
  std::cout << std::endl;
}

int main() {
  auto europeanPut = std::make_shared<options::Option>(
      50.0, 52.0, 0.05, "24mo", 0.3, options::OptionType::Put,
      options::ExerciseStyle::European);
  auto europeanCall = std::make_shared<options::Option>(
      50.0, 52.0, 0.05, "24mo", 0.3, options::OptionType::Call,
      options::ExerciseStyle::European);
  auto americanCall = std::make_shared<options::Option>(
      50.0, 52.0, 0.05, "24mo", 0.3, options::OptionType::Call,
      options::ExerciseStyle::American);
  auto americanPut = std::make_shared<options::Option>(
      50.0, 52.0, 0.05, "24mo", 0.3, options::OptionType::Put,
      options::ExerciseStyle::American);
  printOptionDetails(europeanPut);

  model::BlackScholesModel BSM(europeanPut);
  model::BinomialModel BM(europeanPut, 2);
  printModelDetails(BSM, BM);
  printOptionDetails(europeanCall);
  BSM.setOption(europeanCall);
  BM.setOption(europeanCall);
  printModelDetails(BSM, BM);
  printOptionDetails(americanCall);
  BSM.setOption(americanCall);
  BM.setOption(americanCall);
  printModelDetails(BSM, BM);
  printOptionDetails(americanPut);
  BSM.setOption(americanPut);
  BM.setOption(americanPut);
  printModelDetails(BSM, BM);
  std::cout << "================" << std::endl;
  return 0;
}
