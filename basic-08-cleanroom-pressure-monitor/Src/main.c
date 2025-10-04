#include <stdint.h>
#include <stdio.h>
#include "bmp280.h"
#include "uart.h"
#include "systick.h"

static uint8_t status;
static uint8_t press_buffer[3];
static uint8_t temp_buffer[3];
static bmp280_cal_t bmp280_cal;
static int32_t t_fine;
static int32_t  T_centi;
static uint32_t P_pa;
static uint8_t log_str[64];

static uint32_t bmp280_comp_press(uint32_t adc_P)
{
    int64_t var1 = ((int64_t)t_fine) - 128000;
    int64_t var2 = var1 * var1 * (int64_t)bmp280_cal.dig_P6;
    var2 += ((var1 * (int64_t)bmp280_cal.dig_P5) << 17);
    var2 += ((int64_t)bmp280_cal.dig_P4 << 35);
    var1 = ((var1 * var1 * (int64_t)bmp280_cal.dig_P3) >> 8) + ((var1 * (int64_t)bmp280_cal.dig_P2) << 12);
    var1 = (((((int64_t)1) << 47) + var1) * (int64_t)bmp280_cal.dig_P1) >> 33;
    if (var1 == 0) return 0; // avoid div/0

    int64_t p = 1048576 - (int64_t)adc_P;
    p = (((p << 31) - var2) * 3125) / var1;
    var1 = ((int64_t)bmp280_cal.dig_P9 * (p >> 13) * (p >> 13)) >> 25;
    var2 = ((int64_t)bmp280_cal.dig_P8 * p) >> 19;
    p = ((p + var1 + var2) >> 8) + ((int64_t)bmp280_cal.dig_P7 << 4);
    return (uint32_t)p;  // Pa
}



static int32_t bmp280_comp_temp(uint32_t adc_T)
{
    int32_t var1 = ((((int32_t)adc_T >> 3) - ((int32_t)bmp280_cal.dig_T1 << 1)) * (int32_t)bmp280_cal.dig_T2) >> 11;
    int32_t var2 = ((((((int32_t)adc_T >> 4) - (int32_t)bmp280_cal.dig_T1) * (((int32_t)adc_T >> 4) - (int32_t)bmp280_cal.dig_T1)) >> 12) * (int32_t)bmp280_cal.dig_T3) >> 14;
    t_fine = var1 + var2;
    return (t_fine * 5 + 128) >> 8;   // °C * 100
}

int main(void)
{
	/* Init UART */
	uart_init();

	/* Init sensor */
	bmp280_init();

	/* Read sensor ID */
	uint8_t dev_id = 0;
	bmp280_read_address(0xD0, &dev_id);

	/* Read sensor's trimming parameters from its NVM */
	bmp280_read(0x88, 24, (uint8_t *)&bmp280_cal);

	/* Oversampling temp: x4 (011); Oversampling press: x4 (011); power mode: normal (11) */
	uint8_t ctrl_meas = 0b01101111;
	bmp280_write(0xF4, &ctrl_meas);

    /* Loop forever */
	while (1) {
		/* Wait sensor status not to be measuring or updating NVM */
		do{
			bmp280_read(0xF3, 1, &status);
		} while (status & 0b1001);

		/* Read pressure */
		bmp280_read(0xF7, 3, &press_buffer[0]);

		/* Read temperature */
		bmp280_read(0xFA, 3, &temp_buffer[0]);

		/* Compute pressure - press_msb(8) press_lsb(8) press_xlsb(4) */
		uint32_t adc_P = ((uint32_t)press_buffer[0] << 12) |
		                 ((uint32_t)press_buffer[1] << 4)  |
		                 ((uint32_t)press_buffer[2] >> 4);

		/* Compute temperature - temp_msb(8) temp_lsb(8) temp_xlsb(4) */
		uint32_t adc_T = ((uint32_t)temp_buffer[0] << 12) |
		                 ((uint32_t)temp_buffer[1] << 4)  |
		                 ((uint32_t)temp_buffer[2] >> 4);

        T_centi = bmp280_comp_temp(adc_T); // °C * 100
        P_pa    = bmp280_comp_press(adc_P) >> 8; // Pa

        /* Build and print info */
        snprintf((char *)log_str, sizeof(log_str),
                 "T=%ld.%02ld C, P=%lu.%02lu hPa\r\n",
                 T_centi / 100,
                 T_centi % 100,
                 P_pa / 100,
                 P_pa % 100);
        uart_print((char *)log_str);

        systick_delay_ms(1000);
	}
}
