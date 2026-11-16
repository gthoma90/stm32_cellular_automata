// Minimal Cortex-M3 startup; section boundaries are supplied by stm32f103.ld.
#include <stdint.h>

// These linker symbols represent addresses, not stored C variables.
extern uint32_t _sdata, _edata, _sbss, _ebss;
extern uint32_t _sidata;

int main(void);
void systick_handler(void);

void reset_handler(void) {
    // Restore initialized globals from their flash load image before calling main.
    uint32_t *src = &_sidata;
    uint32_t *dst = &_sdata;
    while (dst < &_edata) {
        *dst++ = *src++;
    }

    // Zero out .bss
    dst = &_sbss;
    while (dst < &_ebss) {
        *dst++ = 0;
    }

    main();

    // There is no runtime to return to if main exits.
    while (1) {}
}

void default_handler(void) {
    while(1) {}
}

// Hardware reads the initial stack pointer and reset address from the start of flash.
// The stack top matches the 10 KiB RAM region; only vectors through TIM2 are listed.
uint32_t *vector_table[] __attribute__((section(".isr_vector"))) = {
    (uint32_t *) 0x20002800,      // stack pointer
    (uint32_t *) reset_handler,   // reset
    (uint32_t *) default_handler, // NMI
    (uint32_t *) default_handler, // hard fault
    (uint32_t *) default_handler, // memory fault
    (uint32_t *) default_handler, // bus fault
    (uint32_t *) default_handler, // usage fault
    (uint32_t *) default_handler, // reserved
    (uint32_t *) default_handler, // reserved
    (uint32_t *) default_handler, // reserved
    (uint32_t *) default_handler, // reserved
    (uint32_t *) default_handler, // SVCall
    (uint32_t *) default_handler, // debug
    (uint32_t *) default_handler, // reserved
    (uint32_t *) default_handler, // PendSV
    (uint32_t *) systick_handler, // SysTick
    (uint32_t *) default_handler, // IRQ0  - WWDG
    (uint32_t *) default_handler, // IRQ1  - PVD
    (uint32_t *) default_handler, // IRQ2  - TAMPER
    (uint32_t *) default_handler, // IRQ3  - RTC
    (uint32_t *) default_handler, // IRQ4  - FLASH
    (uint32_t *) default_handler, // IRQ5  - RCC
    (uint32_t *) default_handler, // IRQ6  - EXTI0
    (uint32_t *) default_handler, // IRQ7  - EXTI1
    (uint32_t *) default_handler, // IRQ8  - EXTI2
    (uint32_t *) default_handler, // IRQ9  - EXTI3
    (uint32_t *) default_handler, // IRQ10 - EXTI4
    (uint32_t *) default_handler, // IRQ11 - DMA1 CH1
    (uint32_t *) default_handler, // IRQ12 - DMA1 CH2
    (uint32_t *) default_handler, // IRQ13 - DMA1 CH3
    (uint32_t *) default_handler, // IRQ14 - DMA1 CH4
    (uint32_t *) default_handler, // IRQ15 - DMA1 CH5
    (uint32_t *) default_handler, // IRQ16 - DMA1 CH6
    (uint32_t *) default_handler, // IRQ17 - DMA1 CH7
    (uint32_t *) default_handler, // IRQ18 - ADC1_2
    (uint32_t *) default_handler, // IRQ19 - CAN1_TX
    (uint32_t *) default_handler, // IRQ20 - CAN1_RX0
    (uint32_t *) default_handler, // IRQ21 - CAN1_RX1
    (uint32_t *) default_handler, // IRQ22 - CAN1_SCE
    (uint32_t *) default_handler, // IRQ23 - EXTI9_5
    (uint32_t *) default_handler, // IRQ24 - TIM1_BRK
    (uint32_t *) default_handler, // IRQ25 - TIM1_UP
    (uint32_t *) default_handler, // IRQ26 - TIM1_TRG_COM
    (uint32_t *) default_handler, // IRQ27 - TIM1_CC
    (uint32_t *) default_handler, // IRQ28 - TIM2
};
