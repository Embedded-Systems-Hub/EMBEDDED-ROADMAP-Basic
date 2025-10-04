#include "stm32f4xx.h"
#include "i2c.h"

void i2c1_init(void)
{
	/* Enable clock access to GPIOB */
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;

	/* Set PB8 and PB9 mode to alternate functions */
	GPIOB->MODER &= ~(1U << 16);
	GPIOB->MODER |= (1U << 17);
	GPIOB->MODER &= ~(1U << 18);
	GPIOB->MODER |= (1U << 19);

	/* Output as open drain */
	GPIOB->OTYPER |= GPIO_OTYPER_OT_8;
	GPIOB->OTYPER |= GPIO_OTYPER_OT_9;

    /* Set output speed: High speed */
	GPIOB->OSPEEDR &= ~(1U << 16);
	GPIOB->OSPEEDR |= (1U << 17);
	GPIOB->OSPEEDR &= ~(1U << 18);
	GPIOB->OSPEEDR |= (1U << 19);

	/* Pull-up */
	GPIOB->PUPDR |= (1U << 16);
	GPIOB->PUPDR &= ~(1U << 17);
	GPIOB->PUPDR |= (1U << 18);
	GPIOB->PUPDR &= ~(1U << 19);

	/* Set alternate function mode to AD04 */
	GPIOB->AFR[1] &= ~(1U << 0);
	GPIOB->AFR[1] &= ~(1U << 1);
	GPIOB->AFR[1] |= (1U << 2);
	GPIOB->AFR[1] &= ~(1U << 3);
	GPIOB->AFR[1] &= ~(1U << 4);
	GPIOB->AFR[1] &= ~(1U << 5);
	GPIOB->AFR[1] |= (1U << 6);
	GPIOB->AFR[1] &= ~(1U << 7);


	/* Enable clock access to I2C1 */
	RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;

	/* Enter reset mode */
	I2C1->CR1 = I2C_CR1_SWRST;

	/* Exit reset mode */
	I2C1->CR1 = 0;

	/* Set Pclk frequency */
	I2C1->CR2 = 42;

	/* Sm mode and 100kHz */
	I2C1->CCR = (42000 / (2 * 100));

	/* Set rise time -> TRISE = (tmax ns / Tpclk) + 1 */
	I2C1->TRISE = 43;

	/* Enable I2C1 */
	I2C1->CR1 |= I2C_CR1_PE;
}

void i2c1_byte_read(uint8_t slave_id, uint8_t memory_addr, uint8_t *data)
{
	volatile int tmp;

	/* Wait until bus not busy */
	while (I2C1->SR2 & I2C_SR2_BUSY);

	/* Generate START */
	I2C1->CR1 |= I2C_CR1_START;

	/* Wait until START flag is set */
	while (!(I2C1->SR1 & I2C_SR1_SB));

	/* Transmit SLAVE ADDRESS + W (0) */
	I2C1->DR = (slave_id << 1);

	/* Wait until ADDR flag is set */
	while (!(I2C1->SR1 & I2C_SR1_ADDR));

	/* Clear ADDR flag */
	tmp = I2C1->SR2;

	/* Send memory address to be read */
	I2C1->DR = memory_addr;

	/* Wait until transmitter empty */
	while (!(I2C1->SR1 & I2C_SR1_TXE));

	/* Generate START */
	I2C1->CR1 |= I2C_CR1_START;

	/* Wait until START flag is set */
	while (!(I2C1->SR1 & I2C_SR1_SB));

	/* Transmit SLAVE ADDRESS + R (1) */
	I2C1->DR = (slave_id << 1) | 1;

	/* Wait until ADDR flag is set */
	while (!(I2C1->SR1 & I2C_SR1_ADDR));

	/* Disable ACK */
	I2C1->CR1 &= ~(I2C_CR1_ACK);

	/* Clear ADDR flag */
	tmp = I2C1->SR2;

	/* Generate STOP after data received */
	I2C1->CR1 |= I2C_CR1_STOP;

	/* Wait until RXNE flag is set */
	while (!(I2C1->SR1 & I2C_SR1_RXNE));

	/* Read data from DR */
	*data = I2C1->DR;

	(void)tmp;
}

