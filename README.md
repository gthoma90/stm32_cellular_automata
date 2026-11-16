# STM32 Cellular Automata Display

A small, interactive device for generating and displaying elementary cellular automata on a 128×64 OLED screen, built using bare-metal firmware on an STM32F103 microcontroller.

The project grew out of studying automata theory and an interest in how simple, local rules can produce complex, emergent behavior. What started as a fun way to visualize those rules became primarily an exercise in learning embedded development: configuring peripherals, communicating with hardware, managing a framebuffer, and handling user input. The firmware uses CMSIS register definitions without an STM32 HAL or graphics framework.

## Demo

*TODO*

## Hardware

| Component | Description |
|---|---|
| Microcontroller | STM32F103C8T6 (Blue Pill) |
| Display | SSD1306 128×64 monochrome OLED |
| Input | Rotary encoder with integrated push button |
| Programmer/debugger | ST-Link |
| Communication | I²C1 |

### Pin Assignments

| Pin | Function |
|---|---|
| PB6 | I²C1 SCL |
| PB7 | I²C1 SDA |
| PA0 | TIM2 encoder channel A |
| PA1 | TIM2 encoder channel B |
| PA2 | Encoder push button |

## Operation

On startup, the device presents a home screen where the user can select one of the 256 elementary cellular automaton rules. Turning the rotary encoder changes the selected rule, with faster rotation increasing the step size from 1 to 5.

Pressing the encoder button enters display mode. The screen is cleared, the selected rule appears in the header, and the automaton is generated from a single active cell in the center of the initial row. The user can then start or pause continuous scrolling, or return to the home screen to choose another rule.

| Input | Action |
|---|---|
| Rotate encoder | Select rule (0–255) |
| Rotate quickly | Change rule in steps of 5 |
| Single click on home screen | Display selected automaton |
| Single click in display mode | Start/pause scrolling |
| Double click in display mode | Return to home screen |

## Firmware

The firmware consists of several components implemented for this project:

- **I²C driver:** Register-level configuration and communication using the STM32F103 I²C peripheral and CMSIS definitions. Handles START/STOP conditions, addressing, and byte transmission.
- **SSD1306 driver:** Display initialization, command and data transmission, and framebuffer updates using the OLED's page-addressing mode.
- **Graphics and text:** A 1024-byte framebuffer represents the 128×64 monochrome display. Custom graphics and text-rendering functions operate on this buffer, using character glyph data obtained from an existing open-source font library.
- **User input:** The rotary encoder uses TIM2's hardware encoder interface, with additional logic for detecting rotation speed and applying acceleration. The push button is polled and handled by a debounced state machine supporting single and double clicks.
- **Cellular automata:** Implements all 256 elementary cellular automaton rules. Each successive row is determined by the three-cell neighborhoods in the preceding row, with scrolling allowing the pattern to evolve beyond the physical height of the display.

## Notes

### Scrolling Bottleneck

One of the more interesting problems was improving the display's scrolling performance. Initially, I suspected that I²C transmission time was limiting the update rate. I measured the time required to transfer the framebuffer, then increased the I²C clock speed. However, the visible scrolling speed improved much less than expected. Timing the framebuffer operations separately revealed that rendering was taking substantially longer than transmitting the data.

The initial scrolling implementation relied on repeatedly manipulating individual pixels. I replaced this with byte-level operations using bit shifts to move the framebuffer contents more efficiently. This was a useful lesson in measuring performance before deciding what to optimize: improving communication speed alone wasn't enough when most of the time was being spent preparing the data.

### Framebuffer and Glyph Layout

The SSD1306 organizes display memory into eight pages, each containing 128 bytes. Each byte represents eight vertically adjacent pixels, whereas the glyph data used for text rendering was organized with bits packed horizontally. Implementing the text renderer required translating between these two layouts, providing an interesting exercise in bit manipulation and working with the display's memory representation.

### Hardware Peripherals

Configuring I²C directly required working through the STM32 reference manual, understanding peripheral state and status flags, and getting the communication sequence correct. I also encountered a documented hardware erratum that required a workaround.

The button and encoder presented a different set of challenges, particularly handling mechanical input reliably. The button required debouncing and tracking multiple states to distinguish single and double clicks, while encoder acceleration required translating hardware counts and timing into predictable user-interface behavior.

## Building

The project uses the ARM GNU toolchain and a Makefile, with the following development tools:

- `arm-none-eabi-gcc`
- GNU Make
- OpenOCD
- GDB
- ST-Link programmer/debugger

From the project directory, run:

```bash
make
```

This builds the firmware and generates the ELF executable. The project includes startup code, a linker script, and Makefile configuration adapted from existing reference material.

## Project Status

The core functionality is implemented, including rule selection, automaton generation, display scrolling, and encoder/button controls.

Error detection and recovery have not yet been implemented. Additional refinements, testing, documentation, and enclosure work are ongoing.

## Attribution

The 8×8 bitmap font data comes from [Daniel Hepper's font8x8 library](https://github.com/dhepper/font8x8), based on public-domain VGA fonts. The font data is public domain; the text rendering implementation was written for this project.
