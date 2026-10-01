# Monopoly: Sri Lankan Edition (C Simulation)

Welcome to the **Monopoly: Sri Lankan Edition**, a highly detailed, text-based simulation of the classic board game, built entirely in C.

Unlike a standard Monopoly game, this project features a dynamic, breathing world with advanced financial mechanics (like loans, insurance, and property depreciation), regional and economic events, and autonomous AI players with distinct playstyles.

## Key Features

* **Sri Lankan Game Board:** Buy, trade, and build on iconic locations ranging from Pettah and Maradana to Galle Fort, Trincomalee, and Galle Face.

* **Autonomous AI Players:** The game simulates matches between four distinct AI strategies:

  * *Aggressive Investor:* Takes big risks for maximum future rent.

  * *Conservative Banker:* Hoards cash, buys comprehensive insurance, and avoids debt.

  * *Risk Taker:* Leverages heavy loans and bids aggressively in auctions.

  * *Opportunistic Trader:* Adapts to market booms and depreciation discounts.

* **Advanced Financial Mechanics:**

  * **Banking & Loans:** Players can take out secured loans against unmortgaged properties (with dynamic interest rates and foreclosure risks).

  * **Property Depreciation & Maintenance:** Properties age, depreciate in value, and require periodic maintenance to prevent structural damage.

  * **Insurance:** Players can protect their assets with Basic, Comprehensive, or Business insurance policies.

* **Dynamic World Events:**

  * *Inflation:* Periodic inflation alters property prices, rents, and taxes.

  * *National Event Cards:* Affects players with grants, disasters, tourism hypes, or strikes.

  * *Economic & Regional Events:* Shifts the market, such as an "IT Industry Growth" in Nugegoda/Maharagama or a "Tourism Boom" in the South, altering property values and rents.

  * *Government Regulations:* Introduces luxury taxes, housing subsidies, or interest rate cuts.

## Project Structure

The project is modularized into several C files, all tied together by a central header.

* `main.c` - The entry point of the application. It seeds the random number generator and starts the game loop.

* `types.h` - Contains all game constants, configuration values (like tax rates, property prices, event durations), and data structures (`Player`, `BoardSpace`, Enums).

* `game.c` - Manages the core game loop, turn execution, periodic events triggers, and end-of-game summary/winner determination.

* `board.c` - Initializes the 40-square game board with Sri Lankan properties, base prices, rents, and handles player movement calculations.

* `players.c` - Defines AI logic, bidding behavior for auctions, purchasing decisions, and property development/maintenance strategies.

* `finance.c` - Handles the heavy lifting of the economy: rent calculation (factoring in conditions, events, and multipliers), taking/repaying loans, asset taxation, insurance purchases, and bankruptcies.

* `events.c` - Manages the chaotic world elements including the National Event deck, inflation cycles, regional market booms/busts, and government regulations.

## How to Build and Run

This project uses a "unity build" setup where `main.c` directly includes the other source files using the `MONOPOLY_IMPLEMENTATION` macro.

**Prerequisites:** You need a C compiler (like GCC or Clang) installed on your system.

1. Open your terminal or command prompt.

2. Navigate to the directory containing the source files.

3. Compile the game using GCC:

   ```
   gcc main.c -o monopoly_lk
   ```

4. Run the executable:

   * **Linux/macOS:**
     ```
     ./monopoly_lk
     ```

   * **Windows:**
     ```
     monopoly_lk.exe
     ```

## Gameplay

The game runs automatically as a simulation for up to 500 rounds (or until only one solvent player remains). Sit back and read the console output to watch the AI players negotiate a volatile real estate market, suffer through natural disasters, and fight for monopolies! Every 10 rounds, a summary is printed showing player net worth, properties owned, cash, and current market conditions.
