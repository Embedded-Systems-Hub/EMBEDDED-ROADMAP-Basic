#include <stdio.h>
#include <string.h>
#include "gpio.h"
#include "uart.h"
#include "tim.h"
#include "systick.h"

#define STIMULUS_DELAY_MS	10000U

static bool detect_button_press(void)
{
	static bool prev_state = false;
	bool curr_state = btn_get_state();
	bool pressed = false;

	if (!prev_state && curr_state)
	{
		pressed = true;
	}
	prev_state = curr_state;
	return pressed;
}

static void print_reaction_result(uint32_t us)
{
	char result_str[100];
	snprintf(result_str, sizeof(result_str), "Result = %lu us\r\n", us);
	uart_print(result_str);
}

int main(void)
{
	bool testing = false;
	uint32_t result_us = 0;

	/* Init peripherals */
	led_init();
	btn_init();
	uart_init();

	uart_print("Formula 1 training system ready!\r\n");

	/* Start timer to trigger the stimulus after a delay */
	tim2_start(STIMULUS_DELAY_MS);

	while (1)
	{
		/* Stimulus triggered: LED ON, start measuring reaction time */
		if (tim2_go())
		{
			testing = true;
			led_on();
			tim5_start();
		}

		/* Button pressed: check if valid or too early */
		if (detect_button_press())
		{
			if (testing)
			{
				result_us = tim5_stop();
				tim2_stop();
				led_off();
				print_reaction_result(result_us);
			}
			else
			{
				tim2_stop();
				led_off();
				uart_print("You failed! (False start)\r\n");
			}

			testing = false;

			systick_delay_ms(1000);

			tim2_start(STIMULUS_DELAY_MS);
		}

		/* Timeout: player was too slow */
		if (testing && tim5_timeout())
		{
			tim2_stop();
			tim5_stop();
			led_off();
			testing = false;
			uart_print("You were too late!\r\n");
			tim2_start(STIMULUS_DELAY_MS);
		}
	}
}
