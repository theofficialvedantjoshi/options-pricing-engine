#pragma once
#include <iostream>
#include <memory>
#include <options-pricing-engine/Model.hpp>
#include <options-pricing-engine/Option.hpp>
#include <options-pricing-engine/Types.hpp>
#include <options-pricing-engine/Utils.hpp>
#include <stdexcept>

std::string_view header = R"(
   ____             __     _                                   ____             _            _                             ______                     _               
  / __ \    ____   / /_   (_)  ____    ____    _____          / __ \   _____   (_)  _____   (_)   ____    ____ _          / ____/   ____    ____ _   (_)   ____   ___ 
 / / / /   / __ \ / __/  / /  / __ \  / __ \  / ___/         / /_/ /  / ___/  / /  / ___/  / /   / __ \  / __ `/         / __/     / __ \  / __ `/  / /   / __ \ / _ \
/ /_/ /   / /_/ // /_   / /  / /_/ / / / / / (__  )         / ____/  / /     / /  / /__   / /   / / / / / /_/ /         / /___    / / / / / /_/ /  / /   / / / //  __/
\____/   / .___/ \__/  /_/   \____/ /_/ /_/ /____/         /_/      /_/     /_/   \___/  /_/   /_/ /_/  \__, /         /_____/   /_/ /_/  \__, /  /_/   /_/ /_/ \___/ 
        /_/                                                                                            /____/                            /____/                       
  )";

constexpr const char* red = "\033[31m";
constexpr const char* blue = "\033[34m";
constexpr const char* green = "\033[32m";
constexpr const char* bold = "\033[1m";

namespace cli {
class CLI {
 public:
  CLI() {};
  void run() {
    int choice;
    do {
      try {
        printMainMenu();
        std::cout << blue << "\nEnter your choice: ";
        std::cin >> choice;

        switch (choice) {
          case 1:
            createOption();
            break;
          case 2:
            if (isOptionSet()) {
              getOption();
            } else {
              exit();
              return;
            }
            break;
          case 3:
            if (isBSMSet()) {
              priceBlackScholesModel();
            } else {
              setBlackScholesModel();
            }
            break;
          case 4:
            if (isBMSet()) {
              priceBinomialModel();
            } else {
              setBinomialModel();
            }
            break;
          case 5:
            exit();
            return;
          default:
            clearScreen();
            std::cout << blue << header << "\n\n";
            std::cout << red << "Invalid choice. Please try again.\n";
        }
        std::cout << blue << "Press Enter to return...";
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cin.get();
      } catch (const std::exception& e) {
        clearScreen();
        std::cout << blue << header << "\n\n";
        std::cerr << red << "Error: " << e.what() << "\n";
        std::cout << "Please Restart the application.\n";
        break;
      }
    } while (true);
  }
  ~CLI() = default;

