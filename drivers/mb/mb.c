/*************************************************
  Mailbox rajapinnan ajuri
**************************************************/

// Pohjautuu raspberrypi:n dokumentaatioihin:
// https://github.com/raspberrypi/firmware/wiki/Accessing-mailboxes
// https://github.com/raspberrypi/firmware/wiki/Mailbox-property-interface

#include <common/common.h>
#include <mmio/mmio.h>
#include <config/config.h>
#include <common/delay.h>
#include <uart/uart.h>
#include <common/types.h>

#define MB_REQUEST              0

// Kanavat
#define MBOX_CH_POWER           0
#define MBOX_CH_FB              1
#define MBOX_CH_VUART           2
#define MBOX_CH_VCHIQ           3
#define MBOX_CH_LEDS            4
#define MBOX_CH_BTNS            5
#define MBOX_CH_TOUCH           6
#define MBOX_CH_COUNT           7
#define MBOX_CH_PROP            8

#define MB_TAG_GETMAC           0x00010003
#define MB_TAG_GETSERIAL        0x00010004
#define MB_TAG_LAST             0

/**************** Framebuffer ********************/
#define MB_TAG_ALLOCATE_BUFFER              0x00040001
#define MB_TAG_SET_PHYSICAL_DISPLAY         0x00048003
#define MB_TAG_SET_VIRTUAL_DISPLAY          0x00048004
#define MB_TAG_GET_VIRTUAL_OFFSET           0x00040009
#define MB_TAG_SET_VIRTUAL_OFFSET           0x00048009
#define MB_TAG_SET_DEPTH                    0x00048005
#define MB_TAG_SET_PIXEL_ORDER              0x00048006

// Mailboxin viestipuskuri (speksin mukaisesti align 16)
volatile uint32_t __attribute__((aligned(16))) mb[36];

// Puskurin offsetti
uint32_t mb_pointer_offset = 0;

// Palauttaa mailboxin nykyisen tilan
uint32_t mb_read_status()
{
    // Luetaan mailboxin tila
    return ((uint32_t)mmio_read(MB_STATUS));
}

uint32_t mb_is_full() 
{
    // Maskataan tilasta full-bitit
    return (mb_read_status() & MB_FULL);
}

uint32_t mb_is_empty()
{
    // Maskataan tilasta empty-bitit
    return (mb_read_status() & MB_EMPTY);
}

// Kirjoittaa annetun arvon mailboxille
void mb_write(uint32_t value)
{
    // Kirjoiettaan arvo mailboxille
    mmio_write(MB_WRITE, value);
}

// Lukee mailboxin viestin
uint32_t mb_read()
{
    // Luetaan viesti mailboxista
    return mmio_read(MB_READ);
}

// Lähettää mailbox kutsun.
// Paluuarvot: 0 = Epäonnistui, !0 = Ok
void mb_call(uint8_t ch)
{
    // [28bit address, 4bit channel]
    // Yhdistetään puskurin osoite ja kanava
    // Maskataan mb osoitteesta ekat 4bittiä
    // Liitetään channelin ekat 4bittiä
    uint32_t addressChannel = (((uint32_t)((unsigned long)&mb)&~0xF) | (ch & 0xF));

    // Odotetaan kunnes voidaan kirjoittaa
    do{NOP} while(mb_is_full());

    // Kirjoitetaan osoite ja kanava mailboxille
    mb_write(addressChannel);

    // Aloitetaan odottamaan vastausta mailbox rajapinnalta
    while(1) 
    {
        // Odotetaan, jos mailbox on tyhjä
        do{NOP} while(mb_is_empty());
        // Jos vastaus on meidän viestille
        if(addressChannel == mb_read())
        {
            // Poistutaan loopista
            break;
        }
    }

    // Odotellaan, että kyseessä on vastaus
    while((mb[1] & MB_RESPONSE) == 0);
}

void mb_begin_call()
{
    // Nollataan osoitin offsetti
    mb_pointer_offset = 0;
}

void mb_add_tag(uint32_t tag)
{
    // Aloituskohta alkaa 2, koska 0/1 on varattu headereille
    mb[mb_pointer_offset + 2] = tag;
    mb_pointer_offset++;
    // Ei arvoja, joten pituus 0
    mb[mb_pointer_offset + 2] = 0;
    mb_pointer_offset++;
    // Request/Reponse indikointi ylempi 1bit, alemmat indikoi arvon pituutta tavuina
    mb[mb_pointer_offset + 2] = 0;
    mb_pointer_offset++;
}

void mb_add_tag_value1(uint32_t tag, uint32_t value)
{
    // Aloituskohta alkaa 2, koska 0/1 on varattu headereille
    mb[mb_pointer_offset + 2] = tag;
    mb_pointer_offset++;
    // Value1 kirjoittaa vain yhden arvon eli 32-bit
    mb[mb_pointer_offset + 2] = 4;
    mb_pointer_offset++;
    // Request/Reponse indikointi ylempi 1bit, alemmat indikoi arvon pituutta tavuina
    mb[mb_pointer_offset + 2] = 4;
    mb_pointer_offset++;
    // Asetetaan tagille arvo
    mb[mb_pointer_offset + 2] = value;
    mb_pointer_offset++;
}

