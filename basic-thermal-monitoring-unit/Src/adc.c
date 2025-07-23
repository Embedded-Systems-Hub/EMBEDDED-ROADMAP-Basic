#include "adc.h"

// ADC1 ch1 -> PA1
void adc1_ch1_init(void)
{
	/* Enable clock access to GPIOA */
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;

	/* Set PA1 mode to analog */
	GPIOA->MODER |= GPIO_MODER_MODER1;

	/* Enable clock access to ADC */
	RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;

	/* Configure sequence */
	ADC1->SQR3 = ADC_SQR3_SQ1_0;
	ADC1->SQR1 = 0;

	/* Enable ADC */
	ADC1->CR2 |= ADC_CR2_ADON;
}

void adc1_ch1_single_conversion(void)
{
	/* Start ADC conversion */
	ADC1->CR2 |= ADC_CR2_SWSTART;
}

void adc1_ch1_start_continuous_conversion(void)
{
	/* Enable continuous conversion */
	ADC1->CR2 |= ADC_CR2_CONT;

	/* Start ADC conversion */
	ADC1->CR2 |= ADC_CR2_SWSTART;
}

uint32_t adc1_ch1_read(void)
{
	/* Wait for conversion to be complete */
	while(!(ADC1->SR & (ADC_SR_EOC)));

	/* Read converted value */
	return (ADC1->DR);
}

// ADC1 ch16 -> temperature sensor
void temp_sensor_init(void)
{
    /* Enable ADC1 clock */
    RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;

    /* Enable temperature sensor and VREFINT */
    ADC->CCR |= ADC_CCR_TSVREFE;

    /* Configure ADC channel 16 (temperature sensor) */
    ADC1->SQR3 = 16;				// Channel 16
    ADC1->SMPR1 |= ADC_SMPR1_SMP16;	// Max sample time for stable reading

    /* Enable ADC1 */
    ADC1->CR2 |= ADC_CR2_ADON;

    /* Delay for temperature sensor stabilization (>10us) */
    for (volatile int i = 0; i < 10000; i++);
}

void temp_sensor_read(uint16_t *raw_adc)
{
	/* Start conversion */
	ADC1->CR2 |= ADC_CR2_SWSTART;

	/* Wait for conversion to complete */
	while (!(ADC1->SR & ADC_SR_EOC));

	/* Read ADC value */
	*raw_adc = ADC1->DR;
}
