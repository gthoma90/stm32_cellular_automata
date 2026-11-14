#include "stm32f1xx.h"
#include "systick.h"

#define STK_CTRL (*(volatile uint32_t *)0xE000E010UL)
#define STK_LOAD (*(volatile uint32_t *)0xE000E014UL)
#define STK_VAL  (*(volatile uint32_t *)0xE000E018UL)

static volatile uint32_t ticks_ms;

void systick_handler(void) {
    ++ticks_ms;
}

uint32_t systick_millis(void) {
    return ticks_ms;
}

void systick_init(void) {  
    STK_CTRL = 0;

    STK_LOAD = 7999;         // using standard 8MHz clock
    STK_VAL = 0;

    STK_CTRL |= (1U << 2);   // CLKSOURCE
    STK_CTRL |= (1U << 1);   // TICKINT
    STK_CTRL |= (1U << 0);   // ENABLE
}


