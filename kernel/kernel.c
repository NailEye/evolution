/*******************************
 Kernelin aloituskohdan määritykset
********************************/

#include <uart/uart.h>
#include <system/system.h>
#include <resources/logo.h>
#include <fb/fb.h>
#include <dma/dma.h>
#include <common/delay.h>
#include <graph/graph.h>





void kernel_main(uint32_t r0, uint32_t r1, uint32_t atags) {
	// "Käytetään käyttämättömänä ettei
    // kääntäjä mene sorkkimaan.
	(void) r0;
	(void) r1;
	(void) atags;

    // Alustetaan uart
    //uart_init(115200, UART_FRAME_8BITS, UART_PARITY_DISABLE, UART_FIFO_ENABLE);

    uint32_t logo_x = ((GRAPH_SCREEN_WIDTH/2) - (RES_LOGO_WIDTH/2));
    uint32_t logo_y = ((GRAPH_SCREEN_HEIGHT/2) - (RES_LOGO_HEIGHT/2));

    uint32_t center_text_x = ((GRAPH_SCREEN_WIDTH / 2) - (224 / 2));

    uint32_t bottom_text_y = (GRAPH_SCREEN_HEIGHT - 8);

    graph_init();

    // Valaistuksen taso
    uint8_t exposure_level = 0x00;
    // Fade suunta
    uint8_t fade_direction = 0;

    // Laitetaan exposure mode päälle
    graph_enable_exposure_mode();
    

    while(1){
        // Siivotaan ruutu
        graph_clear_screen();

        graph_put_text("Press any button to continue", center_text_x, 320);

        graph_put_text("Copyright (c) 2020 Samu Sutinen", 0, bottom_text_y);

        // Asetetaan valoisuus taso
        graph_set_value_exposure_mode(exposure_level);

        // Piirretään logo
        graph_put_array(&img_logo, logo_x, logo_y);

        // Jos ollaan kirkkaimillaan
        if(exposure_level == 0xFF && fade_direction == 0)
        {
            // Asetetaan fade suunta 1 = alaspäin
            fade_direction = 1;
        }else if(exposure_level < 0x80 && fade_direction == 1) {
            // Asetetaan fade suunta 0 = ylöspäin
            fade_direction = 0;
        }

        // Jos fade suunta on 0
        if(fade_direction == 0)
        {
            // Kasvatetaan valoisuus tasoa
            exposure_level += 5;
        }else{
            // Pienennetään
            exposure_level -= 5;
        }

        // Flipataan rendattu ruutu näkyviin
        graph_flip();
    }
}