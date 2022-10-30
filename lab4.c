// Laboratorio 4 Microcontroladores - Sismografo
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
	// comms
#include <libopencm3/stm32/usart.h>
#include <string.h>

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

	gpio_mode_setup(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO9);

	/* Setup USART1 TX pin as alternate function. */
	gpio_set_af(GPIOA, GPIO_AF7, GPIO9);
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
static void usart_setup(void)
{
	/* Enable clocks for USART2. */
	rcc_periph_clock_enable(RCC_USART1);
	
	/* Setup USART2 parameters. */
	usart_set_baudrate(USART1, 115200);
	usart_set_databits(USART1, 8);
	usart_set_stopbits(USART1, USART_STOPBITS_1);
	usart_set_mode(USART1, USART_MODE_TX);
	usart_set_parity(USART1, USART_PARITY_NONE);
	usart_set_flow_control(USART1, USART_FLOWCONTROL_NONE);

	/* Finally enable the USART. */
	usart_enable(USART1);
}




// other functions

// read accel ------------------------------- begin
// read accel ------------------------------- end


static void batt_setup(void)
{
	rcc_periph_clock_enable(RCC_GPIOG);
	// set up Orange LED as output
	gpio_mode_setup(GPIOG, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO14);
}

static void adc_setup(void)
{
	rcc_periph_clock_enable(RCC_ADC1);
	gpio_mode_setup(GPIOA, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, GPIO1);
	adc_power_off(ADC1);
	adc_disable_scan_mode(ADC1);
	adc_set_sample_time_on_all_channels(ADC1, ADC_SMPR_SMP_3CYC);
	adc_power_on(ADC1);
}

static uint16_t read_adc_naiive(uint8_t channel)
{
	uint8_t channel_array[16];
	channel_array[0] = channel;
	adc_set_regular_sequence(ADC1, 1, channel_array);
	adc_start_conversion_regular(ADC1);
	while (!adc_eoc(ADC1));
	uint16_t reg16 = adc_read_regular(ADC1);
	return reg16;
}



// - - - - - - - - - - - - - - - - MAIN - - - - - - - - - - - - - - - -
int main (void)
{	


    // Setup

    clock_setup();
	button_setup();
	batt_setup();
    adc_setup();
	gpio_setup();

	console_setup(115200);
	sdram_init();
	lcd_spi_init();
	gfx_init(lcd_draw_pixel, 240, 320);
	gfx_fillScreen(LCD_BLACK);

	usart_setup();

	
	// funcionamiento continuo
    while(1) {


		gfx_fillScreen(LCD_BLACK);
		gfx_setTextColor(LCD_BLUE, LCD_BLACK);
		gfx_setTextSize(2);
		gfx_setCursor(24, 30);


		gfx_puts("_Sismografo_");

		// get gyro_values - placeholder values	
		

		
		
		
		int accx = 147;
		int accy = 1470;
		int accz = 252;



		char xstr[8], ystr[8], zstr[8];
		char* accxptr = &xstr;
		char* accyptr = &ystr;
		char* acczptr = &zstr;

		itoa(accx, xstr, 10);
		itoa(accy, ystr, 10);
		itoa(accz, zstr, 10);

		
		// get batt_values
		uint16_t input_adc1 = read_adc_naiive(1);
		int norm_batt_value = (input_adc1*100)/4096; 
		char adcstr[8];
		char* adcptr = &adcstr;
		itoa(norm_batt_value, adcstr, 10);


		// print gyro_values
		gfx_setCursor(20, 80);
		gfx_puts("Acc X: ");
		gfx_puts(accxptr);
		
		gfx_setCursor(20, 120);
		gfx_puts("Acc Y: ");
		gfx_puts(accyptr);

		gfx_setCursor(20, 160);
		gfx_puts("Acc Z: ");
		gfx_puts(acczptr);


		// print batt_and_comms_status
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


			for (int j = 0; j < strlen(xstr); j++)
			{
				usart_send_blocking(USART1, xstr[j]);	
			}
			usart_send_blocking(USART1, ',');
			for (int j = 0; j < strlen(ystr); j++)
			{
				usart_send_blocking(USART1, ystr[j]);	
			}
			usart_send_blocking(USART1, ',');
			for (int j = 0; j < strlen(zstr); j++)
			{
				usart_send_blocking(USART1, zstr[j]);	
			}
			usart_send_blocking(USART1, ',');
			for (int j = 0; j < strlen(adcstr); j++)
			{
				usart_send_blocking(USART1, adcstr[j]);	
			}
			usart_send_blocking(USART1, ';');
			usart_send_blocking(USART1, '\r');
			usart_send_blocking(USART1, '\n');
        }
		

		
		// Mostrar estado bateria y parpadear led advertencia de ser necesario
		gfx_setCursor(90, 250);
		gfx_puts(adcptr);
		gfx_puts(" %");

		if (input_adc1 < 3.9){
			gpio_toggle(GPIOG, GPIO14);
			// Enviar notificacion de bateria baja al dashboard

		}

		else{
			gpio_clear(GPIOG, GPIO14);
		}
		lcd_show_frame();

		// Esperar un tiempo
        for (int i = 0; i < 20000000; i++) {
			__asm__("nop");
		}
    }
    return 0;
}
