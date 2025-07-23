#include "stm32f4xx.h"
#include "tim.h"

void tim2_start(uint32_t reload_value_ms)
{
    /* Enable clock access to tim2 */
	RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;

	/* Set prescaler value */
	TIM2->PSC = 16000 - 1;

	/* Set auto-reload value */
	TIM2->ARR = reload_value_ms - 1;

	/* Clear counter */
	TIM2->CNT = 0;

	/* Enable update interrupt */
    TIM2->DIER |= TIM_DIER_UIE;

    /* Enable TIM2 interrupt */
    NVIC_EnableIRQ(TIM2_IRQn);

	/* Enable timer */
	TIM2->CR1 = TIM_CR1_CEN;
}

void tim2_stop(void)
{
	/* Disable timer */
	TIM2->CR1 &= ~TIM_CR1_CEN;

	/* Clear flags */
	TIM2->SR &= ~TIM_SR_UIF;
}

bool tim2_go(void)
{
	bool ret = false;
	if (TIM2->SR & TIM_SR_UIF)
	{
		TIM2->SR &= ~TIM_SR_UIF;
		ret = true;
	}
	return ret;
}

void tim3_start(uint32_t reload_value_ms)
{
    /* Enable clock access to tim2 */
	RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;

	/* Set prescaler value */
	TIM3->PSC = 16000 - 1;

	/* Set auto-reload value */
	TIM3->ARR = reload_value_ms - 1;

	/* Clear counter */
	TIM3->CNT = 0;

	/* Enable update interrupt */
    TIM3->DIER |= TIM_DIER_UIE;

    /* Enable TIM2 interrupt */
    NVIC_EnableIRQ(TIM3_IRQn);

	/* Enable timer */
	TIM3->CR1 = TIM_CR1_CEN;
}

void tim3_stop(void)
{
	/* Disable timer */
	TIM3->CR1 &= ~TIM_CR1_CEN;

	/* Clear flags */
	TIM3->SR &= ~TIM_SR_UIF;
}

void tim5_start(uint32_t reload_value_ms)
{
    /* Enable clock access to tim2 */
	RCC->APB1ENR |= RCC_APB1ENR_TIM5EN;

	/* Set prescaler value */
	TIM5->PSC = 16000 - 1;

	/* Set auto-reload value */
	TIM5->ARR = reload_value_ms - 1;

	/* Clear counter */
	TIM5->CNT = 0;

	/* Enable update interrupt */
    TIM5->DIER |= TIM_DIER_UIE;

    /* Enable TIM2 interrupt */
    NVIC_EnableIRQ(TIM5_IRQn);

	/* Enable timer */
	TIM5->CR1 |= TIM_CR1_CEN;
}


bool tim5_timeout(void)
{
    if (TIM5->SR & TIM_SR_UIF) {
        TIM5->SR &= ~TIM_SR_UIF; // Clear the update interrupt flag
        return true;
    }
    return false;
}


uint32_t tim5_stop(void)
{
    uint32_t count = TIM5->CNT;
    TIM5->CR1 &= ~TIM_CR1_CEN;
    TIM5->SR  &= ~TIM_SR_UIF;
    TIM5->CNT = 0;
    return count;
}
