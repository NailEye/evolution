#include <fb/fb.h>
#include <dma/dma.h>
#include <common/common.h>
#include <common/delay.h>
#include <graph/graph.h>
#include "graph-font-8x8.h"

typedef struct graph_control_block
{
    fb_screen_resolution_t phy_res;
    fb_screen_resolution_t vir_res;
    uint8_t page;
} graph_control_block_t;

typedef struct graph_blend_modes
{
    uint8_t exposure_mode_enable;
    uint8_t exposure_mode_value;
} graph_blend_modes_t;

// Määritellään näyttöpuskuri
volatile uint32_t screen_buffer[GRAPH_SCREEN_SIZE];

// Määritellään tyhjä näyttöpuskuri
volatile uint32_t empty_buffer[GRAPH_SCREEN_SIZE];

// Valoisuus tila päällä/pois
graph_blend_modes_t blend_modes = {0};

// Määritellään grapin control blocki
graph_control_block_t grap_cb = {
    {
        GRAPH_SCREEN_WIDTH, 
        GRAPH_SCREEN_HEIGHT
    },
    {
        GRAPH_SCREEN_WIDTH,
        (GRAPH_SCREEN_HEIGHT * 2)
    },
    0
};

uint32_t get_current_mem_addr()
{
    // Palautetaan nykinen muistiosoite sivutuksen mukaan
    return ((uint32_t)fb_get_info()->mem_addr + (grap_cb.page * (GRAPH_SCREEN_SIZE * (GRAPH_SCREEN_DEPTH / 8))));
}

void graph_enable_exposure_mode()
{
    // Asetetaan exposure mode päälle
    blend_modes.exposure_mode_enable = 1;
}

void graph_set_value_exposure_mode(uint8_t value)
{
    // Asetetaan exposure moden arvo
    blend_modes.exposure_mode_value = value;
}

void graph_disable_exposure_mode()
{
    // Asetetaan exposure mode pois päältä
    blend_modes.exposure_mode_enable = 0;
}

void graph_init() 
{
    // Alustetaan framebuffer
    fb_init(&grap_cb.phy_res, &grap_cb.vir_res, GRAPH_SCREEN_DEPTH);
    // Alustetaan DMA0
    dma_enable(0x01);
}

void graph_clear_screen()
{
    // Suoritetaan näyttöpuskurin tyhjennys
    //dma0_clear((uint32_t)&screen_buffer, (GRAPH_SCREEN_WIDTH*GRAPH_SCREEN_HEIGHT) * ((GRAPH_SCREEN_DEPTH / 8)));
    dma0_copy((uint32_t)&empty_buffer, (uint32_t)&screen_buffer, (GRAPH_SCREEN_WIDTH*GRAPH_SCREEN_HEIGHT) * ((GRAPH_SCREEN_DEPTH / 8)));
}

void graph_put_pixel(uint32_t x, uint32_t y, uint32_t color)
{
    // Pikselin piirto näyttöpuskuriin
    *(volatile uint32_t *)((uint32_t)&screen_buffer + ((x + (GRAPH_SCREEN_WIDTH * y)) * (GRAPH_SCREEN_DEPTH / 8))) = color;
}

void graph_put_array(graph_image_t * img, uint32_t x, uint32_t y)
{
    // Loopataan korkeus
    for(uint32_t yh = 0; yh < img->height; yh++)
    {
        // Loopataan leveys
        for(uint32_t xw = 0; xw < img->width; xw++)
        {
            // Luetaan pikselin alpha kanava
            uint32_t pixel_alpha = ((img->data[yh * img->width + xw] >> 24) & 0xFF);
            // Luetaan pikselin väri kanavat
            uint32_t pixel_color = (img->data[yh * img->width + xw] & 0xFFFFFF);

            // Jos alpha on yli tai yhtäsuuri 128
            if(pixel_alpha >= 0xFF)
            {
                // Jos exposure mode on päällä
                if(blend_modes.exposure_mode_enable)
                {
                    // Lasketaan pikselille valoisuus
                    pixel_color = ((float)pixel_color * (float)(blend_modes.exposure_mode_value / 255.0f));
                }

                // Asetetaan pikseli näyttöpuskuriin
                screen_buffer[(yh * GRAPH_SCREEN_WIDTH + xw) + (y * GRAPH_SCREEN_WIDTH + x)] = pixel_color;
            }
        }
    }
}

void graph_copy_array(graph_image_t * img, uint32_t x, uint32_t y)
{
    // Jos kuvan korkeus on yli 0
    if(img->height > 0){
        // Suoritetaan taulukon siirto dma:lla näyttöpuskuriin
        dma0_copy_2d(
            (uint32_t)img->data,
            0, 
            (uint32_t)&screen_buffer + ((y * GRAPH_SCREEN_WIDTH + x) * (GRAPH_SCREEN_DEPTH / 8)), 
            (GRAPH_SCREEN_WIDTH - img->width) * (GRAPH_SCREEN_DEPTH / 8),
            img->width * (GRAPH_SCREEN_DEPTH / 8), 
            (img->height - 1)
        );
    }
}

void graph_copy_array_rect(graph_image_t * img, uint32_t x, uint32_t y, uint32_t rect_w, uint32_t rect_h, uint32_t rect_x, uint32_t rect_y)
{
    // Jos kuvan korkeus on yli 0
    if(img->height > 0){
        // Suoritetaan taulukon siirto dma:lla näyttöpuskuriin
        dma0_copy_2d(
            (uint32_t)img->data + ((rect_y * img->width + rect_x) * (GRAPH_SCREEN_DEPTH / 8)),
            (img->width - rect_w) * (GRAPH_SCREEN_DEPTH / 8),
            (uint32_t)&screen_buffer + ((y * GRAPH_SCREEN_WIDTH + x) * (GRAPH_SCREEN_DEPTH / 8)), 
            (GRAPH_SCREEN_WIDTH - rect_w) * (GRAPH_SCREEN_DEPTH / 8),
            rect_w * (GRAPH_SCREEN_DEPTH / 8), 
            (rect_h - 1)
        );
    }
}

void graph_put_text(const char *text, uint32_t x, uint32_t y)
{
    // Määritellään fontti
    graph_image_t img_font = {0};
    img_font.width = 128;
    img_font.height = 128;
    img_font.data = (uint32_t *)&font_8x8;

    // Loopataan niin kauan, että NUl terminate tulee vastaan
    for(uint32_t i = 0; text[i] != 0x00; i++)
    {
        // Lasketaan rivien määrä
        uint8_t char_y = (text[i] / 16);
        // Lasketaan sarakkeiden määrä
        uint8_t char_x = (text[i] - (char_y * 16));

        graph_copy_array_rect(&img_font, x + (i * 8), y, 8, 8, (char_x * 8), (char_y * 8));
    }

}

void graph_flip()
{
    // Kopioidaan näyttöpuskurin data framebufferiin aka videomuistiin
    dma0_copy((uint32_t)&screen_buffer, get_current_mem_addr(), GRAPH_SCREEN_SIZE * (GRAPH_SCREEN_DEPTH / 8));

    // Siirretään offsettia
    fb_set_virtual_offset(0, GRAPH_SCREEN_HEIGHT * grap_cb.page);
    // Vaihdetaan sivutusta käänteiseen tilaan
    grap_cb.page = (!grap_cb.page);
}

