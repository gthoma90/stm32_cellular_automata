#include "automata.h"



void automata_generate_row(const uint8_t *ptr_curr, uint8_t *ptr_new, uint8_t rule_num) {
    uint8_t left = 0;
    uint8_t center = 0;
    uint8_t right = 0;
    for (uint16_t i = 0; i < NUM_COLS; ++i) {
        // Reflect at each edge: the missing neighbor mirrors the inward neighbor.
        if (i == 0) {
            left = ptr_curr[i + 1];
            center = ptr_curr[i];
            right = ptr_curr[i + 1];
        } 
        else if (i == NUM_COLS - 1) {
            left = ptr_curr[i - 1];
            center = ptr_curr[i];
            right = ptr_curr[i - 1];
        } 
        else {
            left = ptr_curr[i - 1];
            center = ptr_curr[i];
            right = ptr_curr[i + 1];
        }

        // Encode left/center/right as 0..7 to select the corresponding rule bit.
        uint8_t neighborhood = (left << 2) | (center << 1) | (right);

        ptr_new[i] = (rule_num >> neighborhood) & 1;
    }
}

