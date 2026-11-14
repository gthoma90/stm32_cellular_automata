#include "automata.h"



void automata_generate_row(const uint8_t *ptr_curr, uint8_t *ptr_new, uint8_t rule_num) {
    for (uint16_t i = 0; i < NUM_COLS; ++i) {
        if (i == 0 || i == NUM_COLS - 1) {
            ptr_new[i] = 0;
        } else {
            uint8_t left = ptr_curr[i - 1];
            uint8_t center = ptr_curr[i];
            uint8_t right = ptr_curr[i + 1];

            uint8_t neighborhood = (left << 2) | (center << 1) | (right);

            ptr_new[i] = (rule_num >> neighborhood) & 1;
        }
    }
}

