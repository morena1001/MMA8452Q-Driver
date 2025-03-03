/*
 * MMA8452Q.c
 *
 * Created on : November 7, 2024
 * Author : Josue Flores
 *
 * Last Modified : November 7, 2024
 */

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include "MMA8452Q.h"

// PRIVATE VARIABLES
uint8_t raw_accel_data [6];

/*
 * INITIALIZATION FUNCTION
 */

uint8_t MMA8452Q_Init (MMA8452Q_t* device, I2C_HandleTypeDef* i2c_handle) {
	return 1;
}

/*
 * LOW LEVEL FUNCTIONS
 */

HAL_StatusTypeDef MMA8452Q_Byte_Read (MMA8452Q_t* device, uint8_t reg, uint8_t* data) {
	HAL_StatusTypeDef status = HAL_I2C_Master_Transmit (device->i2c_handle, MMA8452Q_ADDR, &reg, 1, HAL_MAX_DELAY);
	if (status != HAL_OK)	return status;

	return HAL_I2C_Master_Receive (device->i2c_handle, MMA8452Q_ADDR | 1, data, 1, HAL_MAX_DELAY);
}

HAL_StatusTypeDef MMA8452Q_MultiByte_Read (MMA8452Q_t* device, uint8_t reg, uint8_t* data, uint8_t length) {
	HAL_StatusTypeDef status = HAL_I2C_Master_Transmit (device->i2c_handle, MMA8452Q_ADDR, &reg, 1, HAL_MAX_DELAY);
	if (status != HAL_OK)	return status;

	return HAL_I2C_Master_Receive (device->i2c_handle, MMA8452Q_ADDR | 1, data, length, HAL_MAX_DELAY);
}

HAL_StatusTypeDef MMA8452Q_Byte_Write (MMA8452Q_t* device, uint8_t reg, uint8_t* data) {
	return HAL_I2C_Mem_Write (device->i2c_handle, MMA8452Q_ADDR, reg, I2C_MEMADD_SIZE_8BIT, data, 1, HAL_MAX_DELAY);
}

HAL_StatusTypeDef MMA8452Q_MultiByte_Write (MMA8452Q_t* device, uint8_t reg, uint8_t* data, uint8_t length) {
	return HAL_I2C_Mem_Write (device->i2c_handle, MMA8452Q_ADDR, reg, I2C_MEMADD_SIZE_8BIT, data, length, HAL_MAX_DELAY);
}
