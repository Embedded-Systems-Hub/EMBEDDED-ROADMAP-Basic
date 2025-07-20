#include "stm32f4xx.h"
#include "tim.h"

#define RCC_TIM2EN		(1U << 0)
#define RCC_TIM5EN		(1U << 3)
#define TIM2_CR1DIR		(1U << 4)
#define TIM2_CR1CEN		(1U << 0)
#define TIM5_CR1CEN		(1U << 0)

#define CR1_CEN (1U<<0)

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
	TIM2->CR1 = CR1_CEN;
}


void tim2_stop(void)
{
	/* Disable timer */
	TIM2->CR1 &= ~TIM2_CR1CEN;

	/* Clear flags */
	TIM2->SR &= ~TIM_SR_UIF;
}

bool tim2_go(void)
{
	bool ret = false;
	if (TIM2->SR & (1U << 0))
	{
		TIM2->SR &= ~(1U << 0);
		ret = true;
	}
	return ret;
}

void tim5_start(void)
{
	RCC->APB1ENR |= RCC_APB1ENR_TIM5EN;
	TIM5->CR1 &= ~TIM_CR1_CEN;
	TIM5->PSC = 16 - 1;
	TIM5->ARR = 1000000 - 1;
	TIM5->CR1 &= ~TIM_CR1_DIR;
	TIM5->CNT = 0;
	TIM5->SR  &= ~TIM_SR_UIF;
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
