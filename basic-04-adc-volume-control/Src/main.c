#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "adc.h"
#include "uart.h"

int sensor_value;

int main(void)
{
	adc1_ch1_init();
	uart_init();
	uart_print("Simple audio system with adjustable volume ready!\r\n");

	adc1_ch1_start_conversion();

	while(1)
	{
		sensor_value = adc_read();

		char volume_str[100];
		memset(volume_str, 0, sizeof(volume_str));
		snprintf(volume_str, sizeof(volume_str), "Sensor value = %d\r\n", sensor_value);
		uart_print(volume_str);

		/* 0 -> 0%, 4095 -> 100% */
		int volume_percentage = sensor_value * 100 / 4096;
		memset(volume_str, 0, sizeof(volume_str));
		snprintf(volume_str, sizeof(volume_str), "Volume = %d%%\r\n\n", volume_percentage);
		uart_print(volume_str);
	}
}
