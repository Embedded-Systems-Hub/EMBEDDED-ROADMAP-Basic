#include "adc.h"

// ADC1 ch1 -> PA1
void adc1_ch1_init(void)
{
	/* Enable clock access to GPIOA */
	RCC->AHB1ENR |= (1U << 0);

	/* Set PA1 mode to analog */
	GPIOA->MODER |= (1U << 2);
	GPIOA->MODER |= (1U << 3);

	/* Enable clock access to ADC */
	RCC->APB2ENR |= (1U << 8);

	/* Configure sequence */
	ADC1->SQR3 = (1U << 0);
	ADC1->SQR1 = 0U;

	/* Enable ADC */
	ADC1->CR2 |= (1U << 0);
}

void adc1_ch1_start_conversion(void)
{
	/* Enable continuous conversion */
	ADC1->CR2 |= (1U << 1);

	/* Start ADC conversion */
	ADC1->CR2 |= (1U << 30);
}

uint32_t adc_read(void)
{
	/* Wait for conversion to be complete */
	while(!(ADC1->SR & (1U << 1)));

	/* Read converted value */
	return (ADC1->DR);
}
