#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <stdint.h>
#include <stdbool.h>


void graphics_clear_frame(void);
void graphics_draw_pixel(uint16_t x, uint16_t y, bool on);
void graphics_render_char(uint16_t x, uint16_t y, char c);
void graphics_draw_text(uint16_t x, uint16_t y, const char *text);
// Returns the internal page-ordered framebuffer; no copy is made.
const uint8_t *graphics_get_buffer(void);
// ptr must contain len cells; nonzero cells draw lit pixels.
void graphics_draw_line(uint16_t x, uint16_t y, const uint8_t *ptr, const uint16_t len);
void graphics_scroll_frame(void);


#endif
