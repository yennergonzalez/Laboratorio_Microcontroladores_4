// Laboratorio 4 Microcontroladores -	Sismografo
// Yenner Gonzalez Araya 			-	B83375
// Gabriel Barahona Otoya 			-	B70896


// - - - - - - - - - - - - - - - - INCLUDES - - - - - - - - - - - - - - - -

// gpio
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>

// battery/adc
#include <libopencm3/stm32/adc.h>

// comms
#include <string.h>
#include <libopencm3/stm32/usart.h>

// lcd/gyro
#include <stdint.h>
#include <math.h>
#include "clock.h"
#include "console.h"
#include "sdram.h"
#include "lcd-spi.h"
#include "gfx.h"
#include <libopencm3/stm32/spi.h>

// - - - - - - - - - - - - - - - - DEFINES - - - - - - - - - - - - - - - -

#define GYR_RNW					(1 << 7) /* Write when zero */
#define GYR_MNS					(1 << 6) /* Multiple reads when 1 */
#define GYR_WHO_AM_I			0x0F
#define GYR_OUT_TEMP			0x26
#define GYR_STATUS_REG			0x27
#define GYR_CTRL_REG1			0x20
#define GYR_CTRL_REG1_PD		(1 << 3)
#define GYR_CTRL_REG1_XEN		(1 << 1)
#define GYR_CTRL_REG1_YEN		(1 << 0)
#define GYR_CTRL_REG1_ZEN		(1 << 2)
#define GYR_CTRL_REG1_BW_SHIFT	4
#define GYR_CTRL_REG4			0x23
#define GYR_CTRL_REG4_FS_SHIFT	4

#define GYR_OUT_X_L				0x28
#define GYR_OUT_X_H				0x29
#define GYR_OUT_Y_L				0x2A
#define GYR_OUT_Y_H				0x2B
#define GYR_OUT_Z_L				0x2C
#define GYR_OUT_Z_H				0x2D

#define L3GD20_SENSITIVITY_250DPS  (0.00875F)  

// - - - - - - - - - - - - - - - - GLOBAL VARIABLES - - - - - - - - - - - - - - - -

// Store data read from gyro axes - initialized as zero
int16_t axis_data_x = 0;
int16_t axis_data_y = 0;
int16_t axis_data_z = 0;

// - - - - - - - - - - - - - - - - SETUP FUNCTIONS - - - - - - - - - - - - - - - -

// gpio
static void gpio_setup(void)
{
	// Enable G Port Clock Running Mode  
	rcc_periph_clock_enable(RCC_GPIOG);

	// Set up GPIO G13 as OUTPUT (Green LED - LD3)
	gpio_mode_setup(GPIOG, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO13);
}


// button
static void button_setup(void)
{
	// Enable A Port Clock Running Mode
	rcc_periph_clock_enable(RCC_GPIOA);
	// Set up GPIO A0 as INPUT (USER switch)
	gpio_mode_setup(GPIOA, GPIO_MODE_INPUT, GPIO_PUPD_NONE, GPIO0);
}



// battery
static void batt_setup(void)
{
	// Enable G Port Clock Running Mode
	rcc_periph_clock_enable(RCC_GPIOG);
	// Set up GPIO G14 as OUTPUT (Red LED - LD4)
	gpio_mode_setup(GPIOG, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO14);
}


// adc
static void adc_setup(void)
{
	// Enable ADC1 Clock Running Mode
	rcc_periph_clock_enable(RCC_ADC1);
	// Set up GPIO A1 as ANALOG INPUT
	gpio_mode_setup(GPIOA, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, GPIO1);
	// ADC starts turned off
	adc_power_off(ADC1);
	adc_disable_scan_mode(ADC1);
	adc_set_sample_time_on_all_channels(ADC1, ADC_SMPR_SMP_3CYC);
	// Turns on ADC
	adc_power_on(ADC1);
}


// comms
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


