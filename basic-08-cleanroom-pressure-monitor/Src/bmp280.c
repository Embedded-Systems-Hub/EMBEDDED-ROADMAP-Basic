#include "bmp280.h"
#include "i2c.h"

void bmp280_read_address(const uint8_t reg, uint8_t *data)
{
	i2c1_byte_read(BMP280_DEVICE_ADDR, reg, data);
}

void bmp280_write(const uint8_t reg, uint8_t *data)
{
	i2c1_burst_write(BMP280_DEVICE_ADDR, reg, 1, data);
}

void bmp280_read(const uint8_t reg, const uint32_t n, uint8_t *data)
{
	i2c1_burst_read(BMP280_DEVICE_ADDR, reg, n, data);
}

void bmp280_init(void)
{
	/* Enable I2C */
	i2c1_init();

	/* Reset sensor */
	uint8_t reset_value = 0xB6;
	bmp280_write(0xE0, &reset_value);
}
