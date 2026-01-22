# LPC2148 LED Matrix Cursor Control

Bare-metal embedded application for the **NXP LPC2148 (ARM7TDMI-S)** demonstrating
GPIO, SPI communication, and basic human-machine interaction.

The system controls an **8×8 LED matrix**, reads a **joystick**, and displays
cursor coordinates on a **16×2 LCD**.

---

## Features

- Move a cursor on an 8×8 LED matrix using a joystick
- Display current `(X, Y)` position on a 16×2 LCD
- Center/reset cursor using joystick button
- SPI communication with LED matrix
- Direct GPIO control (no OS, no HAL)
- Designed for educational / laboratory use

---

## Hardware Components

| Component        | Interface | Notes |
|------------------|-----------|-------|
| LPC2148 MCU      | —         | ARM7TDMI-S |
| 8×8 LED Matrix   | SPI (GPIO / bit-banged) | Single LED active at a time |
| 16×2 LCD         | GPIO      | Custom parallel interface |
| Joystick         | GPIO      | Active-low inputs |

---

## How It Works

- **SPI / GPIO**  
  Sends row/column data to the LED matrix using software SPI.

- **LCD Control**  
  Commands and data are sent through GPIO using custom routines.

- **Joystick Handling**  
  Direction inputs are read from GPIO pins and mapped to cursor movement.

- **Cursor Logic**  
  Updates `(x, y)` coordinates and lights the corresponding LED.

---

## Pin Configuration

### Joystick (Active-Low GPIO)

| Direction | Pin   |
|----------|-------|
| UP       | P0.20 |
| DOWN     | P0.17 |
| LEFT     | P0.18 |
| RIGHT    | P0.19 |
| CENTER   | P0.16 |

> ⚠️ Note: GPIO pins are reused for multiple functions. SPI communication is
> implemented via software (bit-banging) and time-multiplexed with input handling.

### LCD

- Connected to **GPIO Port 1**
- Control/data lines handled in `sendCommand()` and `sendData()`

---

## Project Structure

- src/ Application source code
- linker/ Linker script
- keil/ Keil µVision project files


---

## Build & Run

1. Open `keil/lab10.uvproj` in **Keil µVision**
2. Build the project
3. Flash to LPC2148 using ULINK (or compatible programmer)

---

## Notes

This project focuses on **low-level embedded programming** and intentionally avoids
libraries or abstraction layers to demonstrate direct hardware control.

