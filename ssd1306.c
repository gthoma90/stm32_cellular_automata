#include "ssd1306.h"
#include "i2c1.h" 
#include "graphics.h"

void ssd1306_command(const uint8_t *cmd, uint16_t len)
{
    i2c1_start(SSD1306_ADDR);
    i2c1_write_byte(CTRL_CMD);
    for (uint16_t i = 0; i < len; ++i) {
        i2c1_write_byte(cmd[i]);
    }
    i2c1_stop();
}

void ssd1306_data(const uint8_t *data, uint16_t len)
{ 
    i2c1_start(SSD1306_ADDR);
    i2c1_write_byte(CTRL_DATA);
    for (uint16_t i = 0; i < len; ++i) {
        i2c1_write_byte(data[i]);
    }
    i2c1_stop();
}

  
void ssd1306_init(void)
{
    const uint8_t init_cmds[] = {
        0xAE,        // Display OFF

        0xD5, 0x80,  // Clock configuration
        0xA8, 0x3F,  // 64-row multiplex ratio
        0xD3, 0x00,  // No display offset
        0x40,        // Start line 0

        0x20, 0x02,  // Page addressing mode

        0xA1,        // Segment remap
        0xC8,        // COM scan direction
        0xDA, 0x12,  // COM hardware configuration

        0x81, 0x7F,  // Contrast
        0xD9, 0xF1,  // Precharge period
        0xDB, 0x20,  // VCOMH deselect level

        0xA4,        // Display RAM contents
        0xA6,        // Normal display
        0x2E,        // Disable scrolling

        0x8D, 0x14,  // Enable charge pump
        0xAF,        // Display ON
        // 0xA5         // Force ALL pixels ON    
    };

    ssd1306_command(init_cmds, sizeof(init_cmds));
}

void ssd1306_all_on(void) {
    const uint8_t all_on[] = {0xA5};
    ssd1306_command(all_on, sizeof(all_on));
}

void ssd1306_update(const uint8_t *buffer) {
    uint8_t page_commands[] = {
        0xB0,        // PAGE 0
        0x00, 0x10   // COLUMN 0
    };
    for(uint8_t page = 0; page < PAGE_COUNT; ++page) {
        page_commands[0] = 0xB0 + page;
        ssd1306_command(page_commands, sizeof(page_commands));
        const uint8_t *bufferPtr = buffer + (page * DISP_WIDTH);
        ssd1306_data(bufferPtr, DISP_WIDTH);
    }
}

