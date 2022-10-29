// Laboratorio 4 Microcontroladores
// Yenner Gonzalez Araya - B83375
// Gabriel Barahona Otoya - B7....


// - - - - - - - - - - - - - - - - INCLUDES - - - - - - - - - - - - - - - -
    // button
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>

    // lcd 
#include <stdint.h>
#include <math.h>
#include <setjmp.h>
#include "sdram.h"
#include "lcd-spi.h"
#include "gfx.h"
#include "clock.h"
#include "console.h"

    // gyro







// global variables




// - - - - - - - - - - - - - - - - SETUP FUNCTIONS - - - - - - - - - - - - - - - -

// clock setup

// gpio setup
static void gpio_setup(void)
{
	/* Using API functions: */
	rcc_periph_clock_enable(RCC_GPIOG);

	/* Set GPIO13 (in GPIO port G) to 'output push-pull'. */
	gpio_mode_setup(GPIOG, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO13);

}

// button setup

static void button_setup(void)
{
	/* Enable GPIOA clock. */
	rcc_periph_clock_enable(RCC_GPIOA);

	/* Set GPIO0 (in GPIO port A) to 'input open-drain'. */
	gpio_mode_setup(GPIOA, GPIO_MODE_INPUT, GPIO_PUPD_NONE, GPIO0);
}


// lcd setup



// gyro setup





// comms setup





// other functions

// read accel

// read batt

// print accel + batt on display


// check button/switch state

// if switch state == 1 
// send accel data
// turn on LED



// else
// turn off LED
// do not send accel data




// - - - - - - - - - - - - - - - - MAIN - - - - - - - - - - - - - - - -
int main (void)
{
    // setup

    clock_setup();
	button_setup();
    gpio_setup();


	console_setup(115200);
	sdram_init();
	lcd_spi_init();
	//lcd_show_frame();
	gfx_init(lcd_draw_pixel, 240, 320);
	gfx_fillScreen(LCD_BLACK);

	// funcionamiento continuo
	int i;
    while(1) {

		gfx_fillScreen(LCD_BLACK);
		gfx_setTextColor(LCD_BLUE, LCD_BLACK);
		gfx_setTextSize(2);
		gfx_setCursor(24, 30);


		gfx_puts("_Sismografo_");

		// get gyro_values
		char* accx = "123";
		char* accy = "852";
		char* accz = "375";

		// get batt_values


		// print display

		gfx_setCursor(20, 80);
		gfx_puts("Acc X: ");
		gfx_puts(accx);
		

		gfx_setCursor(20, 120);
		gfx_puts("Acc Y: ");
		gfx_puts(accy);

		gfx_setCursor(20, 160);
		gfx_puts("Acc Z: ");
		gfx_puts(accz);


		gfx_setTextSize(1);
		gfx_setCursor(10, 250);
		gfx_puts("Bateria: ");
		gfx_setCursor(10, 280);
		gfx_puts("Transmitting: ");


        // lee presionado boton
        if (gpio_get(GPIOA, GPIO0)) {
			// apagar led y comms
            gpio_clear(GPIOG, GPIO13);

			gfx_setCursor(120, 280);
			gfx_puts("OFF");
			lcd_show_frame();


		}
        else {
            // parpadea led si no se ha presionado el boton
            gpio_toggle(GPIOG, GPIO13);

			// funcionamiento comunicacion va aca
			gfx_setCursor(120, 280);
			gfx_puts("ON");
			lcd_show_frame();
        }

        for (i = 0; i < 30000000; i++) { // Espera un tiempo
			__asm__("nop");
		}
    }
    return 0;
}
