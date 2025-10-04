#ifndef BMP280_H_
#define BMP280_H_

#include <stdint.h>

#define BMP280_DEVICE_ADDR		0x77

typedef struct {
    uint16_t dig_T1;
    int16_t  dig_T2, dig_T3;
    uint16_t dig_P1;
    int16_t  dig_P2, dig_P3, dig_P4, dig_P5, dig_P6, dig_P7, dig_P8, dig_P9;
} bmp280_cal_t;

void bmp280_read_address(const uint8_t reg, uint8_t *data);
void bmp280_write(const uint8_t reg, uint8_t *data);
void bmp280_read(const uint8_t reg, const uint32_t n, uint8_t *data);
void bmp280_init(void);

#endif /* BMP280_H_ */
