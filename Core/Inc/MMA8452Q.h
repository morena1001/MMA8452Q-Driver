/*
 * MMA8452Q.h
 *
 * Created on : November 7, 2024
 * Author : Josue Flores
 *
 * Last Modified : November 7, 2024
 */

#ifndef INC_MMA8452Q_H_
#define INC_MMA8452Q_H_

#include "stm32f3xx_hal.h" // NEEDED FOR I2C
#include "stdbool.h"

/*
 * DEFINES
 */

#define MMA8452Q_ADDR		(0x1D << 1)
#define MMA8452Q_ID			0x2A

/*
 * REGISTERS p.19
 */

#define MMA8452Q_REG_STATUS					0x00
#define MMA8452Q_REG_OUT_X_MSB				0x01
#define MMA8452Q_REG_OUT_X_LSB				0x02
#define MMA8452Q_REG_OUT_Y_MSB				0x03
#define MMA8452Q_REG_OUT_Y_LSB				0x04
#define MMA8452Q_REG_OUT_Z_MSB				0x05
#define MMA8452Q_REG_OUT_Z_LSB				0x06
#define MMA8452Q_REG_SYSMOD					0x0B
#define MMA8452Q_REG_INT_SOURCE				0x0C
#define MMA8452Q_REG_WHO_AM_I				0x0D
#define MMA8452Q_REG_XYZ_DATA_CFG			0x0E
#define MMA8452Q_REG_HP_FILTER_CUTOFF		0x0F
#define MMA8452Q_REG_PL_STATUS				0x10
#define MMA8452Q_REG_PL_CFG					0x11
#define MMA8452Q_REG_PL_COUNT				0x12
#define MMA8452Q_REG_PL_BF_ZCOMP			0x13
#define MMA8452Q_REG_P_L_THS_REG			0x14
#define MMA8452Q_REG_FF_MT_CFG				0x15
#define MMA8452Q_REG_FF_MT_SRC				0x16
#define MMA8452Q_REG_FF_MT_THS				0x17
#define MMA8452Q_REG_FF_MT_COUNT			0x18
#define MMA8452Q_REG_TRANSIENT_CFG			0x1D
#define MMA8452Q_REG_TRANSIENT_SRC			0x1E
#define MMA8452Q_REG_TRANSIENT_THS			0x1F
#define MMA8452Q_REG_TRANSIENT_COUNT		0x20
#define MMA8452Q_REG_PULSE_CFG				0x21
#define MMA8452Q_REG_PULSE_SRC				0x22
#define MMA8452Q_REG_PULSE_THSX				0x23
#define MMA8452Q_REG_PULSE_THSY				0x24
#define MMA8452Q_REG_PULSE_THSZ				0x25
#define MMA8452Q_REG_PULSE_TMLT				0x26
#define MMA8452Q_REG_PULSE_LTCY				0x27
#define MMA8452Q_REG_PULSE_WIND				0x28
#define MMA8452Q_REG_ASLP_COUNT				0x29
#define MMA8452Q_REG_CTRL_REG1				0x2A
#define MMA8452Q_REG_CTRL_REG2				0x2B
#define MMA8452Q_REG_CTRL_REG3				0x2C
#define MMA8452Q_REG_CTRL_REG4				0x2D
#define MMA8452Q_REG_CTRL_REG5				0x2E
#define MMA8452Q_REG_OFF_X					0x2F
#define MMA8452Q_REG_OFF_Y					0x30
#define MMA8452Q_REG_OFF_Z					0x31

/*
 * REGISTER CONFIGURATION SHORTCUTS
 */

#define MMA8552Q_G_TO_BIT(val)				((val >> 2) & 2)


/*
 * DEFAULT VALUES
 */

#define MMA8452Q_DEFAULT_XYZ_DATA_CFG		0x02 // No high pass filter, and range of 8
#define MMA8452Q_DEFAULT_PL_CFG				0xC0 // Debounce cleared when condition is no longer valid, and portrait/landscape detection is enabled
#define MMA8452Q_DEFAULT_PL_COUNT			0x00 // Count for debounce is 0
#define MMA8452Q_DEFAULT_ASLP_COUNT			0x0F // Minimum period of inactivity is 15
#define MMA8452Q_DEFAULT_CTRL_REG1			0xD9 // ASLP_RATE is 1.56Hz, ODR is 100 Hz, No reduced noise mode or fast-read mode, active mode
#define MMA8452Q_DEFAULT_CTRL_REG2			0x04 // Self-test disabled, Software reset disable, sleep and active mode power schemes are normal, Auto-sleep is enable
#define MMA8452Q_DEFAULT_CTRL_REG3			0x22 // Transient, pulse, and freefall/motion functions are bypassed in sleep mode, orientation function interrupt can wake up system, and interrupt polarityt is active high, and interrupt is push-pull
#define MMA8452Q_DEFAULT_CTRL_REG4			0x81 // Auto-sleep/wake interrupt is enabled, transient, pulse, freefall/motion, and data-ready interrupts are disabled, and orientation interrupt is enabled
#define MMA8452Q_DEFAULT_CTRL_REG5			0x10 // ASLP, transient, pulse, freefall/motion, and data ready interrupts are routed to INT2 pin, orientation interrupt ins routed to INT1 pin

/*
 * SENSOR STRUCT AND ENUM
 */

enum FULL_SCALE_RANGE {RANGE_2, RANGE_4, RANGE_8};

enum LAPO_Orientation {PORTRAIT_UP, PORTRAIT_DOWN, LANDSCAPE_RIGHT, LANDSCAPE_LEFT};
enum BAFRO_Orientation {FRONT, BACK};

typedef struct {
	I2C_HandleTypeDef *i2c_handle;
	bool f_read_set;
	enum FULL_SCALE_RANGE range;

	uint16_t acc_raw[3];
	float acc_g[3];
	enum LAPO_Orientation lapo;
	enum BAFRO_Orientation bafro;

} MMA8452Q_t;

/*
 * INITIALIZATION FUNCTION
 */

uint8_t MMA8452Q_Init (MMA8452Q_t *device, I2C_HandleTypeDef *i2c_handle);

/*
 * CONFIGURATION FUNCTIONS
 */



/*
 * DATA AQUISITION
 */

HAL_StatusTypeDef MMA8452Q_ReadAccel (MMA8452Q_t *device);
HAL_StatusTypeDef MMA8452Q_ReadOrientation (MMA8452Q_t *device);

/*
 * LOW LEVEL FUNCTIONS
 */

HAL_StatusTypeDef MMA8452Q_Byte_Read (MMA8452Q_t *device, uint8_t reg, uint8_t *data);
HAL_StatusTypeDef MMA8452Q_MultiByte_Read (MMA8452Q_t *device, uint8_t reg, uint8_t *data, uint8_t length);
HAL_StatusTypeDef MMA8452Q_Byte_Write (MMA8452Q_t *device, uint8_t reg, uint8_t *data);
HAL_StatusTypeDef MMA8452Q_MultiByte_Write (MMA8452Q_t *device, uint8_t reg, uint8_t *data, uint8_t length);

#endif /* INC_MMA8452Q_H_ */
