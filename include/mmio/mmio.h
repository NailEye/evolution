/********************************************
 Memory-mapped I/O määritykset
*********************************************/
#pragma once

#include <common/common.h>
#include <config/config.h>

// Kirjoittaa datan osoitettuun mmio osoitteeseen
static inline void mmio_write(uint32_t offset, uint32_t data) 
{
    // Kirjoitetaan data mmio osositteeseen
    *(volatile uint32_t *)(MMIO_BASE + offset) = data;
}

// Lukee datan osoitetusta mmio osoitteesta
static inline uint32_t mmio_read(uint32_t offset) {
    // Luetaan data mmio osoitteesta
    return *(volatile uint32_t *)(MMIO_BASE + offset);
}

static inline void mmio_enable(uint32_t offset, uint32_t data) 
{
    // Kirjoitetaan data mmio osositteeseen
    *(volatile uint32_t *)(MMIO_BASE + offset) |= data;
}