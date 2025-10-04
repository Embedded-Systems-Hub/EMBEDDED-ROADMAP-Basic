#ifndef I2C_H_
#define I2C_H_

#include <stdint.h>

#include "stm32f4xx.h"

void i2c1_init(void);
void i2c1_byte_read(const uint8_t slave_id, const uint8_t memory_addr, uint8_t* data);
void i2c1_burst_read(const uint8_t slave_id, const uint8_t memory_addr, uint32_t n, uint8_t* data);
void i2c1_burst_write(const uint8_t slave_id, const uint8_t memory_addr, const uint32_t n, const uint8_t* data);

#endif /* I2C_H_ */
