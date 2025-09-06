#pragma once
#include <functional>
#include <iostream>
#include <memory>
#include <options-pricing-engine/Model.hpp>
#include <options-pricing-engine/Option.hpp>
#include <options-pricing-engine/Types.hpp>
#include <options-pricing-engine/Utils.hpp>
#include <stdexcept>

namespace cli {
constexpr const char *header = R"(
   ____             __     _                                   ____             _            _                             ______                     _               
  / __ \    ____   / /_   (_)  ____    ____    _____          / __ \   _____   (_)  _____   (_)   ____    ____ _          / ____/   ____    ____ _   (_)   ____   ___ 
 / / / /   / __ \ / __/  / /  / __ \  / __ \  / ___/         / /_/ /  / ___/  / /  / ___/  / /   / __ \  / __ `/         / __/     / __ \  / __ `/  / /   / __ \ / _ \
/ /_/ /   / /_/ // /_   / /  / /_/ / / / / / (__  )         / ____/  / /     / /  / /__   / /   / / / / / /_/ /         / /___    / / / / / /_/ /  / /   / / / //  __/
\____/   / .___/ \__/  /_/   \____/ /_/ /_/ /____/         /_/      /_/     /_/   \___/  /_/   /_/ /_/  \__, /         /_____/   /_/ /_/  \__, /  /_/   /_/ /_/ \___/ 
        /_/                                                                                            /____/                            /____/                       
  )";

constexpr const char *red = "\033[31m";
constexpr const char *blue = "\033[34m";
constexpr const char *green = "\033[32m";
constexpr const char *bold = "\033[1m";
struct Command {
    std::string description;
    std::function<void()> func;
    std::function<bool()> valid;
    bool exitAfter{false};
};
class CLI {
  public:
    CLI() {};
    void run() {
        do {
            try {
                clearScreen();
                std::cout << blue << bold << header << "\n\n";

                auto commands = generateMenu();
                std::cout << "Main Menu:\n";
                for (int i = 0; i < commands.size(); ++i) {
                    std::cout << blue << i + 1 << ". "
                              << commands[i].description << "\n";
                }
                std::cout << blue << "\nEnter your choice: ";
                int choice;
                std::cin >> choice;
                if (choice < 1 || choice > commands.size()) {
                    std::cout << red << "Invalid choice. Try again.\n";
                } else {
                    commands[choice - 1].func();
                    if (commands[choice - 1].exitAfter)
                        break;
                }

                std::cout << blue << "Press Enter to return...";
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(),
                                '\n');
                std::cin.get();
            } catch (const std::exception &e) {
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
    std::shared_ptr<model::MonteCarloModel> m_MC;
    void clearScreen() const { std::cout << "\033[2J\033[1;1H"; }
    bool isOptionSet() const { return m_option != nullptr; }
    bool isBSMSet() const { return m_BSM != nullptr; }
    bool isBMSet() const { return m_BM != nullptr; }
    bool isMCSet() const { return m_MC != nullptr; }

    std::vector<Command> generateMenu();
    void createOption();
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
        std::cout << blue << "Strike Price: " << green
                  << m_option->getStrikePrice() << " $\n";
        std::cout << blue << "Interest Rate: " << green
                  << m_option->getInterestRate() * 100 << " %\n";
        std::cout << blue << "Maturity: " << green
                  << m_option->getMaturity() * 12 << " months\n";
        std::cout << blue << "Volatility: " << green
                  << m_option->getVolatility() * 100 << " %\n";
        std::cout << blue << "Carry Rate: " << green
                  << m_option->getCarry() * 100 << " %\n";
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
    void setBlackScholesModel();
    void setBinomialModel();
    void priceBlackScholesModel() const;
    void priceBinomialModel() const;
    void setMonteCarloModel();
    void priceMonteCarloModel() const;
    void getImpliedVolatility() const;
    void exit() const {
        clearScreen();
        std::cout << red << "Goodbye!\n";
    }
};
} // namespace cli