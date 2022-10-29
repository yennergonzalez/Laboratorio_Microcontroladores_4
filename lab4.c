// Laboratorio 4 Microcontroladores
// Yenner Gonzalez Araya - B83375
// Gabriel Barahona Otoya - B70896


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
#include <libopencm3/stm32/spi.h>

	// batt
#include <libopencm3/stm32/adc.h>

// - - - - - - - - - - - - - - - - DEFINES - - - - - - - - - - - - - - - -




// global variables




// - - - - - - - - - - - - - - - - SETUP FUNCTIONS - - - - - - - - - - - - - - - -


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




// gyro setup





// comms setup





// other functions

// read accel

// read batt
static void batt_setup(void)
{
	/* Enable GPIOA clock. */
	rcc_periph_clock_enable(RCC_GPIOC);

	/* Set GPIO3 (in GPIO port C) to 'input open-drain'. */
	gpio_mode_setup(GPIOC, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, GPIO3);

	// set up Orange LED as output
}



// - - - - - - - - - - - - - - - - MAIN - - - - - - - - - - - - - - - -
int main (void)
{
    // Setup

    clock_setup();
	button_setup();
	batt_setup();
    gpio_setup();

	console_setup(115200);
	sdram_init();
	lcd_spi_init();
	gfx_init(lcd_draw_pixel, 240, 320);
	gfx_fillScreen(LCD_BLACK);

	// funcionamiento continuo
    while(1) {
		gfx_fillScreen(LCD_BLACK);
		gfx_setTextColor(LCD_BLUE, LCD_BLACK);
		gfx_setTextSize(2);
		gfx_setCursor(24, 30);


		gfx_puts("_Sismografo_");

		// get gyro_values - placeholder values
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
		}
        else {
            // parpadea led si no se ha presionado el boton
            gpio_toggle(GPIOG, GPIO13);

			// funcionamiento comunicacion va aca
			gfx_setCursor(120, 280);
			gfx_puts("ON");
        }

		// Corregir esta comparacion
		if ((gpio_get(GPIOC, GPIO3)) < 3.9){
			gfx_setCursor(90, 250);
			gfx_puts("LOW");
		}
		lcd_show_frame();

        for (int i = 0; i < 30000000; i++) { // Espera un tiempo
			__asm__("nop");
		}
    }
    return 0;
}
