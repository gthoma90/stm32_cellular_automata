#include "ssd1306.h"
#include "i2c1.h"
#include "graphics.h"
#include "automata.h"
#include "encoder.h"
#include "systick.h"


void draw_home_screen(void);
void draw_ca_screen(void);
void draw_scroll_screen(void);
int16_t get_rule_increment(void);
static void rule_to_string(void);
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

static ButtonPress button;
static uint8_t selected_rule;
static uint8_t temp_rule;
static int16_t reset_count;
static char rule_str[4];


static void startup_delay(void)
{
    for (volatile uint32_t i = 0; i < 600000u; ++i) {
        __asm volatile ("nop");
    }
}

int main(void) {

    AppMode mode = MODE_HOME;
    
    bool dirty = true;
    bool scrolling = false;
   
    systick_init();
    encoder_init();
    i2c1_init();
    startup_delay();
    ssd1306_init();
    graphics_clear_frame();
    ssd1306_update(graphics_get_buffer());

    selected_rule = 0;
    reset_count = encoder_count();


    while(1) {

        button = button_press();

        switch (mode) {
            case MODE_HOME:
                switch (button) {
                    case BUTTON_NONE: {
                        int16_t increment = get_rule_increment();
                        if (increment) {
                            selected_rule += increment;
                            dirty = true;
                        }
                        if (dirty) {
                            draw_home_screen();
                            ssd1306_update(graphics_get_buffer());
                            dirty = false;
                        }
                        break;
                                      }
                    case BUTTON_SINGLE:
                        dirty = true;
                        rule_to_string();
                        mode = MODE_CA;
                        break;
                    case BUTTON_DOUBLE:
                        dirty = true;
                        rule_to_string();
                        mode = MODE_CA;
                        break;
                }
                break;

            case MODE_CA:
                switch (button) {
                    case BUTTON_NONE:
                        if (dirty) {
                            draw_ca_screen();
                            ssd1306_update(graphics_get_buffer());
                            dirty = false;
                        }
                        if (scrolling) {

                            uint32_t start = systick_millis();

                            draw_scroll_screen();
                            ssd1306_update(graphics_get_buffer());

                            volatile uint32_t scroll_frame_ms = systick_millis() - start;

                            draw_scroll_screen();
                            ssd1306_update(graphics_get_buffer());
                        }
                        break;
                    case BUTTON_SINGLE:
                        if (!scrolling) {
                            scrolling = true;
                        }
                        else {
                            scrolling = false;
                        }
                        break;
                    case BUTTON_DOUBLE:
                        dirty = true;
                        reset_count = encoder_count();
                        graphics_clear_frame();
                        mode = MODE_HOME;
                        break;
                }
                break;
        }
        
    }
}


void draw_home_screen(void) {
    rule_to_string();
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

void draw_scroll_screen(void) {
    graphics_scroll_frame();
    advance_automaton();
    graphics_draw_line(0, 63, curr, DISP_WIDTH);
}

static void advance_automaton(void)
{
    automata_generate_row(curr, next, selected_rule); 

    uint8_t *temp = curr;
    curr = next;
    next = temp;
}

int16_t get_rule_increment(void) {
    int16_t count = encoder_count();
    EncoderSpeed speed = encoder_speed();
    int16_t delta = count - reset_count;
    int16_t steps = delta / 4;
    reset_count += steps * 4;
    switch (speed) {
        case ENCODER_SLOW:
            return steps;
        case ENCODER_FAST:
            return (5 * steps);
        default:
            return steps;
    } 
}

static void rule_to_string(void) {
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


