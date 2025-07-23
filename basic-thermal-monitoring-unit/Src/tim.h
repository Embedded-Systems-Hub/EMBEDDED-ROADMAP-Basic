#ifndef TIM_H_
#define TIM_H_

#include <stdbool.h>
#include <stdint.h>

void tim2_start(uint32_t reload_value_ms);
void tim2_stop(void);
bool tim2_go(void);
void tim3_start(uint32_t reload_value_ms);
void tim3_stop(void);
void tim5_start(uint32_t reload_value_ms);
bool tim5_timeout(void);
uint32_t tim5_get_now(void);
uint32_t tim5_stop(void);

#endif /* TIM_H_ */
