#include "stm32f1xx.h"
#include "i2c1.h"


void i2c1_init(void) {
    // Enable clocks for GPIOB, and I2C1
    RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;
    RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;

    // Release both lines and disable I2C
    I2C1->CR1 &= ~I2C_CR1_PE;
    GPIOB->BSRR = (1u << 6) | (1u << 7);

    // Temporarily configure PB6/PB7 as GPIO open-drain
    GPIOB->CRL &= ~(GPIO_CRL_MODE6 | GPIO_CRL_CNF6 |
                    GPIO_CRL_MODE7 | GPIO_CRL_CNF7);

    GPIOB->CRL |= GPIO_CRL_MODE6 | GPIO_CRL_CNF6_0 |
                  GPIO_CRL_MODE7 | GPIO_CRL_CNF7_0;

    // Work around the STM32F1 I2C analog-filter startup issue with line transitions.
    // IDR reads do not verify levels; released lines require external pull-ups.
    (void)GPIOB->IDR;

    GPIOB->BRR = (1u << 7);   // SDA low
    (void)GPIOB->IDR;

    GPIOB->BRR = (1u << 6);   // SCL low
    (void)GPIOB->IDR;

    GPIOB->BSRR = (1u << 6);  // SCL high
    (void)GPIOB->IDR;

    GPIOB->BSRR = (1u << 7);  // SDA high
    (void)GPIOB->IDR;

    // Configure PB6 (SCL) and PB7 (SDA)
    // Alternate-function open-drain, 50 MHz GPIO output mode
    GPIOB->CRL &= ~(GPIO_CRL_MODE6 | GPIO_CRL_CNF6 |
                    GPIO_CRL_MODE7 | GPIO_CRL_CNF7);
    GPIOB->CRL |= (GPIO_CRL_MODE6 | GPIO_CRL_CNF6 |
                   GPIO_CRL_MODE7 | GPIO_CRL_CNF7);

    // Reset the peripheral after refreshing the line filters.
    I2C1->CR1 |= I2C_CR1_SWRST;
    I2C1->CR1 &= ~I2C_CR1_SWRST;

    // Assumes reset-default clocks: HSI, HCLK and PCLK1 are all 8 MHz.
    I2C1->CR2 &= ~I2C_CR2_FREQ;
    I2C1->CR2 |= 8;

    // Fast Mode: approximately 381 kHz SCL
    // CCR = 8 MHz / (3 * 400 kHz) = 6.67
    // Round up to 7 to stay below 400 kHz
    // FS = 1, DUTY = 0 (2:1)
    I2C1->CCR = I2C_CCR_FS | 7;

    // Maximum rise time = 300 ns
    // PCLK1 period = 125 ns
    // TRISE = floor(300 / 125 + 1) = 3
    I2C1->TRISE = 3;

    // Enable I2C1 peripheral
    I2C1->CR1 |= I2C_CR1_PE;
}

void i2c1_write(
        uint8_t addr,
        const uint8_t *data,
        uint16_t length)
{
    // Set start condition
    I2C1->CR1 |= I2C_CR1_START;
    
    // Reading SB followed by writing DR completes the start-event sequence.
    while (!(I2C1->SR1 & I2C_SR1_SB)) {}

    // Shift the 7-bit address into place; bit 0 stays clear for a write.
    I2C1->DR = (addr << 1); 

    // ADDR indicates that the slave acknowledged its address.
    while (!(I2C1->SR1 & I2C_SR1_ADDR)) {}

    // The SR1 poll followed by this volatile SR2 read clears ADDR.
    (void)I2C1->SR2;

    uint16_t i = 0;
    while (i < length) {
        // Poll transmit data register empty flag: TxE
        while (!(I2C1->SR1 & I2C_SR1_TXE)) {}

        I2C1->DR = *(data + i);

        ++i;
    }

    // TXE alone does not mean the final byte has finished on the bus.
    while (!(I2C1->SR1 & I2C_SR1_BTF)) {}

    // Generate stop
    I2C1->CR1 |= I2C_CR1_STOP;

}

void i2c1_start(uint8_t addr) {
    I2C1->CR1 |= I2C_CR1_START;
    while (!(I2C1->SR1 & I2C_SR1_SB)) {}
    I2C1->DR = (addr << 1); 
    while (!(I2C1->SR1 & I2C_SR1_ADDR)) {}
    (void)I2C1->SR2;
}

void i2c1_stop(void) {
    // Finish the last byte before STOP; hardware clears STOP after it is sent.
    while (!(I2C1->SR1 & I2C_SR1_BTF)) {}
    I2C1->CR1 |= I2C_CR1_STOP;
    while (I2C1->CR1 & I2C_CR1_STOP) {}
    // Retained timing delay; its duration depends on compiler output and clock speed.
    for (volatile uint32_t i = 0; i < 5; ++i) {}
}

void i2c1_write_byte(uint8_t byte) {
    while (!(I2C1->SR1 & I2C_SR1_TXE)) {}
    I2C1->DR = byte;
}

