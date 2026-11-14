#include "ssd1306.h"
#include "i2c1.h"
#include "graphics.h"
#include "automata.h"
#include "encoder.h"


void draw_home_screen(void);
void draw_ca_screen(void);
uint8_t get_rule(uint16_t);
void rule_to_string(void);
static void advance_automaton(void);
void reset_seed_row(void);

typedef enum {
    MODE_HOME,
    MODE_CA
} AppMode;


static uint8_t row_a[NUM_COLS];
static uint8_t row_b[NUM_COLS];
static uint8_t *curr = row_a;
static uint8_t *next = row_b;

static uint8_t selected_rule;
static uint8_t temp_rule;
static char rule_str[4];


static void startup_delay(void)
{
    for (volatile uint32_t i = 0; i < 600000u; ++i) {
        __asm volatile ("nop");
    }
}

int main(void) {

    AppMode mode = MODE_HOME;
    bool pressed = false;
    bool was_pressed = false;
    bool dirty = true;
    
    encoder_init();
    i2c1_init();
    startup_delay();
    ssd1306_init();
    graphics_clear_frame();
    ssd1306_update(graphics_get_buffer());

    while(1) {

        pressed = button_pressed();
        if (pressed && !was_pressed) {
            graphics_clear_frame();
            dirty = true;
            if (mode == MODE_HOME) {
                mode = MODE_CA;
            }
            else {
                encoder_clear_count(); 
                mode = MODE_HOME;
            }
        }
        was_pressed = pressed;

        switch (mode) {
            case MODE_HOME:
                temp_rule = get_rule(encoder_count());
                if (temp_rule != selected_rule) {
                    selected_rule = temp_rule;
                    dirty = true;
                }
                if (dirty) {
                    rule_to_string();
                    draw_home_screen();
                    ssd1306_update(graphics_get_buffer());
                    dirty = false;
                }
                break;

            case MODE_CA:
                if (dirty) {
                    rule_to_string();
                    draw_ca_screen();
                    ssd1306_update(graphics_get_buffer());
                    dirty = false;
                }
                break;
        }
        
    }
}


void draw_home_screen(void) {
    graphics_draw_text(32, 16, "CELLULAR");
    graphics_draw_text(32, 24, "AUTOMATA");
    graphics_draw_text(24, 40, "RULE : ");
    graphics_draw_text(80, 40, rule_str);
}

void draw_ca_screen(void) {
    graphics_draw_text(0, 0, "RULE:");
    graphics_draw_text(0, 8, rule_str);
    reset_seed_row();
    graphics_draw_line(0, 16, curr, DISP_WIDTH);
    for (uint16_t y = 17; y < DISP_HEIGHT; ++y) {
        advance_automaton();
        graphics_draw_line(0, y, curr, DISP_WIDTH);
    }
}

static void advance_automaton(void)
{
    automata_generate_row(curr, next, selected_rule); 

    uint8_t *temp = curr;
    curr = next;
    next = temp;
}

uint8_t get_rule(uint16_t count) {
    return (uint8_t)(count / 4); 
}

void rule_to_string(void) {
    rule_str[0] = '0' + selected_rule / 100;
    rule_str[1] = '0' + (selected_rule / 10) % 10;
    rule_str[2] = '0' + selected_rule % 10;
    rule_str[3] = '\0';
}

void reset_seed_row(void) {
    for (uint8_t i = 0; i < DISP_WIDTH; ++i) {
        curr[i] = 0;
    }
    curr[NUM_COLS / 2] = 1;
}


