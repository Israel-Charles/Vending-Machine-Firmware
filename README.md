# Vending Machine Firmware

This project involves the development of a software-based vending machine for sodas using the ESP32-WROOM development board, programmed in C. The vending machine operates under FreeRTOS to manage real-time tasks, handle concurrent operations, and simulate random faults. The machine supports transactions using nickels, dimes, and quarters and dispenses four types of sodas with different prices and quantities. The system features error handling and task management to ensure a realistic and robust vending machine simulation.

#### Demo > https://wokwi.com/projects/406402630070544385

## Project Features

- **Coin Acceptance**: The machine accepts nickels, dimes, and quarters. Each transaction is calculated based on these denominations.
  
- **Soda Dispensing**: Offers four types of sodas (Cola, Grape, Orange, Lime), each with unique costs and quantities.

- **Error Handling**: Implements random fault generation with a 4% chance per second to simulate real-world vending machine issues. Some errors can be resolved without resetting the machine.

- **Task Management**: Utilizes FreeRTOS for task scheduling, prioritization, and concurrency management.

- **LED Indicators**: Uses LED indicators to display system status during setup and operation.

## Assumptions and Implementation Details

1. **Transaction Handling**:
   - Tasks are created to handle coin insertion, soda selection, and dispensation.
   - Supports input for coin denominations ('Q' for Quarters, 'D' for Dimes, 'N' for Nickels).
   - Users can select sodas using IDs (1 for Cola, 2 for Grape, 3 for Orange, 4 for Lime) or cancel the transaction with 'C'.

2. **Soda Inventory and Coin Availability**:
   - Displays menu showing soda quantities, prices, and available coin counts.
   - Easy configuration for initial quantities and prices in the source code.

3. **Task Management**:
   - Input and error handling tasks operate concurrently using both ESP32 cores.
   - Task prioritization ensures critical tasks (e.g., error handling) can interrupt lower-priority tasks.

4. **Error Generation and Handling**:
   - Four types of errors are simulated, with random generation and priority handling.
   - Task suspension and delays manage task execution and access to global variables.
   - Error with ID ‘1’ can be resolved using an activation code ('IziUnlock') without resetting the machine.

## User Manual

1. **Starting Up**:
   - Displays a welcome message and instructions on boot-up.
   - May require pressing the reset button initially.

2. **Making a Purchase**:
   - Enter coins using 'Q', 'D', 'N' and press 'Enter'.
   - Select soda using IDs 1-4 or cancel with 'C'.

3. **Handling Errors**:
   - Error descriptions provided on-screen.
   - Some errors can be resolved with the activation code, while others require a reset.

