#include "MS5803.h"
#include "spi.h"
#include <math.h>

uint8_t MS5803_CMD_RES = 0x1E;
uint8_t cmdAdcRead_ = 0x00;
uint8_t cmdAdcD1_ = 0x40;
uint8_t cmdAdcD2_ = 0x50;
uint8_t cmdAdcD1_4096_ = 0x48;
uint8_t cmdAdcD2_4096_ = 0x58;
uint8_t cmdAdcD1_2048_ = 0x46;
uint8_t cmdAdcD2_2048_ = 0x56;
uint8_t MS5803_CMD_PROM_READ = 0xA0;

uint16_t coefficients_[8];

void MS5803_Init()
{

    HAL_SPI_Transmit_IT(&hspi3, &MS5803_CMD_RES, 1);
    HAL_Delay(300);

    for (uint8_t coeff_num = 1; coeff_num < 7; ++coeff_num)
    {
        uint8_t _cmd = MS5803_CMD_PROM_READ + ((coeff_num)*2);
        HAL_Delay(1);
        HAL_SPI_Transmit_IT(&hspi3, &_cmd, 1);
        HAL_Delay(1);
        uint8_t _byte1, _byte2;
        HAL_SPI_Receive_IT(&hspi3, &_byte1, 1);
        HAL_SPI_Receive_IT(&hspi3, &_byte2, 1);
        HAL_Delay(1);

        coefficients_[coeff_num] = (_byte1 << 8) | _byte2;

        
    }
}

uint32_t readMS5803AdcResultsOLD()
{
    uint8_t _byte1, _byte2, _byte3;

    HAL_SPI_Transmit_IT(&hspi3, &cmdAdcRead_, 1);
    HAL_SPI_Receive_IT(&hspi3, &_byte1, 1);
    HAL_SPI_Receive_IT(&hspi3, &_byte2, 1);
    HAL_SPI_Receive_IT(&hspi3, &_byte3, 1);

    uint32_t _receive = (_byte1 << 16) | (_byte2 << 8) | (_byte3);
    return _receive;
}

uint32_t readMS5803AdcResults()
{
    uint32_t _AdcResult[3];

    HAL_SPI_Transmit_IT(&hspi3, &cmdAdcRead_, 1);
    HAL_SPI_Receive_IT(&hspi3, (uint8_t *)&_AdcResult, 3);

    return (uint32_t)_AdcResult;
}