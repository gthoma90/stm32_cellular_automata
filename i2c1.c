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

    // Force transitions to refresh the I2C input filters
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
    // Clear bits
    GPIOB->CRL &= ~(GPIO_CRL_MODE6 | GPIO_CRL_CNF6 |
                    GPIO_CRL_MODE7 | GPIO_CRL_CNF7);
    // Set bits
    GPIOB->CRL |= (GPIO_CRL_MODE6 | GPIO_CRL_CNF6 |
                   GPIO_CRL_MODE7 | GPIO_CRL_CNF7);

    I2C1->CR1 |= I2C_CR1_SWRST;
    I2C1->CR1 &= ~I2C_CR1_SWRST;

    // I2C peripheral clock: PCLK1 = 8 MHz
    I2C1->CR2 &= ~I2C_CR2_FREQ;
    I2C1->CR2 |= 8;

    // Standard Mode: 100 kHz SCL
    // CCR = 8 MHz / (2 * 100 kHz) = 40
    I2C1->CCR = 40;

    // Maximum rise time = 1000 ns
    // PCLK1 period = 125 ns
    // TRISE = (1000 / 125) + 1 = 9
    I2C1->TRISE = 9;

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
    
    // Poll for SB bit (start bit)
    // NOTE: Register AND Bit-Mask to "READ" 
    while (!(I2C1->SR1 & I2C_SR1_SB)) {}

    // Write Slave address to DR register
    // NOTE: The address is found in the 
    // peripheral's documentation.
    // The 7-bit address is shifted left 1 place
    // to give space for the R/W bit at pos 0.
    // This move leaves the bit as 0 where
    // R/W = 0: Masteir writes, slave reads
    I2C1->DR = (addr << 1); 

    // 4. Poll address bit 
    while (!(I2C1->SR1 & I2C_SR1_ADDR)) {}

    // 5. Read SR2 to clear address bit
    // NOTE: Hardware needs a read of both SR1 
    // and SR2 to clear ADDR bit.
    // The (void) says to evaluate this expression but
    // with no need to return the value.
    // Important that this is done with 'volatile' 
    // variables since it forces the compiler to 
    // execute the read.
    (void)I2C1->SR2;

    uint16_t i = 0;
    while (i < length) {
        // Poll transmit data register empty flag: TxE
        while (!(I2C1->SR1 & I2C_SR1_TXE)) {}

        I2C1->DR = *(data + i);

        ++i;
    }

    // Poll BTF for byte finish
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
    while (!(I2C1->SR1 & I2C_SR1_BTF)) {}
    I2C1->CR1 |= I2C_CR1_STOP;
    // do i need to add this for stop flag?
    while (I2C1->SR1 & I2C_CR1_STOP) {}
     // Temporary timing experiment
    for (volatile uint32_t i = 0; i < 10000; ++i) {}
}

void i2c1_write_byte(uint8_t byte) {
    while (!(I2C1->SR1 & I2C_SR1_TXE)) {}
    I2C1->DR = byte;
}

