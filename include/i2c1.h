#ifndef I2C1_H
#define I2C1_H

#include <stdint.h>


// Blocking, single-master writes using 7-bit addresses.
// Polling has no timeout or error recovery; an unresponsive slave can stall the caller.
void i2c1_init(void);

void i2c1_start(uint8_t addr);

// Call after transmitting at least one byte with i2c1_write_byte().
void i2c1_stop(void);

void i2c1_write_byte(uint8_t byte);

// data must contain length bytes; length must be nonzero.
void i2c1_write(
        uint8_t address,
        const uint8_t *data,
        uint16_t length);

#endif
