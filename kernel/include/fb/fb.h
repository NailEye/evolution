#pragma once

#include <config/config.h>
#include <common/types.h>
#include <mb/mb.h>

typedef struct fb_info
{
    uint32_t width;
    uint32_t height;
    uint32_t depth;
    uint32_t mem_addr;
    uint32_t mem_size;
} fb_info_t;

typedef vector2_t fb_screen_resolution_t;

// Osoitin framebufferin muistiin
static fb_info_t info;

static inline void fb_init(fb_screen_resolution_t *phy_res, fb_screen_resolution_t *vir_res, uint8_t depth)
{
    // Puskurin tiedot luetaan tähän
    uint32_t address_size[2] = {0x00};

    // Koitetaan alustaa framebufferi
    if(mb_fb_init(phy_res, vir_res, depth, address_size))
    {
        // Onnistui, joten asetetaan fb puskurin base osoite ja pituus
        info.width = phy_res->x;
        info.height = phy_res->y;
        info.depth = depth;
        info.mem_addr = (address_size[0] & MEMORY_RAM_END);
        info.mem_size = address_size[1];
    }
}

static inline void fb_set_virtual_offset(uint32_t x, uint32_t y)
{
    // Asetetaan virtuaali "näytön" offset
    mb_fb_set_offset(x, y);
}

static inline void fb_write_pixel(uint32_t x, uint32_t y, uint32_t color)
{
    // Asetetaan pikseli sijaintiin
    *(volatile uint32_t *)(info.mem_addr + ((x + (info.width * y)) * (info.depth / 8))) = color;
}

static inline fb_info_t *fb_get_info()
{
    // Palautetaan info
    return &info;
}