 private:
  std::shared_ptr<options::Option> m_option;
  std::shared_ptr<model::BlackScholesModel> m_BSM;
  std::shared_ptr<model::BinomialModel> m_BM;
  void clearScreen() const { std::cout << "\033[2J\033[1;1H"; }
  bool isOptionSet() const { return m_option != nullptr; }
  bool isBSMSet() const { return m_BSM != nullptr; }
  bool isBMSet() const { return m_BM != nullptr; }
  void printMainMenu() {
    clearScreen();
    std::cout << blue << bold << header << "\n\n";
    std::cout << "Main Menu:\n";

    isOptionSet() ? std::cout << blue << "1. Update the Option\n"
                  : std::cout << blue << "1. Create an Option\n";
    if (isOptionSet()) {
      std::cout << blue << "2. Get Option\n";
      if ((isBSMSet() && isBMSet()) ||
          (m_option->getStyle() == options::ExerciseStyle::American &&
           isBMSet())) {
        std::cout << blue << "3. Price using Black-Scholes Model\n";
        std::cout << blue << "4. Price using Binomial Model\n";
      } else {
        std::cout << blue << "3. Set Black-Scholes Model\n";
        std::cout << blue << "4. Set Binomial Model\n";
      }
      std::cout << blue << "5. Exit\n";
    } else {
      std::cout << blue << "2. Exit\n";
    }
  }
  void createOption() {
    clearScreen();
    std::cout << header << "\n\n";
    double spotPrice, strikePrice, interestRate, volatility, carryRate;
    std::string maturity;
    int style;
    int type;

    std::cout << blue << "Enter Spot Price 'S' (Ex: 100.0 in $): ";
    std::cin >> spotPrice;
    std::cout << blue << "Enter Strike Price 'K' (Ex: 105.0 in $): ";
    std::cin >> strikePrice;
    std::cout << blue << "Enter Interest Rate 'r' (Ex: 0.05 for 5%): ";
    std::cin >> interestRate;
    std::cout << blue << "Enter Maturity 'T' (In months, Ex: 24mo): ";
    std::cin >> maturity;
    std::cout << blue << "Enter Volatility 'sigma' (Ex: 0.2 for 20%): ";
    std::cin >> volatility;
    std::cout << blue
              << "Enter Carry Rate (Includes dividend yield, convinience "
                 "yield, forex carry, etc., Ex: 0.0 for 0%): ";
    std::cin >> carryRate;

    std::cout << blue << "Enter Option Type (0 for Call, 1 for Put): ";
    std::cin >> type;

    std::cout << blue
              << "Enter Exercise Style (0 for European, 1 for American): ";
    std::cin >> style;

    m_option = std::make_shared<options::Option>(
        spotPrice, strikePrice, interestRate, maturity, volatility,
        static_cast<options::OptionType>(type),
        static_cast<options::ExerciseStyle>(style), carryRate);
  }
  void getOption() const {
    clearScreen();
    std::cout << header << "\n\n";
    if (!isOptionSet()) {
      std::cout << "No option set. Please create an option first.\n";
      return;
    }
    std::cout << "Option Details:\n";
    std::cout << blue << "Spot Price: " << green << m_option->getSpotPrice()
              << " $\n";
    std::cout << blue << "Strike Price: " << green << m_option->getStrikePrice()
              << " $\n";
    std::cout << blue << "Interest Rate: " << green
              << m_option->getInterestRate() * 100 << " %\n";
    std::cout << blue << "Maturity: " << green << m_option->getMaturity() * 12
              << " months\n";
    std::cout << blue << "Volatility: " << green
              << m_option->getVolatility() * 100 << " %\n";
    std::cout << blue << "Carry Rate: " << green << m_option->getCarry() * 100
              << " %\n";
    std::cout << blue << "Type: " << green
              << (m_option->getType() == options::OptionType::Call ? "Call"
                                                                   : "Put")
              << "\n";
    std::cout << blue << "Exercise Style: " << green
              << (m_option->getStyle() == options::ExerciseStyle::European
                      ? "European"
                      : "American")
              << "\n";
  }
  void setBlackScholesModel() {
    clearScreen();
    std::cout << header << "\n\n";
    if (!isOptionSet()) {
      std::cout << red << "No option set. Please create an option first.\n";
      return;
    }
    if (m_option->getStyle() == options::ExerciseStyle::American) {
      std::cout
          << red
          << "Black-Scholes Model is not applicable for American options.\n";
      return;
    }
    if (isBSMSet()) {
      m_BSM->setOption(m_option);
      std::cout << blue << "Black-Scholes Model updated successfully.\n";
      return;
    }
    m_BSM = std::make_shared<model::BlackScholesModel>(m_option);
    std::cout << blue << "Black-Scholes Model set successfully.\n";
  }
  void setBinomialModel() {
    clearScreen();
    std::cout << header << "\n\n";
    if (!isOptionSet()) {
      std::cout << red << "No option set. Please create an option first.\n";
      return;
    }
    int steps;
    std::cout << blue << "Enter number of steps for Binomial Model: ";
    std::cin >> steps;
    m_BM = std::make_shared<model::BinomialModel>(m_option, steps);
    std::cout << blue << "Binomial Model set successfully.\n";
  }
  void priceBlackScholesModel() const {
    clearScreen();
    std::cout << header << "\n\n";
    if (!isBSMSet()) {
      std::cout << red << "Black-Scholes Model not set. Please set it first.\n";
      return;
    }
    if (m_option->getStyle() == options::ExerciseStyle::American) {
      std::cout
          << red
          << "Black-Scholes Model is not applicable for American options.\n";
      return;
    }
    double price = m_BSM->calculatePrice(), delta = m_BSM->calculateDelta(),
           gamma = m_BSM->calculateGamma(), theta = m_BSM->calculateTheta(),
           vega = m_BSM->calculateVega(), rho = m_BSM->calculateRho();
    std::cout << blue << "Black-Scholes Price: " << green << price << " $\n";
    std::cout << blue << "Option Delta: " << green << delta << "\n";
    std::cout << blue << "Option Gamma: " << green << gamma << "\n";
    std::cout << blue << "Option Theta: " << green << theta << "\n";
    std::cout << blue << "Option Vega: " << green << vega << "\n";
    std::cout << blue << "Option Rho: " << green << rho << "\n";
    std::cout << red << "Note: All Greeks are calculated at the current option "
              << "parameters.\n";
  }
  void priceBinomialModel() const {
    clearScreen();
    std::cout << header << "\n\n";
    if (!isBMSet()) {
      std::cout << red << "Binomial Model not set. Please set it first.\n";
      return;
    }
    double price = m_BM->calculatePrice(), uptick = m_BM->getUptick(),
           downtick = m_BM->getDowntick(), probability = m_BM->getProbability();
    int i, j;
    std::cout << blue << "Binomial Price: " << green << price << " $\n";
    std::cout << blue << "Uptick ^: " << green << uptick << "\n";
    std::cout << blue << "Downtick v: " << green << downtick << "\n";
    std::cout << blue << "Probability p: " << green << probability << "\n";
    std::cout << blue
              << "To Calculate Greeks, specify indices (i, j) in the Binomial "
                 "Tree. i must be less than "
              << red << m_BM->getSteps() - 1 << blue
              << " and j must be less than i.\nIf its not possible proceed "
                 "with (-1,-1)\n";
    std::cout << blue << "Enter i (step index): ";
    std::cin >> i;
    std::cout << blue << "Enter j (node index at step i): ";
    std::cin >> j;
    if (i == -1 && j == -1) {
      return;
    }
    std::cout << blue << "Delta: " << green << m_BM->calculateDelta(i, j)
              << "\n";
    std::cout << blue << "Gamma: " << green << m_BM->calculateGamma(i, j)
              << "\n";
    std::cout << blue << "Theta: " << green << m_BM->calculateTheta(i, j)
              << "\n";
  }
  void exit() const {
    clearScreen();
    std::cout << red << "Goodbye!\n";
  }
};
}  // namespace cli