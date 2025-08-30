/********************************************************
 Universal Asynchronous Receiver Transmitter määritykset
*********************************************************/

#include <gpio/gpio.h>
#include <common/math.h>
#include <common/cstring.h>

#define UART_FIFO_ENABLE            0x01
#define UART_FIFO_DISABLE           0x00

#define UART_FRAME_5BITS            0x05
#define UART_FRAME_6BITS            0x06
#define UART_FRAME_7BITS            0x07
#define UART_FRAME_8BITS            0x08

#define UART_PARITY_ENABLE         0x01
#define UART_PARITY_DISABLE        0x00

// Disabloi UART0
static inline void uart_set_control(uint32_t control)
{
    // Kirjoitetaan UART0 control asetus
    mmio_write(UART_CR, control);
}

static inline void uart_interrupt_clear(uint32_t flags)
{
    // Kirjoitetaan UART0 keskeytyksien tyhjennys rekisteriin
    mmio_write(UART_ICR, flags);
}

static inline void uart_set_baudrate(uint32_t baud)
{
    // Lasketaan jako
    float divider = (UART_CLOCK / (16 * baud));

    // Kirjoitetaan UART0 baudraten kokonaisluku osuus
    mmio_write(UART_IBRD, (uint32_t)divider);
    // Kirjoitetaan UART0 baudraten liukuluku osuus
    mmio_write(UART_FBRD, (uint32_t)((frac(divider) * 64) + 0.5));
}

static inline void uart_set_line_control(uint32_t control)
{
    // Kirjoiteatan linecontrol asetus
    mmio_write(UART_LCRH, control);
}

static inline void uart_set_interrupt_mask_control(uint32_t control) 
{
    // Kirjoitetaan control asetus
    mmio_write(UART_IMSC, control);
}

static inline void uart_putc(unsigned char c) 
{
    // Kytätään TXFF-bittiä, ollaanko valmiita lähetykseen
    while(mmio_read(UART_FR) & UART_FR_TXFF);
    // Kirjoitetaan merkki data rekisteriin
    mmio_write(UART_DR, c);
}

static inline unsigned char uart_getc()
{
    // Kytätään RXFE-bittiä, ollaanko valmiita vastaanottoon
    while(mmio_read(UART_FR) & UART_FR_RXFE);
    // Luetaan merkki data rekisteristä ja palautetaan se
    return mmio_read(UART_DR);
} 

static inline void uart_puts(const char *str) 
{
    // Haetaan stringin pituus
    size_t length = strlen(str);

    // Loopataan pituuden verran
    for(size_t i = 0; i < length; i++) 
    {
        // Kirjoitetaan loopin osoittama merkki
        uart_putc((unsigned char)str[i]);
    }
}

static inline void uart_hex(uint64_t value, uint8_t len)
{
    // Nykyinen 4 bittiä
    uint8_t curr;
    // Loopataan 32bit läpi
    for(int i = ((len * 8) - 4); i >= 0; i -= 4) 
    {
        // Shiftataan i verran bittejä ja maskataan 4 bittiä
        curr = (value >> i) & 0xF;
        // Muutetaan osoittamaan 0-9 merkkejä ja 10-15 A -> F
        curr += (curr > 9) ? 0x37:0x30;
        // Lähetetään merkki
        uart_putc(curr);
    }
}

static inline void uart_init(uint32_t baud, uint8_t frameBits, uint8_t parity, uint8_t enableFifo) 
{
    // Määritellään linecontrol
    uint32_t lineControl = 0x00000000;

    // Framen pituuden määritys bitti määrityksen mukaan
    switch(frameBits) {
        // 6-bittinen frame
        case UART_FRAME_6BITS:
            lineControl |= UART_LCRH_WLEN_6;
        break;
        // 7-bittinen frame
        case UART_FRAME_7BITS:
            lineControl |= UART_LCRH_WLEN_7;
        break;
        // 8-bittinen frame
        case UART_FRAME_8BITS:
            lineControl |= UART_LCRH_WLEN_8;
        break;
        // Oletuksena 5-bittinen frame
        default:
        break;
    }

    // Jos Parity halutaan päälle
    if(parity == UART_PARITY_ENABLE) {
        // Lisätään lineControliin parityn enablointi
        lineControl |= UART_LCRH_PEN;
    }

    // Jos FIFO halutaan päälle
    if(enableFifo == UART_FIFO_ENABLE) {
        // Lisätään lineControliin fifon enablointi
        lineControl |= UART_LCRH_FEN;
    }

    // Disabloidaan UART0 (speksi sanoo, että aina kun uarttia konffitaan pitää ensin disabloida)
    uart_set_control(UART_CR_DISABLE);

    // Poistetaan UART pinneiltä (14 ja 15) pull up/down resistori
    gpio_set_pud(1 << 14 | 1 << 15, GPIO_GPPUD_DISABLE);

    // Tyhjennetään kaikki odottavat keskeytykset
    uart_interrupt_clear(UART_ICR_ALL);
    // Asetetaan baudrate
    uart_set_baudrate(baud);
    // Kirjoitetaan line control asetukset
    uart_set_line_control(lineControl);
    // Maskataan kaikki keskeytykset
    uart_set_interrupt_mask_control(UART_IMSC_ALL);

    // Enabloidaan UART0 sekä sen Rx/Tx
    uart_set_control(UART_CR_UARTEN|UART_CR_TXE|UART_CR_RXE);
}

