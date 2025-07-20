#include <stdio.h>
#include <string.h>
#include "gpio.h"
#include "uart.h"
#include "tim.h"
#include "systick.h"

volatile bool button_debouncing = false;

int main(void)
{
	uart_init();
	uart_print("Medical Device Menu Navigation ready!\r\n");

	btn_init();
	btn_irq_init();

	while(1)
	{
		systick_delay_ms(2000); // Running other jobs
	}
}

static void exti13_callback(void)
{
	uart_print("Button pressed!\r\n");
}

void EXTI15_10_IRQHandler(void)
{
    EXTI->PR |= (1U << 13); // Clear flag

	if (button_debouncing) return;

	/* Start debounce logic */
	button_debouncing = true;
	tim2_start(500);
	exti13_callback();
}

void TIM2_IRQHandler(void)
{
    if (TIM2->SR & TIM_SR_UIF)
    {
    	tim2_stop();
        button_debouncing = false; // Allow next button press
    }
}

