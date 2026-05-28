# Binary-Anoid-Meowser

Addition of an Arduino Nano to the Anoid Meowser to enable binary firing capabilities. 

This project adapts the solenoid-driven Anoid Meowser by introducing a microcontroller to manage the firing cycle. Using a custom state-machine and switch debouncing, this modification allows the blaster to fire exactly one dart when the trigger is pulled, and one dart when the trigger is released.

*Code heavily borrowed and adapted from Will(BEEF)'s Manatee remix of the Gnk-200, modified by drift_Loki/Eugene.*

---

## ⚡ Features

*   **Binary Trigger:** Fires once on trigger pull, and once on trigger release.
*   **Spam Protection:** A built-in queue cap (max 3 darts) prevents "runaway" firing if the trigger is feathered faster than the solenoid can physically cycle.
*   **Adjustable DPS:** Easily tune your Darts Per Second (DPS) in the code to match your hopper or magazine feeding rate.
*   **Cold Start Compensation:** Adds a customizable delay to the very first shot to overcome static friction when the solenoid has been resting.
*   **Non-Blocking State Machine:** Uses `millis()` instead of `delay()`, ensuring the microcontroller is always reading the trigger state even while the solenoid is moving.

---

## 🛠️ Bill of Materials (BOM)

To complete this modification, you will need:
*   **1x** Arduino Nano
*   **1x** Nexperia PSMN1R0-30YLC MOSFET (Highly recommended for high-rate-of-fire solenoid control)
*   **1x** Microswitch (for the trigger)
*   **1x** Solenoid (Standard or Neutron high-rate-of-fire)
*   Standard blaster wiring components (wire, resistors, flyback diode for the solenoid, XT60 connector, etc.)

---

## 🔌 Wiring Diagram

Please refer to the wiring diagram below before soldering. Ensure your flyback diode is oriented correctly across the solenoid terminals to protect the MOSFET from voltage spikes.

<img width="1700" height="1000" alt="meowser diagram" src="https://github.com/user-attachments/assets/c9c2b6d3-35cd-44de-bed7-c116a4837521" />

---

## 💻 Software Installation

1. Download and install the [Arduino IDE](https://www.arduino.cc/en/software).
2. Install the **Debounce** library:
   * Go to `Sketch` -> `Include Library` -> `Manage Libraries...`
   * Search for `debounce` and install it.
3. Open the `Binary_Anoid_Meowser.ino` sketch.
4. (Optional) Adjust the timing variables at the top of the sketch to match your specific solenoid's retraction and extension times:
```cpp
   const int solenoidOn = 40;   // Time it takes solenoid to extend 
   const int solenoidOff = 32;  // Time it takes solenoid to retract
   const int solenoidCold = 15; // Extra time for first shot friction
   int dpsSetting = 9;          // Desired darts per second
