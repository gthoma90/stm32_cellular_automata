#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <stdint.h>
#include <stdbool.h>

#define DISP_WIDTH  128
#define DISP_HEIGHT  64

#define PAGE_COUNT  ((DISP_HEIGHT + 7) / 8)  // +7 for partial page dimensions
#define BUFFER_SIZE  PAGE_COUNT * DISP_WIDTH


void graphics_clear_frame(void);
void graphics_draw_pixel(uint16_t x, uint16_t y, bool on);
void graphics_render_char(uint16_t x, uint16_t y, char c);
void graphics_draw_text(uint16_t x, uint16_t y, const char *text);
const uint8_t *graphics_get_buffer(void);
void graphics_draw_line(uint16_t x, uint16_t y, const uint8_t *ptr, const uint16_t len);





#endif
