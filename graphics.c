#include "graphics.h"
#include "font8x8_basic.h"


static uint8_t framebuffer[BUFFER_SIZE];



void graphics_clear_frame(void) {
    for(uint16_t i = 0; i < BUFFER_SIZE; ++i) {
        framebuffer[i] = 0;
    }
}

static uint16_t get_index(uint16_t x, uint16_t y) {
    return (y / 8) * DISP_WIDTH + x;  // For Page style indexing
}

static uint8_t get_mask(uint16_t y) {
    return (uint8_t) (1u << (y % 8));
} 

void graphics_draw_pixel(uint16_t x, uint16_t y, bool on) {
    if (x >= DISP_WIDTH || y >= DISP_HEIGHT) {
        return;
    }
    uint16_t index = get_index(x, y);
    uint8_t mask = get_mask(y);
    if (on) {
        framebuffer[index] |= mask;
    }
    else {
        framebuffer[index] &= ~mask;
    }
}

void graphics_render_char(uint16_t x, uint16_t y, char c) {
    uint8_t row = (uint8_t)c;

    if (row >= 128)
        return;

    for (uint8_t i = 0; i < 8; ++i) {
        for (uint8_t j = 0; j < 8; ++j) {
            uint8_t mask = 1u << i;
            bool on = (font8x8_basic[row][j] & mask) != 0;

            graphics_draw_pixel(x + i, y + j, on);
        }
    }
}

void graphics_draw_text(uint16_t x, uint16_t y, const char *text) {
    while (*text != '\0') {   
        if (x + 8 > DISP_WIDTH || y + 8 > DISP_HEIGHT) 
            return;

        graphics_render_char(x, y, *text);

        ++text;
        x += 8;
    }
}

const uint8_t *graphics_get_buffer(void) 
{
    return framebuffer;
}

void graphics_draw_line(uint16_t x, uint16_t y, const uint8_t *ptr, const uint16_t len) { 
    for (uint16_t i = 0; i < len; ++i) {
        bool on = false;
        if (*(ptr + i) != 0) 
            on = true;
        graphics_draw_pixel(x + i, y, on);
    }
}

void graphics_draw_scroll_line(uint16_t x, uint16_t y, const uint16_t len) {
    for (uint16_t i = 0; i < len; ++i) {
        uint16_t index = get_index(x + i, y + 1);
        uint8_t mask = get_mask(y + 1);
        bool on = ((framebuffer[index] & mask) != 0);
        graphics_draw_pixel(x + i, y, on);
    }
}

void graphics_scroll_frame(void) {
    for (uint16_t i = 2; i < PAGE_COUNT; ++i) {
        for (uint16_t j = 0; j < DISP_WIDTH; ++j) {
            uint16_t index = (i * DISP_WIDTH + j);
            if (i == 2) {
                framebuffer[index] = framebuffer[index] >> 1;
            }
            else {
                uint8_t page_shift = ((framebuffer[index] & 1) << 7);
                framebuffer[index - DISP_WIDTH] |= page_shift;
                framebuffer[index] = framebuffer[index] >> 1;
            }
        } 
    }
}