// gyro
static void spi_setup(void)
{
	rcc_periph_clock_enable(RCC_SPI5);
    rcc_periph_clock_enable(RCC_GPIOC);
	rcc_periph_clock_enable(RCC_GPIOF);

    /* select chip line */
	gpio_mode_setup(GPIOC, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO1);
    gpio_set(GPIOC, GPIO1);
    gpio_mode_setup(GPIOF, GPIO_MODE_AF, GPIO_PUPD_NONE,
		GPIO7 | GPIO8 | GPIO9);   
	gpio_set_af(GPIOF, GPIO_AF5, GPIO7 | GPIO8 | GPIO9);
    // gyro initialization and configuration
    spi_set_master_mode(SPI5);
	spi_set_baudrate_prescaler(SPI5, SPI_CR1_BR_FPCLK_DIV_64);
	spi_set_clock_polarity_0(SPI5);
	spi_set_clock_phase_0(SPI5);
	spi_set_full_duplex_mode(SPI5);
	spi_set_unidirectional_mode(SPI5); /* bidirectional but in 3-wire */
	spi_enable_software_slave_management(SPI5);
	spi_send_msb_first(SPI5);
	spi_set_nss_high(SPI5);
    SPI_I2SCFGR(SPI5) &= ~SPI_I2SCFGR_I2SMOD;  
	spi_enable(SPI5);

	gpio_clear(GPIOC, GPIO1);
	spi_send(SPI5, GYR_CTRL_REG1); 
	spi_read(SPI5);
	spi_send(SPI5, GYR_CTRL_REG1_PD | GYR_CTRL_REG1_XEN |
			GYR_CTRL_REG1_YEN | GYR_CTRL_REG1_ZEN |
			(3 << GYR_CTRL_REG1_BW_SHIFT));
	spi_read(SPI5);
	gpio_set(GPIOC, GPIO1); 
    gpio_clear(GPIOC, GPIO1);
	spi_send(SPI5, GYR_CTRL_REG4);
	spi_read(SPI5);
	spi_send(SPI5, (1 << GYR_CTRL_REG4_FS_SHIFT));
	spi_read(SPI5);
	gpio_set(GPIOC, GPIO1);
}


// - - - - - - - - - - - - - - - - READ FUNCTIONS - - - - - - - - - - - - - - - -

// adc
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

// gyro
void read_xyz(void)
{	
	// read x axis
	gpio_clear(GPIOC, GPIO1);
	spi_send(SPI5, GYR_OUT_X_L | GYR_RNW);
	spi_read(SPI5);
	spi_send(SPI5, 0);
	axis_data_x = spi_read(SPI5);
	gpio_set(GPIOC, GPIO1);

	gpio_clear(GPIOC, GPIO1);
	spi_send(SPI5, GYR_OUT_X_H | GYR_RNW);
	spi_read(SPI5);
	spi_send(SPI5, 0);
	axis_data_x |= spi_read(SPI5) << 8;
	gpio_set(GPIOC, GPIO1);

	// read y axis
	gpio_clear(GPIOC, GPIO1);
	spi_send(SPI5, GYR_OUT_Y_L | GYR_RNW);
	spi_read(SPI5);
	spi_send(SPI5, 0);
	axis_data_y =spi_read(SPI5);
	gpio_set(GPIOC, GPIO1);

	gpio_clear(GPIOC, GPIO1);
	spi_send(SPI5, GYR_OUT_Y_H | GYR_RNW);
	spi_read(SPI5);
	spi_send(SPI5, 0);
	axis_data_y|= spi_read(SPI5) << 8;
	gpio_set(GPIOC, GPIO1);

	// read z axis
	gpio_clear(GPIOC, GPIO1);
	spi_send(SPI5, GYR_OUT_Z_L | GYR_RNW);
	spi_read(SPI5);
	spi_send(SPI5, 0);
	axis_data_z=spi_read(SPI5);
	gpio_set(GPIOC, GPIO1);

	gpio_clear(GPIOC, GPIO1);
	spi_send(SPI5, GYR_OUT_Z_H | GYR_RNW);
	spi_read(SPI5);
	spi_send(SPI5, 0);
	axis_data_z|= spi_read(SPI5) << 8;
	gpio_set(GPIOC, GPIO1);

	axis_data_x = axis_data_x*L3GD20_SENSITIVITY_250DPS;
	axis_data_y = axis_data_y*L3GD20_SENSITIVITY_250DPS;
	axis_data_z = axis_data_z*L3GD20_SENSITIVITY_250DPS;
}


