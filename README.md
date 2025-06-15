# C++ Options Pricing Engine

A C++ model for pricing Options using the Black-Scholes and Binomial Tree methods. This project provides a simple implementation of options pricing models, allowing users to create and price different types of options.

## Features

- An Option class used to model different types of options with parameters including type, exercise style and carry rate(dividend yield).
- A Black-Scholes model for pricing European options.
- A Binomial Tree model for pricing both European and American options.

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

## TODO

- [ ] Add a GUI for easier interaction.
- [ ] Add plotting capabilities to visualize option pricing.
- [ ] Add new pricing models such as Monte Carlo simulation.
- [ ] Add tests for the pricing models.