void i2c1_burst_read(const uint8_t slave_id, const uint8_t memory_addr, uint32_t n, uint8_t *data)
{
	volatile uint32_t tmp;

	/* Wait until bus not busy */
	while (I2C1->SR2 & I2C_SR2_BUSY);

	/* Generate START */
	I2C1->CR1 |= I2C_CR1_START;

	/* Wait until START flag is set */
	while (!(I2C1->SR1 & I2C_SR1_SB));

	/* Transmit SLAVE ADDRESS + W (0) */
	I2C1->DR = (slave_id << 1);

	/* Wait until ADDR flag is set */
	while (!(I2C1->SR1 & I2C_SR1_ADDR));

	/* Clear ADDR flag */
	tmp = I2C1->SR2;

	/* Wait until transmitter empty */
	while (!(I2C1->SR1 & I2C_SR1_TXE));

	/* Send memory address to be read */
	I2C1->DR = memory_addr;

	/* Wait until transmitter empty */
	while (!(I2C1->SR1 & I2C_SR1_TXE));

	/* Generate START */
	I2C1->CR1 |= I2C_CR1_START;

	/* Wait until START flag is set */
	while (!(I2C1->SR1 & I2C_SR1_SB));

	/* Transmit SLAVE ADDRESS + R */
	I2C1->DR = (slave_id << 1) | 1;

	/* Wait until ADDR flag is set */
	while (!(I2C1->SR1 & I2C_SR1_ADDR));

	/* Clear ADDR flag */
	tmp = I2C1->SR2;

	/* Enable ACK */
	I2C1->CR1 |= (I2C_CR1_ACK);

	while (n > 0U)
	{
		if (n == 1U)
		{
			/* Disable ACK */
			I2C1->CR1 &= ~(I2C_CR1_ACK);

			/* Generate STOP */
			I2C1->CR1 |= I2C_CR1_STOP;

			/* Wait for RXNE flag set */
			while (!(I2C1->SR1 & I2C_SR1_RXNE));

			/* Read data from DR */
			*data = I2C1->DR;
			break;
		}
		else
		{
			/* Wait until RXNE flag is set */
			while (!(I2C1->SR1 & I2C_SR1_RXNE));

			/* Read data from DR */
			*data++ = I2C1->DR;
		}

		/* Update number of remaining bytes */
		n--;
	}

	(void)tmp;
}

void i2c1_burst_write(const uint8_t slave_id, const uint8_t memory_addr, const uint32_t n, const uint8_t *data)
{
	volatile uint32_t tmp;

	/* Wait until bus not busy */
	while (I2C1->SR2 & I2C_SR2_BUSY);

	/* Generate START */
	I2C1->CR1 |= I2C_CR1_START;

	/* Wait until START flag is set */
	while (!(I2C1->SR1 & I2C_SR1_SB));

	/* Transmit SLAVE ADDRESS + W (0) */
	I2C1->DR = (slave_id << 1);

	/* Wait until ADDR flag is set */
	while (!(I2C1->SR1 & I2C_SR1_ADDR));

	/* Clear ADDR flag */
	tmp = I2C1->SR2;

	/* Wait until DR empty */

	/* Wait until transmitter empty */
	while (!(I2C1->SR1 & I2C_SR1_TXE));

	/* Send memory address to be written */
	I2C1->DR = memory_addr;

	for (uint32_t i = 0; i < n; i++) {
		/* Wait until transmitter empty */
		while (!(I2C1->SR1 & I2C_SR1_TXE));

		/* Transmit data */
		I2C1->DR = *data++;
	}

	/* Wait until transfer finished */
	while (!(I2C1->SR1 & I2C_SR1_BTF));

	/* Generate STOP */
	I2C1->CR1 |= I2C_CR1_STOP;

	(void)tmp;

}