// - - - - - - - - - - - - - - - - MAIN - - - - - - - - - - - - - - - -
int main(void)
{	
	bool tx_disable;

	// run setup functions
	gpio_setup();
	button_setup();
	batt_setup();
	adc_setup();
	clock_setup();
	console_setup(115200);
	sdram_init();
	usart_setup();
	spi_setup();

	lcd_spi_init();
	gfx_init(lcd_draw_pixel, 240, 320);
	gfx_fillScreen(LCD_BLACK);


	// run loop
	while (1) {

		// Read values and inputs

		// battery
		uint16_t input_adc1 = read_adc_naiive(1);
		int norm_batt_value = (input_adc1*100)/4096; 
		char adcstr[8];
		char* adcptr = &adcstr;
		itoa(norm_batt_value, adcstr, 10);

		// button/switch
		tx_disable = gpio_get(GPIOA, GPIO0);

		// gyro

		// Correct gyro bias
		int16_t past_axis_data_x = axis_data_x;
		int16_t past_axis_data_y = axis_data_y;
		int16_t past_axis_data_z = axis_data_z;

		gpio_clear(GPIOC, GPIO1); 
		read_xyz();
		gpio_set(GPIOC, GPIO1);

		int16_t print_axis_data_x = axis_data_x-past_axis_data_x;
		int16_t print_axis_data_y = axis_data_y-past_axis_data_y;
		int16_t print_axis_data_z = axis_data_z-past_axis_data_z;

		char xstr[16], ystr[16], zstr[16];
		char* accxptr = &xstr;
		char* accyptr = &ystr;
		char* acczptr = &zstr;

		itoa(print_axis_data_x, xstr, 10);
		itoa(print_axis_data_y, ystr, 10);
		itoa(print_axis_data_z, zstr, 10);

		// LEDs blink/off
	
		// Battery LED
		if (norm_batt_value < 75){
			gpio_toggle(GPIOG, GPIO14);
		}
		else{
			gpio_clear(GPIOG, GPIO14);
		}

		// Comms LED
		if (!tx_disable){
			gpio_toggle(GPIOG, GPIO13);
		}
		else{
			gpio_clear(GPIOG, GPIO13);
		}

		// Show LCD
		gfx_fillScreen(LCD_BLACK);
		gfx_setTextColor(LCD_BLUE, LCD_BLACK);
		gfx_setTextSize(2);			
		gfx_setCursor(23, 30);


		gfx_puts("_Sismografo_");		

		// gyro - LCD
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
		gfx_puts("Battery: ");
		gfx_setCursor(90, 250);
		gfx_puts(adcptr);
		gfx_puts(" %");

		gfx_setCursor(10, 280);
		gfx_puts("Transmitting: ");

		// TX - LCD
		if (tx_disable){
			gfx_setCursor(120, 280);
			gfx_puts("OFF");;
		}
		else{
			gfx_setCursor(120, 280);
			gfx_puts("ON");
		}

		lcd_show_frame();


		// USART transmit
		if (!tx_disable){
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
			usart_send_blocking(USART1, ',');
			usart_send_blocking(USART1, '\r');
			usart_send_blocking(USART1, '\n');
		}
		
		for (int i = 0; i < 800000; i++)    /* Wait a bit. */
			__asm__("nop");
		
	}
}
