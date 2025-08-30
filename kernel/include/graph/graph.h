#pragma once

// Määritellään näytön leveys, korkeus ja värisyvyys
#define GRAPH_SCREEN_WIDTH      800
#define GRAPH_SCREEN_HEIGHT     480
#define GRAPH_SCREEN_DEPTH      32
#define GRAPH_SCREEN_SIZE       (GRAPH_SCREEN_WIDTH * GRAPH_SCREEN_HEIGHT)

typedef struct graph_image
{
    uint32_t width;
    uint32_t height;
    uint32_t *data;
} graph_image_t;

void graph_enable_exposure_mode();
void graph_set_value_exposure_mode(uint8_t value);
void graph_disable_exposure_mode();
void graph_init();
void graph_clear_screen();
void graph_put_pixel(uint32_t x, uint32_t y, uint32_t color);
void graph_put_array(graph_image_t * img, uint32_t x, uint32_t y);
void graph_copy_array(graph_image_t * img, uint32_t x, uint32_t y);
void graph_put_text(const char *text, uint32_t x, uint32_t y);
void graph_flip();

