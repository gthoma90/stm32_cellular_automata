#ifndef SSD1306_H
#define SSD1306_H

#include <stdint.h>

#define SSD1306_ADDR  0x3C
#define GDDRAM_PG_SRT_ADDR 0xB0
#define CTRL_CMD 0x00
#define CTRL_DATA 0x40

void ssd1306_init(void);
void ssd1306_command(const uint8_t *cmd, uint16_t len);
void ssd1306_all_on(void);
void ssd1306_data(const uint8_t *data, uint16_t len);
// buffer must contain BUFFER_SIZE bytes in display page order.
void ssd1306_update(const uint8_t *buffer);


#endif

