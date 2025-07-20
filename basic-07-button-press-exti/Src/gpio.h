#ifndef GPIO_H_
#define GPIO_H_

#include <stdbool.h>
#include "stm32f4xx.h"

void led_init(void);
void led_on(void);
void led_off(void);
void btn_init(void);
bool btn_get_state(void);
void btn_irq_init(void);
bool btn_pressed(void);

#endif /* GPIO_H_ */
