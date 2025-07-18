#ifndef ADC_H_
#define ADC_H_

#include <stdint.h>
#include "stm32f4xx.h"

void adc1_ch1_init(void);
void adc1_ch1_start_conversion(void);
uint32_t adc_read(void);

#endif /* ADC_H_ */
