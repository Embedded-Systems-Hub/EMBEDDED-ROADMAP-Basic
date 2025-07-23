#include "gpio.h"
#include "systick.h"

#define NUM_MODES 3
const uint32_t blink_durations[NUM_MODES] = {500, 250, 125}; // ms

/* Toggle LED state and wait for the specified duration */
static void blink_led(uint32_t duration_ms)
{
	static bool led_state = false;

	if (led_state)
	{
		led_off();
		led_state = false;
	}
	else
	{
		led_on();
		led_state = true;
	}

	systick_delay_ms(duration_ms);
}

/* Detect rising edge of button press with simple debounce */
static bool detect_button_press(void)
{
	static bool prev_state = false;
	bool curr_state = btn_get_state();
	bool pressed = false;

	if (!prev_state && curr_state)
	{
		pressed = true;
		systick_delay_ms(20);
	}
	prev_state = curr_state;
	return pressed;
}

int main(void)
{
	int led_mode = 0;

	/* Initialize LED and button GPIOs */
	led_init();
	btn_init();

	while (1)
	{
		/* Change LED mode on button press */
		if (detect_button_press())
		{
			led_mode = (led_mode + 1) % NUM_MODES;
		}

		/* Blink LED with duration based on current mode */
		blink_led(blink_durations[led_mode]);
	}
}
