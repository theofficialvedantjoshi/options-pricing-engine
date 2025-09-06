#include <functional>
#include <iostream>
#include <memory>
#include <options-pricing-engine/Cli.hpp>
#include <options-pricing-engine/Model.hpp>
#include <options-pricing-engine/Option.hpp>
#include <options-pricing-engine/Types.hpp>
#include <options-pricing-engine/Utils.hpp>
#include <stdexcept>

namespace cli {

std::vector<Command> CLI::generateMenu() {
    std::vector<Command> commands;
    commands.push_back({"Create/Update Option", [this] { createOption(); },
                        [] { return true; }});

    commands.push_back({"Get Option", [this] { getOption(); },
                        [this] { return isOptionSet(); }});

    commands.push_back(
        {"Set Black-Scholes Model", [this] { setBlackScholesModel(); },
         [this] {
             return isOptionSet() && !isBSMSet() &&
                    m_option->getStyle() == options::ExerciseStyle::European;
         }});

    commands.push_back({"Price with Black-Scholes Model",
                        [this] { priceBlackScholesModel(); },
                        [this] { return isBSMSet(); }});

    commands.push_back({"Set Binomial Model", [this] { setBinomialModel(); },
                        [this] { return isOptionSet() && !isBMSet(); }});

    commands.push_back({"Price with Binomial Model",
                        [this] { priceBinomialModel(); },
                        [this] { return isBMSet(); }});
    commands.push_back(
        {"Set Monte Carlo Model", [this] { setMonteCarloModel(); },
         [this] {
             return isOptionSet() && !isMCSet() &&
                    m_option->getStyle() == options::ExerciseStyle::European;
         }});

    commands.push_back({"Price with Monte Carlo Model",
                        [this] { priceMonteCarloModel(); },
                        [this] { return isMCSet(); }});
    commands.push_back({"Calculate Implied Volatility",
                        [this] { getImpliedVolatility(); },
                        [this] { return isBSMSet(); }});

    commands.push_back({"Exit", [this] { exit(); }, [] { return true; }, true});

    std::vector<Command> validCommands;
    for (const auto &command : commands) {
        if (!command.valid || command.valid()) {
            validCommands.push_back(command);
        }
    }
    return validCommands;
}
void CLI::createOption() {
    clearScreen();
    std::cout << header << "\n\n";
    Price spotPrice, strikePrice;
    Rate interestRate, volatility, carryRate;
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
        spotPrice, strikePrice, interestRate,
        static_cast<std::string_view>(maturity), volatility,
        static_cast<options::OptionType>(type),
        static_cast<options::ExerciseStyle>(style), carryRate);
}
void CLI::setBlackScholesModel() {
    clearScreen();
    std::cout << header << "\n\n";
    if (!isOptionSet()) {
        std::cout << red << "No option set. Please create an option first.\n";
        return;
    }
    if (m_option->getStyle() == options::ExerciseStyle::American) {
        std::cout << red
                  << "Black-Scholes Model is not applicable for American "
                     "options.\n";
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
void CLI::setBinomialModel() {
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
void CLI::setMonteCarloModel() {
    clearScreen();
    std::cout << header << "\n\n";
    if (!isOptionSet()) {
        std::cout << red << "No option set. Please create an option first.\n";
        return;
    }
    if (m_option->getStyle() == options::ExerciseStyle::American) {
        std::cout << red
                  << "Monte Carlo Model does not support American options\n";
        return;
    }
    if (isMCSet()) {
        m_MC->setOption(m_option);
        std::cout << blue << "Monte Carlo Model updated successfully.\n";
        return;
    }
    int N;
    std::cout << blue
              << "Enter number of iterations for the Monte Carlo Model: ";
    std::cin >> N;
    m_MC = std::make_shared<model::MonteCarloModel>(m_option, N);
    std::cout << blue << "Monte Carlo Model set successfully.\n";
}
void CLI::priceBlackScholesModel() const {
    clearScreen();
    std::cout << header << "\n\n";
    if (!isBSMSet()) {
        std::cout << red
                  << "Black-Scholes Model not set. Please set it first.\n";
        return;
    }
    if (m_option->getStyle() == options::ExerciseStyle::American) {
        std::cout << red
                  << "Black-Scholes Model is not applicable for American "
                     "options.\n";
        return;
    }
    Price price = m_BSM->calculatePrice();
    Greek delta = m_BSM->calculateDelta(), gamma = m_BSM->calculateGamma(),
          theta = m_BSM->calculateTheta(), vega = m_BSM->calculateVega(),
          rho = m_BSM->calculateRho();
    std::cout << blue << "Black-Scholes Price: " << green << price << " $\n";
    std::cout << blue << "Option Delta: " << green << delta << "\n";
    std::cout << blue << "Option Gamma: " << green << gamma << "\n";
    std::cout << blue << "Option Theta: " << green << theta << "\n";
    std::cout << blue << "Option Vega: " << green << vega << "\n";
    std::cout << blue << "Option Rho: " << green << rho << "\n";
    std::cout << red << "Note: All Greeks are calculated at the current option "
              << "parameters.\n";
}
void CLI::priceBinomialModel() const {
    clearScreen();
    std::cout << header << "\n\n";
    if (!isBMSet()) {
        std::cout << red << "Binomial Model not set. Please set it first.\n";
        return;
    }
    Price price = m_BM->calculatePrice();
    double uptick = m_BM->getUptick(), downtick = m_BM->getDowntick(),
           probability = m_BM->getProbability();
    int i, j;
    std::cout << blue << "Binomial Price: " << green << price << " $\n";
    std::cout << blue << "Uptick ^: " << green << uptick << "\n";
    std::cout << blue << "Downtick v: " << green << downtick << "\n";
    std::cout << blue << "Probability p: " << green << probability << "\n";
    std::cout << blue
              << "To Calculate Greeks, specify indices (i, j) in the "
                 "Binomial "
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
void CLI::priceMonteCarloModel() const {
    clearScreen();
    std::cout << header << "\n\n";
    if (!isMCSet()) {
        std::cout << red << "Monte Carlo Model not set. Please set it first.\n";
        return;
    }
    if (m_option->getStyle() == options::ExerciseStyle::American) {
        std::cout << red
                  << "Monte Carlo Model does not support American "
                     "options.\n";
        return;
    }
    Price price = m_MC->calculatePrice();
    const int N = m_MC->getN();
    std::cout << blue << "Monte Carlo Iterations: " << green << N << "\n";
    std::cout << blue << "Monte Carlo Price: " << green << price << " $\n";
}
void CLI::getImpliedVolatility() const {
    clearScreen();
    std::cout << header << "\n\n";
    Price marketPrice;
    std::cout << blue << "Enter Market Price (Ex: 100.0 in $): ";
    std::cin >> marketPrice;
    Rate IV = m_BSM->calculateIV(marketPrice);
    std::cout << blue << "Implied Volatility: " << green << IV * 100 << " %\n";
}
}; // namespace cli