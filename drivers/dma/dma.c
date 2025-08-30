
#include <common/delay.h>
#include <mmio/mmio.h>
#include <config/config.h>
#include <uart/uart.h>

// DMA ControlBlock rakenne
typedef struct dma_cb
{
    uint32_t ti; // DMA siirron ohjaustiedot.
    uint32_t src_addr; // Lähdeosoite josta tieto siirretään 31:0.
    uint32_t dst_addr; // Kohdeosoite johon tieto siirretään 31:0.
    uint32_t length; // Siirrettävien tavujen määrä 29:0. TDMODE:ssa 29:16 YLENGTH, 15:0 XLENGTH  
    uint32_t stride; // Käyttö vain TDMODE:ssa. Harppauksen koko 31:16 D_STRIDE, 15:0 S_STRIDE
    uint32_t nextconbk_addr; // Seuraavan ControlBlockin osoite 31:0.
    uint32_t reserved1; // Pitää asettaa nollaksi
    uint32_t reserved2; // Pitää asettaa nollaksi
} dma_cb_t;

// DMA0 Control Block (speksin mukaisesti align 32-byte aka 256-bit)
volatile dma_cb_t __attribute__((aligned(32))) dma0_cb = {0};

void dma_enable(uint32_t enb)
{
    mmio_write(DMA_ENABLE, enb);
}

void dma0_reset()
{
    mmio_write(DMA_DMA0_CS, DMA_DMA0_CS_RESET);
}

void dma0_clear(uint32_t dst_addr, uint32_t length)
{
    // Kirjoitetaan control blockin osoite DMA0:lle
    mmio_write(DMA_DMA0_CONBLK_AD, ((uint32_t)&dma0_cb | 0xC0000000));

    // Asetetaan control blockin asetukset
    dma0_cb.ti = (DMA_DMA0_TI_DEST_INC | DMA_DMA0_TI_SRC_IGNORE | DMA_DMA0_TI_SRC_WIDTH_128 | DMA_DMA0_TI_DEST_WIDTH_128);
    dma0_cb.src_addr = 0x000000000;
    dma0_cb.dst_addr = BUS_ADDR_TO_PHY_ADDR(dst_addr);
    dma0_cb.length = length;
    dma0_cb.stride = 0x00;
    dma0_cb.nextconbk_addr = 0x00;

    // Käynnistetään DMA siirto
    mmio_write(DMA_DMA0_CS, DMA_DMA0_CS_ACTIVE);

    // Odotellaan, että DMA siirto on valmis
    while((mmio_read(DMA_DMA0_CS) & DMA_DMA0_CS_ACTIVE));

    // Siivotaan lopetus
    mmio_write(DMA_DMA0_CS, DMA_DMA0_CS_END);
}

void dma0_copy(uint32_t src_addr, uint32_t dst_addr, uint32_t length)
{
    // Kirjoitetaan control blockin osoite DMA0:lle
    mmio_write(DMA_DMA0_CONBLK_AD, ((uint32_t)&dma0_cb | 0xC0000000));

    // Asetetaan control blockin asetukset
    dma0_cb.ti = (DMA_DMA0_TI_DEST_INC | DMA_DMA0_TI_SRC_INC | DMA_DMA0_TI_SRC_WIDTH_128 | DMA_DMA0_TI_DEST_WIDTH_128);
    dma0_cb.src_addr = BUS_ADDR_TO_PHY_ADDR(src_addr);
    dma0_cb.dst_addr = BUS_ADDR_TO_PHY_ADDR(dst_addr);
    dma0_cb.length = length;
    dma0_cb.stride = 0x00;
    dma0_cb.nextconbk_addr = 0x00;

    // Käynnistetään DMA siirto
    mmio_write(DMA_DMA0_CS, DMA_DMA0_CS_ACTIVE);

    // Odotellaan, että DMA siirto on valmis
    while((mmio_read(DMA_DMA0_CS) & DMA_DMA0_CS_ACTIVE));

    // Siivotaan lopetus
    mmio_write(DMA_DMA0_CS, DMA_DMA0_CS_END);
}

void dma0_copy_2d(uint32_t src_addr, uint32_t src_stride, uint32_t dst_addr, uint32_t dst_stride, uint32_t x_length, uint32_t y_length)
{
    // Kirjoitetaan control blockin osoite DMA0:lle
    mmio_write(DMA_DMA0_CONBLK_AD, ((uint32_t)&dma0_cb | 0xC0000000));

    // Asetetaan control blockin asetukset
    dma0_cb.ti = ((0x0F << DMA_DMA0_TI_BURST_LENGTH_START) | DMA_DMA0_TI_DEST_INC | DMA_DMA0_TI_SRC_INC | DMA_DMA0_TI_TDMODE | DMA_DMA0_TI_SRC_WIDTH_128 | DMA_DMA0_TI_DEST_WIDTH_128);
    dma0_cb.src_addr = BUS_ADDR_TO_PHY_ADDR(src_addr);
    dma0_cb.dst_addr = BUS_ADDR_TO_PHY_ADDR(dst_addr);
    dma0_cb.length = (x_length & 0xFFFF) | ((y_length & 0x3FFF) << 16);
    dma0_cb.stride = (src_stride & 0xFFFF) | ((dst_stride & 0xFFFF) << 16);
    dma0_cb.nextconbk_addr = 0x00;

    // Käynnistetään DMA siirto
    mmio_write(DMA_DMA0_CS, DMA_DMA0_CS_ACTIVE);

    // Odotellaan, että DMA siirto on valmis
    while((mmio_read(DMA_DMA0_CS) & DMA_DMA0_CS_ACTIVE));

    // Siivotaan lopetus
    mmio_write(DMA_DMA0_CS, DMA_DMA0_CS_END);
}
