#include <stdio.h>
#include "gpio.h"
#include "uart.h"
#include "tim.h"
#include "adc.h"

#define TEMP_WARNING_THRESHOLD_C	2600
#define TEMP_ERROR_THRESHOLD_C		2700
#define LOG_INTERVAL_MS				5000
#define LED_BLINK_WARN_MS			500
#define LED_BLINK_ERROR_MS			200

typedef enum {
	TEMP_ALARM_INFO,
	TEMP_ALARM_WARNING,
	TEMP_ALARM_ERROR,
} temp_alarm_level_t;

volatile bool debounce_in_progress = false; // Set to true when a button press is being debounced
volatile bool trigger_log = false; // Set to true when a temperature log should be performed
static bool led_warn_active = false; // Indicates that the warning LED blink pattern is active
static bool led_error_active = false; // Indicates that the error LED blink pattern is active

// Converts raw ADC value to temperature in centi-degrees Celsius (°C * 100)
int16_t adc_to_centi_celsius(uint16_t raw_adc)
{
    /* === Integer math ===
     * V_SENSE = (raw * VREF) / 4095
     * VREF = 3300 mV */

	uint32_t vsense_mv = (raw_adc * 3300UL) / 4095UL;

	const int32_t V25_MV = 760;
	const int32_t AVG_SLOPE_UV = 2500;

	int32_t delta = ((int32_t)vsense_mv - V25_MV) * 100000 / AVG_SLOPE_UV;
	return 2500 + delta;
}

// Sends a formatted temperature log over UART
void send_temp_log(int16_t temp_c, temp_alarm_level_t level)
{
	const char *level_str = "";
	if (level == TEMP_ALARM_WARNING)
	{
		level_str = "[WARNING]";
	}
	else if (level == TEMP_ALARM_ERROR)
	{
		level_str = "[ERROR]";
	}
	else
	{
		level_str = "[INFO]";
	}

	char msg[64];
	snprintf(msg, sizeof(msg), "%s CPU Temperature = %d.%02d °C\r\n",
	         level_str, temp_c / 100, temp_c % 100);

	uart_print(msg);
}

// Evaluates temperature and triggers appropriate actions (log, LED, timers)
void evaluate_temperature(int16_t temp)
{
	if (temp < TEMP_WARNING_THRESHOLD_C) {
		send_temp_log(temp, TEMP_ALARM_INFO);
		led_warn_active = false;
		led_error_active = false;
	}
	else if (temp < TEMP_ERROR_THRESHOLD_C) {
		send_temp_log(temp, TEMP_ALARM_WARNING);
		if (!led_warn_active) {
			tim3_start(LED_BLINK_WARN_MS);
			led_warn_active = true;
			led_error_active = false;
		}
	}
	else {
		send_temp_log(temp, TEMP_ALARM_ERROR);
		if (!led_error_active) {
			tim3_start(LED_BLINK_ERROR_MS);
			led_warn_active = false;
			led_error_active = true;
		}
	}
}

// Called when a new log is triggered (periodic or manual)
void handle_log_task(void)
{
	uint16_t raw_adc;
	temp_sensor_read(&raw_adc);

	int16_t temp_now = adc_to_centi_celsius(raw_adc);
	evaluate_temperature(temp_now);

	trigger_log = false;
	tim5_start(LOG_INTERVAL_MS);
}

int main(void)
{
	uart_init();
	led_init();
	btn_init();
	btn_irq_init();
	temp_sensor_init();

	uart_print("Thermal Monitoring Unit ready!\r\n");

	tim5_start(LOG_INTERVAL_MS);

	while (1)
	{
		if (trigger_log)
		{
			handle_log_task();
		}
	}
}

// Called when button (PC13) is pressed
void EXTI15_10_IRQHandler(void)
{
	EXTI->PR |= (1U << 13); // Clear EXTI line 13 flag (User Button)

	if (debounce_in_progress) return;

	debounce_in_progress = true;
	tim2_start(500); // Start debounce timer
	trigger_log = true;
}

// Called when TIM2 expires
void TIM2_IRQHandler(void)
{
	if (TIM2->SR & TIM_SR_UIF)
	{
		tim2_stop();
		debounce_in_progress = false;
	}
}

// Called when TIM3 expires
void TIM3_IRQHandler(void)
{
	if (TIM3->SR & TIM_SR_UIF)
	{
		tim3_stop();
		led_toggle();

		if (led_warn_active)
		{
			tim3_start(LED_BLINK_WARN_MS);
		}
		else if (led_error_active)
		{
			tim3_start(LED_BLINK_ERROR_MS);
		}
		else
		{
			led_off();
		}
	}
}

// Called when TIM5 expires
void TIM5_IRQHandler(void)
{
	if (TIM5->SR & TIM_SR_UIF)
	{
		tim5_stop();
		trigger_log = true; // Periodic temperature log
	}
}
