# lpc2148-led-matrix-draw
## LPC2148 LED Matrix Drawing Application

This project implements a simple drawing application using an **8x8 LED matrix**, **joystick**, and **16x2 LCD display**, all controlled by an **LPC2148 ARM7 microcontroller**.

- **LED Matrix (8x8)** via **SPI**
- **Joystick** via **GPIO**
- **LCD 16x2** via **parallel GPIO (custom interface)**
- **Draw mode**: Press the joystick button to center/reset the cursor (or adapt to toggle pixel)

---

## Features

- Move a cursor around the 8x8 LED matrix using the joystick.
- View the current `(X, Y)` position on a 16x2 LCD.
- Center/reset cursor by pressing the joystick button.
- Code includes SPI initialization, LCD control, and GPIO configuration.
- Designed for educational or embedded systems experimentation.

---

## Components Used

| Component        | Interface | Description                      |
|------------------|-----------|----------------------------------|
| LPC2148 MCU      | -         | ARM7TDMI-S Microcontroller       |
| 8x8 LED Matrix   | SPI       | Controlled via `spi_send()`      |
| 16x2 LCD         | GPIO      | Uses custom control/data pins    |
| Joystick         | GPIO      | 5 directions (Up, Down, Left, Right, Center) |

---

## How It Works

- **SPI Initialization**: Communicates with LED matrix.
- **LCD Initialization**: Uses GPIO pins to send commands/data.
- **Joystick Handling**: Reads direction inputs from GPIO (active low).
- **Cursor Movement**: Updates `x` and `y` based on joystick input.
- **Drawing**: Calls `light_dot(x, y)` to light a single LED.

---

## Pin Configuration

### Joystick (Active Low - Input GPIO)

| Direction | Pin        |
|-----------|------------|
| UP        | P0.20      |
| DOWN      | P0.17      |
| LEFT      | P0.18      |
| RIGHT     | P0.19      |
| CENTER    | P0.16      |

### SPI - LED Matrix

| Signal     | Pin        |
|------------|------------|
| CS         | P0.15      |
| MOSI       | P0.18      |
| SCK        | P0.17      |

### LCD - GPIO Mapping

LCD pins are connected to GPIO Port 1 (P1.xx). Details are abstracted in `sendCommand()` and `sendData()`.
