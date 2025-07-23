#include "systick.h"
#include "stm32f4xx.h"

/* Arm Cortex-M4 User Guide */
#define CTRL_ENABLE			(1U << 0)
#define CTRL_CLCKSRC		(1U << 2)
#define CTRL_COUNTFLAG		(1U << 16)

/* On reset the 16 MHz internal RC oscillator is selected as the default CPU clock */
#define TICKS_PER_MS		16000

void systick_delay_ms(uint32_t delay_ms)
{
	/* Load ticks per ms */
	SysTick->LOAD = TICKS_PER_MS - 1;

	/* Select clock source */
	SysTick->CTRL = CTRL_CLCKSRC;

	/* Reset systick current value */
	SysTick->VAL = 0;

	/* Enable SysTick */
	SysTick->CTRL |= CTRL_ENABLE;

	for (int i = 0; i < delay_ms; i++)
	{
		while((SysTick->CTRL & CTRL_COUNTFLAG) == 0);
	}

	/* Disable SysTick */
	SysTick->CTRL = 0;
}
