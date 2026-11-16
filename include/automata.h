#ifndef AUTOMATA_H
#define AUTOMATA_H

#include <stdint.h>

#define NUM_COLS 128

// Buffers must be distinct NUM_COLS-byte rows; input cells must be 0 or 1.
void automata_generate_row(const uint8_t *ptr_curr, uint8_t *ptr_new, uint8_t rule_num);




#endif
