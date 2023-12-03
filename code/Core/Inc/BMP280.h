/*
 * BMP280.h
 *      Author: Wissal slimen et Marzouk ferdawes
 */

#ifndef SRC_BMP280_SIMPLE_H_
#define SRC_BMP280_SIMPLE_H_

typedef uint32_t BMP280_U32_t;
typedef int32_t BMP280_S32_t;
typedef int64_t BMP280_S64_t;

static const uint8_t BMP280_ADDR = 0x77 << 1; // Use 8-bit address
// static const uint8_t BMP280_ADDR = 0x76 << 1; // Use 8-bit address

static const uint8_t BMP280_ID_REG = 0xD0;
static const uint8_t BMP280_ID_LEN = 1;
static const uint8_t BMP280_ID_VAL = 0x58;

static const uint8_t BMP280_CTRL_MEAS_REG = 0xF4;

static const uint8_t BMP280_PRES_REG_MSB = 0xF7;
static const uint8_t BMP280_PRES_LEN = 3;

static const uint8_t BMP280_TEMP_REG_MSB = 0xFA;
static const uint8_t BMP280_TEMP_LEN = 3;

static const uint8_t BMP280_TRIM_REG_MSB = 0x88;
static const uint8_t BMP280_TRIM_LEN = 12 * 2;

//uint8_t temp_calib[3];
//uint8_t pres_calib[3];

#define Calibration_size 26
int BMP280_check();
void BMP280_get_trimming(uint8_t calibration_data[Calibration_size]);
BMP280_U32_t bmp280_compensate_P_int32(BMP280_S32_t adc_P);
BMP280_S32_t bmp280_compensate_T_int32(BMP280_S32_t adc_T);
int BMP280_init();
BMP280_S32_t BMP280_get_temperature();
int BMP280_get_pressure();
uint8_t* BMP280_Read_Reg(uint8_t reg, uint8_t value);
int BMP280_Write_Reg(uint8_t reg, uint8_t value);


#endif /* SRC_BMP280_H_ */