void mb_add_tag_value2(uint32_t tag, uint32_t value1, uint32_t value2)
{
    // Aloituskohta alkaa 2, koska 0/1 on varattu headereille
    mb[mb_pointer_offset + 2] = tag;
    mb_pointer_offset++;
    // Value2 kirjoittaa 2 x 32bit arvoa, eli 4*2
    mb[mb_pointer_offset + 2] = 8;
    mb_pointer_offset++;
    // Request/Reponse indikointi ylempi 1bit, alemmat indikoi arvon pituutta tavuina
    mb[mb_pointer_offset + 2] = 8;
    mb_pointer_offset++;
    // Asetetaan tagille arvo1
    mb[mb_pointer_offset + 2] = value1;
    mb_pointer_offset++;
    // Asetetaan tagille arvo2
    mb[mb_pointer_offset + 2] = value2;
    mb_pointer_offset++;
}

void mb_exec_call(uint8_t ch)
{
    // Asetetaan lopetus tagi
    mb[mb_pointer_offset + 2] = MB_TAG_LAST;
    mb_pointer_offset++;
    // Asetetaan request tyypiksi
    mb[1] = MB_REQUEST;
    mb_pointer_offset++;

    // Lisätään vielä 1 ennen koon laskentaa, koska laskennan pitää ottaa itsensä huomioon
    mb_pointer_offset++;
    // Asetetaan koko viestin koko
    mb[0] = (mb_pointer_offset * 4);

    // Suoritetaan mb kutsu
    mb_call(ch);
}

/**********
Pyyntöviestin rakenne
|BufferSize|u32
|BufferRequest/Response|u32
|TagIdentifier|u32
|ValueBufferSizeInBytes|u32
|Request/Response codes|u32
|ValueBuffer....|u8
|PaddingToAlignTheTagTo32bits|u8
**********/

void mb_get_mac(uint8_t *mac)
{
    // Aloitetaan mailbox kutsu
    mb_begin_call();
    // Asetetaan GETMAC tagi
    mb_add_tag(MB_TAG_GETMAC);

    // Suoritetaan mailbox kutsu
    mb_exec_call(MBOX_CH_PROP);

    // Mapataan arvot taulukkoon
    mac[0] = ((mb[5] >> 0) & 0xFF);
    mac[1] = ((mb[5] >> 8) & 0xFF);
    mac[2] = ((mb[5] >> 16) & 0xFF);
    mac[3] = ((mb[5] >> 24) & 0xFF);
    mac[4] = ((mb[6] >> 0) & 0xFF);
    mac[5] = ((mb[6] >> 8) & 0xFF);
}

void mb_get_sn(uint32_t *sn)
{
    // Aloitetaan mailbox kutsu
    mb_begin_call();
    // Asetetaan GETSERIAL tagi
    mb_add_tag(MB_TAG_GETSERIAL);

    // Suoritetaan mailbox kutsu
    mb_exec_call(MBOX_CH_PROP);

    // Asetetaan sarjanumero
    sn[0] = mb[5];
    sn[1] = mb[6];
}

void mb_fb_init(vector2_t *phy_res, vector2_t *vir_res, uint32_t depth, uint32_t address_size[2])
{
    // Aloitetaan mailbox kutsu
    mb_begin_call();
    // Asetetaan näytön fyysinen resoluutio
    mb_add_tag_value2(MB_TAG_SET_PHYSICAL_DISPLAY, phy_res->x, phy_res->y);
    // Asetetaan näytön virtuaalinen resoluutio
    mb_add_tag_value2(MB_TAG_SET_VIRTUAL_DISPLAY, vir_res->x, vir_res->y);
    // Asetetaan näytön virtuaalinen offsetti (X,Y)
    mb_add_tag_value2(MB_TAG_SET_VIRTUAL_OFFSET, 0, 0);
    // Asetetaan näytön värisyvyys
    mb_add_tag_value1(MB_TAG_SET_DEPTH, depth);
    // Asetetaan näytön värijärjestys (RGB/BGR)
    mb_add_tag_value1(MB_TAG_SET_PIXEL_ORDER, 1);
    // Varataan framebufferille tila (arvoksi alignment in bytes)
    mb_add_tag_value2(MB_TAG_ALLOCATE_BUFFER, 4096, 0x00);

    // Suoritetaan mailbox kutsu
    mb_exec_call(MBOX_CH_PROP);

    // Maskataan osoittamaan fyysiseen muistiin
    //address_size[0] = (mb[28] & MEMORY_RAM_END);
    address_size[0] = mb[28];
    // Asetetaan fb puskurin koko tavuina
    address_size[1] = mb[29];
}

void mb_fb_set_offset(uint32_t x, uint32_t y)
{
    // Mailbox kutsu, offsetin asetusta varten
    mb_begin_call();
    // Asetetaan näytön virtuaalinen offsetti (X,Y)
    mb_add_tag_value2(MB_TAG_SET_VIRTUAL_OFFSET, x, y);

    // Suoritetaan mailbox kutsu
    mb_exec_call(MBOX_CH_PROP);
}