# FINANCIAL OPTIONS PAYOFF CALCULATOR
#### Video Demo: <https://youtu.be/cUFJpF7QhkE>
#### Description:

This project is a Command Line Interface (CLI) calculator developed in C to model and simulate the payoffs of complex financial option structures at expiration. The main goal is to provide a fast and memory-safe tool that uses dynamic memory allocation to calculate the Profit and Loss (PnL) of options strategies, incorporating the mathematical foundations of the Black-Scholes model.

Developed by Luis Claudio Santos da Cruz, from Araruama, Rio de Janeiro, Brazil. As a mathematics teacher with over 20 years of experience, this project was born from the desire to combine advanced quantitative finance concepts with the strict defensive programming and memory management practices taught in CS50.

## Architecture and Design Decisions

The project was intentionally divided into core logical pillars to ensure the code is modular and easy to maintain:

### 1. Data Structures (Structs)
To avoid passing dozens of loose variables between functions, I created two main structs:
*   `Option`: Stores the fundamental parameters of an option (Type, Strike, Time, Risk-Free Rate, Volatility, and Premium).
*   `Leg`: Represents one "leg" of the operation. It contains an `Option`, the position (Long/Short), and the quantity of lots. This allows the composition of strategies ranging from a simple Call (1 leg) to an Iron Condor (4 legs) or more.

### 2. Mathematical Engine and Payoff Logic
The code implements differential equations for price calculation. Although the main simulator focuses on the intrinsic value of the options at expiration ($T=0$), the original Black-Scholes pricing engine was structured in the base code to allow future expansions that calculate PnL before expiration. The `get_leg_pnl` function consolidates the intrinsic value calculation by subtracting the premium paid/received and multiplying it by the direction of the position.

### 3. Dynamic Memory Management
One of the most crucial architectural decisions was not limiting the number of "legs" in a strategy. Instead of declaring a static array, I used `malloc` to dynamically allocate the exact memory needed based on the user's input. The strict use of `free()` at the end of the execution ensures the calculator has no memory leaks, a fact proven by rigorous testing with Valgrind.

## Challenges and Solutions

Building this calculator in C brought significant technical challenges:

*   **The Input Validation Challenge (What went wrong):** My initial approach used simple `scanf` calls. I quickly realized that if a user typed a letter instead of a number for the asset price, the program would enter an infinite loop and crash.
*   **The Solution (What went right):** I had to implement defensive programming loops and a custom `clear_input_buffer()` function to clean keyboard inputs. This ensures the program gracefully handles edge cases, such as negative prices or non-numeric characters.
*   **The Mathematical Translation:** Translating continuous financial mathematics into discrete C code was a major challenge, especially when dealing with floating-point precision. However, successfully mapping the Black-Scholes logic to C structs and seeing the payoff matrix accurately reflect the plateau of a Bull Call Spread was the biggest success of the project.
