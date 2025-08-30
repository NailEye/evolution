/********************************************
  General Purpose I/O määritykset
*********************************************/

#include <common/common.h>
#include <mmio/mmio.h>

// Asettaa annettujen GPIO pinnien pull up/down resistorin tilan
static inline void gpio_set_pud(uint32_t pins, uint32_t pud)
{
    // Kirjoitetaan pud tila, johon pinnit asetetaan
    mmio_write(GPIO_GPPUD, pud);
    // Odotetaan speksin mukainen 150 sykliä
    cycle_wait(150);
    // Kirjoitetaan pinnit johon pud rekisterin mukainen tila asetetaan
    mmio_write(GPIO_GPPUDCLK0, pins);
    // Odotetaan speksin mukainen 150 sykliä
    cycle_wait(150);
    // Resetoidaan GPPUDCLK0 (aikaisemmin asetetut asetukset tulevat voimaan)
    mmio_write(GPIO_GPPUDCLK0, GPIO_RESET);
}
