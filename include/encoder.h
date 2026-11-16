#ifndef ENCODER_H
#define ENCODER_H

#include <stdbool.h>
#include <stdint.h>

typedef enum {
    BUTTON_NONE,
    BUTTON_SINGLE,
    BUTTON_DOUBLE
} ButtonPress;

typedef enum {
    ENCODER_SLOW,
    ENCODER_FAST
} EncoderSpeed;

void encoder_init(void);
uint16_t encoder_count(void);
void encoder_clear_count(void);
EncoderSpeed encoder_speed(void);
// Poll regularly; click events are returned once, after debounced release.
ButtonPress button_press(void);



#endif
