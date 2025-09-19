# C++ Options Pricing Engine

A C++ model and CLI for pricing Options using the Black-Scholes, Binomial Tree methods and Monte Carlo Simulations. This project provides a simple implementation of option pricing models, allowing users to create and price different types of options.

## Features

- An Option class used to model different types of options with parameters including type, exercise style and yield rate(dividend yield) etc.
- A Black-Scholes model for pricing European options.
- A Binomial Tree model for pricing both European and American options.
- A Monte Carlo simulation model for pricing European options.
- Calculation of option Greeks (Delta, Gamma, Theta, Vega, Rho) for each pricing model.
- Calculation of implied volatility based on the Black-Scholes model.
- An interactive command-line interface (CLI) for creating and pricing options.

## Setup

1. Clone the repository.
2. Navigate to the project directory.
3. Create a build directory and navigate into it:

   ```bash
   mkdir build && cd build
   ```

4. Run CMake to configure the project:

   ```bash
   cmake ..
   ```

5. Build the project:

   ```bash
   cmake --build .
    ```

6. Run the main executable:

   ```bash
   ./options_pricing_engine
   ```

## Option Configuration

Options can be configured using the `option.toml` configuration file, which contains default parameters that can be customized for your pricing needs. 

### Configuration File Structure

The `option.toml` file allows you to specify:

- Option type (Call/Put)
- Exercise style (European/American)
- Strike price, spot price, and time to expiration
- Risk-free rate and dividend yield
- Volatility parameters

### Usage

1. Modify the parameters in `option.toml` to match your desired option contract
2. Rebuild and run the project (repeat setup steps 4-6)
3. The engine will automatically load your configuration when started

Example configuration:

```toml
[option]
spot = 100.0      # Spot Price in USD
strike = 105.0    # Strike Price in USD
interest = 0.05   # Risk free interest rate (Ex: 0.05 for 5%)
volatility = 0.2  # Volatility of the underlying asset (Ex: 0.2 for 20%)
maturity = "12mo" # Maturity of the contract (In months, Ex: 24mo)
yield = 0.0       # Yield of the underlying asset, this includes dividend yield, convinience yield, forex yield, etc. (Ex: 0.0 for 0%)
type = 0      # Option type (0 for Call, 1 for Put)
style = 0 # Exercise style (0 for European, 1 for American)
```

## TODO

- [X] Add Option greeks.
- [X] Add an interactive CLI.
- [X] Add support for implied volatilities.
- [X] Add new pricing models such as Monte Carlo simulation.
- [X] Calculate Greeks for Monte Carlo simulation.
- [X] Load option parameters from a configuration file (TOML).
- [ ] Add plotting capabilities to visualize option pricing.
- [ ] Add plotting capabilities to visualize option pricing.

## References

- [Options, Futures, and Other Derivatives](https://faculty.ksu.edu.sa/sites/default/files/options_futures_and_other_derivatives_8th_ed_part1.pdf)
- [Option Volatility and Pricing](https://tfal.in/wp-content/uploads/2023/09/Option-Volatility-and-Pricing_-Advanced-Trading-Strategies-and-Techniques-PDFDrive-.pdf)
- [Coding Jesus](https://www.youtube.com/@CodingJesus)
