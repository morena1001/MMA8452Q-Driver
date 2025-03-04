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

// FUNCTION PROTOTYPES
float process_data (MMA8452Q_t *device, uint16_t raw);

/*
 * INITIALIZATION FUNCTION
 */

uint8_t MMA8452Q_Init (MMA8452Q_t *device, I2C_HandleTypeDef *i2c_handle) {
	// Set struct parameters
	device->i2c_handle = i2c_handle;
	device->f_read_set = false;
	device->range = RANGE_2;

	device->acc_raw[0] = 0;
	device->acc_raw[1] = 0;
	device->acc_raw[2] = 0;

	device->acc_g[0] = 0.0f;
	device->acc_g[1] = 0.0f;
	device->acc_g[2] = 0.0f;

	device->lapo = PORTRAIT_UP;
	device->bafro = FRONT;


	// Store number of transaction errors ( to be returned at the end of the function )
	uint8_t err_num = 0;
	HAL_StatusTypeDef status;

	// Check WHO_AM_I register (p.23)
	uint8_t reg_data;

	status = MMA8452Q_Byte_Read(device, MMA8452Q_REG_WHO_AM_I, &reg_data);
	err_num += (status != HAL_OK);
	if (reg_data != MMA8452Q_ID)	return 255;

	// XYZ_DATA_CFG (p.23)
	reg_data = MMA8452Q_DEFAULT_XYZ_DATA_CFG;
	status = MMA8452Q_Byte_Write(device, MMA8452Q_REG_XYZ_DATA_CFG, &reg_data);
	err_num += (status != HAL_OK);

	// Set the full scale range to correctly process acceleration data
	device->range = reg_data & 3;

	// PL_CFG (p.25)
	reg_data = MMA8452Q_DEFAULT_PL_CFG;
	status = MMA8452Q_Byte_Write(device, MMA8452Q_REG_PL_CFG, &reg_data);
	err_num += (status != HAL_OK);

	// PL_COUNT (p.26)
	reg_data = MMA8452Q_DEFAULT_PL_COUNT;
	status = MMA8452Q_Byte_Write(device, MMA8452Q_REG_PL_COUNT, &reg_data);
	err_num += (status != HAL_OK);

	// ASLP_COUNT (p.39)
	reg_data = MMA8452Q_DEFAULT_ASLP_COUNT;
	status = MMA8452Q_Byte_Write(device, MMA8452Q_REG_ASLP_COUNT, &reg_data);
	err_num += (status != HAL_OK);

	// CTRL_REG1 (p.40)
	reg_data = MMA8452Q_DEFAULT_CTRL_REG1;
	status = MMA8452Q_Byte_Write(device, MMA8452Q_REG_CTRL_REG1, &reg_data);
	err_num += (status != HAL_OK);

	// Set f_read_set to correctly read acceleration data
	device->f_read_set = (reg_data >> 1) & 1;

	// CTRL_REG2 (p.41)
	reg_data = MMA8452Q_DEFAULT_CTRL_REG2;
	status = MMA8452Q_Byte_Write(device, MMA8452Q_REG_CTRL_REG2, &reg_data);
	err_num += (status != HAL_OK);

	// CTRL_REG3 (p.42)
	reg_data = MMA8452Q_DEFAULT_CTRL_REG3;
	status = MMA8452Q_Byte_Write(device, MMA8452Q_REG_CTRL_REG3, &reg_data);
	err_num += (status != HAL_OK);

	// CTRL_REG4 (p.42)
	reg_data = MMA8452Q_DEFAULT_CTRL_REG4;
	status = MMA8452Q_Byte_Write(device, MMA8452Q_REG_CTRL_REG4, &reg_data);
	err_num += (status != HAL_OK);

	// CTRL_REG5 (p.43)
	reg_data = MMA8452Q_DEFAULT_CTRL_REG5;
	status = MMA8452Q_Byte_Write(device, MMA8452Q_REG_CTRL_REG5, &reg_data);
	err_num += (status != HAL_OK);

	return err_num;
}

/*
 * CONFIGURATION FUNCTIONS
 */



/*
 * DATA AQUISITION
 */

HAL_StatusTypeDef MMA8452Q_ReadAccel (MMA8452Q_t *device) {
	uint8_t raw_data[6] = {0};

	HAL_StatusTypeDef status = MMA8452Q_MultiByte_Read (device, MMA8452Q_REG_OUT_X_MSB, raw_data, (device->f_read_set ? 3 : 6));
	if (status != HAL_OK)	return status;

	device->acc_raw[0] = (raw_data[0] << 4) | raw_data[1];
	device->acc_raw[1] = (raw_data[2] << 4) | raw_data[3];
	device->acc_raw[2] = (raw_data[4] << 4) | raw_data[5];

	device->acc_g[0] = process_data(device, device->acc_raw[0]);
	device->acc_g[1] = process_data(device, device->acc_raw[1]);
	device->acc_g[2] = process_data(device, device->acc_raw[2]);

	return HAL_OK;
}

HAL_StatusTypeDef MMA8452Q_ReadOrientation (MMA8452Q_t *device) {
	uint8_t raw_data = 0;

	HAL_StatusTypeDef status = MMA8452Q_Byte_Read (device, MMA8452Q_REG_PL_STATUS, &raw_data);
	if (status != HAL_OK)	return status;

	device->lapo = (raw_data >> 1) & 3;
	device->bafro = raw_data & 1;

	return HAL_OK;
}

/*
 * LOW LEVEL FUNCTIONS
 */

HAL_StatusTypeDef MMA8452Q_Byte_Read (MMA8452Q_t *device, uint8_t reg, uint8_t *data) {
	return HAL_I2C_Mem_Read (device->i2c_handle, MMA8452Q_ADDR, reg, I2C_MEMADD_SIZE_8BIT, data, 1, HAL_MAX_DELAY);
}

HAL_StatusTypeDef MMA8452Q_MultiByte_Read (MMA8452Q_t *device, uint8_t reg, uint8_t *data, uint8_t length) {
	return HAL_I2C_Mem_Read (device->i2c_handle, MMA8452Q_ADDR, reg, I2C_MEMADD_SIZE_8BIT, data, length, HAL_MAX_DELAY);
}

HAL_StatusTypeDef MMA8452Q_Byte_Write (MMA8452Q_t *device, uint8_t reg, uint8_t *data) {
	return HAL_I2C_Mem_Write (device->i2c_handle, MMA8452Q_ADDR, reg, I2C_MEMADD_SIZE_8BIT, data, 1, HAL_MAX_DELAY);
}

HAL_StatusTypeDef MMA8452Q_MultiByte_Write (MMA8452Q_t *device, uint8_t reg, uint8_t *data, uint8_t length) {
	return HAL_I2C_Mem_Write (device->i2c_handle, MMA8452Q_ADDR, reg, I2C_MEMADD_SIZE_8BIT, data, length, HAL_MAX_DELAY);
}

/*
 * PRIVATE FUNCTIONS
 */

float process_data (MMA8452Q_t *device, uint16_t raw) {
	float scale = 0.0f;

	if (!device->f_read_set) {
		if (device->range == RANGE_2) 			scale = 0.001f;
		else if (device->range == RANGE_4) 		scale = 0.002f;
		else if (device->range == RANGE_8) 		scale = 0.0039f;
	} else {
		if (device->range == RANGE_2) 			scale = 0.0156f;
		else if (device->range == RANGE_4) 		scale = 0.03125f;
		else if (device->range == RANGE_8)		scale = 0.0625f;
	}

	if (raw >> 11)	scale *= -1;

	return (float) (raw) * scale;
}
