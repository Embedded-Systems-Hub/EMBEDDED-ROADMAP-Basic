#include "gpio.h"

/* LED - PA5 */
#define GPIOAEN		(1U << 0)
#define LED_BS5		(1U << 5)
#define LED_BR5		(1U << 21)
#define LED_ODR5	(1U << 5)

/* BUTTON - PC13 */
#define GPIOCEN		(1U << 2)
#define BTN_PIN		(1U << 13)

static volatile bool button_pressed = false;

void led_init(void)
{
	/* Enable clock access to GPIOA */
	RCC->AHB1ENR |= GPIOAEN;
	/* Set PA5 mode to output mode */
	GPIOA->MODER |= (1U << 10);
	GPIOA->MODER &= ~(1U << 11);
}

void led_on(void)
{
	/* Set PA5 */
	GPIOA->BSRR |= LED_BS5;
}

void led_off(void)
{
	/* Reset PA5 */
	GPIOA->BSRR |= LED_BR5;
}

void led_toggle(void)
{
	/* Toggle PA5 */
	GPIOA->ODR ^= LED_ODR5;
}

void btn_init(void)
{
	/* Enable clock access to GPIOC */
	RCC->AHB1ENR |= GPIOCEN;

	/* Set PC13 mode to output mode */
	GPIOC->MODER &= ~(1U << 26);
	GPIOC->MODER &= ~(1U << 27);
}

bool btn_get_state(void)
{
	 /* Note : BTN is active low */
	 if(GPIOC->IDR & BTN_PIN)
	 {
		 return false;
	 }
	 else
	 {
		 return true;
	 }
}

void btn_irq_init(void)
{
	/* Disable global interrupts */
	__disable_irq();

    /* Enable SYSCFG clock (needed for EXTI) */
    RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;

    /* Connect PC13 to EXTI13 */
    SYSCFG->EXTICR[3] |= (1U << 5); // 0b0010: PC[x]

    /* Configure EXTI0 line: falling edge, unmask interrupt */
    EXTI->IMR  |= (1 << 13);   // Unmask EXTI13
    EXTI->FTSR |= (1 << 13);   // Trigger on falling edge

    /* Enable EXTI0 interrupt in NVIC */
    NVIC_EnableIRQ(EXTI15_10_IRQn);

	/* Enable global interrupts */
    __enable_irq();
}

bool btn_pressed(void)
{
    if (button_pressed)
    {
        button_pressed = false;
        return true;
    }
    return false;
}
