#ifndef ENCODER_H
#define ENCODER_H

#include <stdbool.h>
#include <stdint.h>

void encoder_init(void);
bool button_pressed(void);
void button_reset(void);
uint16_t encoder_count(void);
void encoder_clear_count(void);




#endif
