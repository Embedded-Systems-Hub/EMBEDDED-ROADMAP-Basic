#ifndef ADC_H_
#define ADC_H_

#include <stdint.h>
#include "stm32f4xx.h"

void adc1_ch1_init(void);
void adc1_ch1_single_conversion(void);
void adc1_ch1_start_continuous_conversion(void);
uint32_t adc_read(void);

void temp_sensor_init(void);
void temp_sensor_read(uint16_t *raw_adc);

#endif /* ADC_H_ */
