#include "stm32f1xx.h"
#include "encoder.h"
#include "systick.h"

#include <stdbool.h>


static bool prev_state = 1;
static bool stable_state = 1;
static uint32_t tick_one = 0;
static uint32_t tick_two = 0;

static uint32_t recent_tick = 0;
static uint16_t recent_count = 0;
static bool turning = false;
static bool turning_fast = false;

typedef enum {
    IDLE,
    ONE_WAITING,
    ONE_DEBOUNCED,
    TWO_WAITING,
} ButtonState;

static ButtonState state = IDLE;


void encoder_init(void) {

    // clock enables
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;

    // TIM2 enable clear
    TIM2->CR1 &= ~TIM_CR1_CEN;

    // pin config
    GPIOA->CRL &= ~GPIO_CRL_MODE0;    
    GPIOA->CRL &= ~GPIO_CRL_CNF0;
    GPIOA->CRL |= (1u << 3); // CNF 10, MODE 00
                             
    GPIOA->CRL &= ~GPIO_CRL_MODE1;    
    GPIOA->CRL &= ~GPIO_CRL_CNF1;
    GPIOA->CRL |= (1u << 7); // CNF 10, MODE 00

    // push button pin 
    GPIOA->CRL &= ~GPIO_CRL_MODE2;
    GPIOA->CRL &= ~GPIO_CRL_CNF2;
    GPIOA->CRL |= (1U << 11); 

    // pins set to pull-up
    GPIOA->BSRR = GPIO_BSRR_BS0 | GPIO_BSRR_BS1 | GPIO_BSRR_BS2;

    // CC input to TI
    TIM2->CCMR1 &= ~(TIM_CCMR1_CC1S);
    TIM2->CCMR1 |= (0b01u << TIM_CCMR1_CC1S_Pos);
    TIM2->CCMR1 &= ~(TIM_CCMR1_CC2S);
    TIM2->CCMR1 |= (0b01u << TIM_CCMR1_CC2S_Pos);

    // CC non-inverting
    TIM2->CCER &= ~(TIM_CCER_CC1P);
    TIM2->CCMR1 &= ~(TIM_CCMR1_IC1F);
    TIM2->CCER &= ~(TIM_CCER_CC2P);
    TIM2->CCMR1 &= ~(TIM_CCMR1_IC2F);

    // Both inputs active on rise and fall
    TIM2->SMCR &= ~(TIM_SMCR_SMS);
    TIM2->SMCR |= (0b011u << TIM_SMCR_SMS_Pos);

    // pre-scaler
    TIM2->PSC = 0;

    // clear count
    TIM2->CNT = 0;

    // count limit
    TIM2->ARR = 0xFFFF;

    // CEN Counter Enable
    TIM2->CR1 |= TIM_CR1_CEN;
}

uint16_t encoder_count(void) {
    return TIM2->CNT;
}


void encoder_clear_count(void) {
    TIM2->CNT = 0; 
}

ButtonPress button_press(void) {
    bool press = false;
    bool release = false;
    uint32_t tick_call = systick_millis();
    bool raw_state = (GPIOA->IDR & GPIO_IDR_IDR2);

    if (raw_state != prev_state) {
        tick_one = tick_call;
        prev_state = raw_state;
    }

    if ((tick_call - tick_one > 10) && (raw_state != stable_state)) {
        if (raw_state == 1) {
            release = true;
            stable_state = 1;
        }
        else {
            press = true;
            stable_state = 0;
        }
    }

    switch (state) {
        case IDLE:
            if (press) {
                state = ONE_WAITING;
            }
            break;

        case ONE_WAITING:
            if (release) {
                tick_two = tick_call;
                state = ONE_DEBOUNCED;
            }
            break;

        case ONE_DEBOUNCED:
            if (tick_call - tick_two > 300) {
                state = IDLE;
                return BUTTON_SINGLE;
            } 
            if (press) {
                state = TWO_WAITING; 
            }
            break;

        case TWO_WAITING:
            if (release) {
                state = IDLE;
                return BUTTON_DOUBLE;
            }
            break;

        default:
            state = IDLE;
            break;           
    }
    return BUTTON_NONE;
}

EncoderSpeed encoder_speed(void) {
    uint32_t raw_encoder_tick = systick_millis();
    uint16_t raw_encoder_count = encoder_count();
    if (raw_encoder_count != recent_count) {
        if (raw_encoder_tick - recent_tick < 100) {
            return ENCODER_FAST;
        }
        recent_count = raw_encoder_count;
        recent_tick = raw_encoder_tick;
    }
    return ENCODER_SLOW;
}
