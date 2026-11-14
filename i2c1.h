#ifndef I2C1_H
#define I2C1_H

#include <stdint.h>


void i2c1_init(void);

void i2c1_start(uint8_t addr);

void i2c1_stop(void);

void i2c1_write_byte(uint8_t byte);

void i2c1_write(
        uint8_t address,
        const uint8_t *data,
        uint16_t length);

#endif
