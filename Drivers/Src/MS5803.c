#include "MS5803.h"
#include "spi.h"
#include <math.h>
#include <stdbool.h>

uint8_t MS5803_CMD_RES = 0x1E;
uint8_t cmdAdcRead_ = 0x00;
uint8_t cmdAdcD1_ = 0x40;
uint8_t cmdAdcD2_ = 0x50;
uint8_t cmdAdcD1_4096_ = 0x48;
uint8_t cmdAdcD2_4096_ = 0x58;
uint8_t cmdAdcD1_2048_ = 0x46;
uint8_t cmdAdcD2_2048_ = 0x56;
uint8_t MS5803_CMD_PROM_READ = 0xA0;

bool MS5803_Disable_SS_RX_Clpt = false;
bool MS5803_Disable_SS_TX_Clpt = false;

uint16_t coefficients_[8];

void MS5803_Init()
{

    HAL_GPIO_WritePin(CS_BARO_GPIO_Port, CS_BARO_Pin, GPIO_PIN_RESET);
    HAL_SPI_Transmit(&hspi3, &MS5803_CMD_RES, 1, 100);
    HAL_GPIO_WritePin(CS_BARO_GPIO_Port, CS_BARO_Pin, GPIO_PIN_SET);
    // Wait for MS5803 to reboot
    HAL_Delay(400);

    for (uint8_t coeff_num = 1; coeff_num < 7; ++coeff_num)
    {
        uint8_t _cmd = MS5803_CMD_PROM_READ + ((coeff_num)*2);
        HAL_GPIO_WritePin(CS_BARO_GPIO_Port, CS_BARO_Pin, GPIO_PIN_RESET);
        HAL_SPI_Transmit(&hspi3, &_cmd, 1, 100);
        //HAL_Delay(1);
        uint8_t _byte1, _byte2;
        HAL_SPI_Receive(&hspi3, &_byte1, 1, 100);
        HAL_SPI_Receive(&hspi3, &_byte2, 1, 100);
        HAL_GPIO_WritePin(CS_BARO_GPIO_Port, CS_BARO_Pin, GPIO_PIN_SET);
        HAL_Delay(1);

        coefficients_[coeff_num] = (_byte1 << 8) | _byte2;
    }
}

float MS5803_GetAltitudeBlocking(sample_t *ThisSample)
{

    ThisSample->tempRaw = MS5803_GetTemperature_Blocking();
    ThisSample->pressureRaw = MS5803_GetPressure_Blocking();

    const uint16_t C1 = coefficients_[1];
    const uint16_t C2 = coefficients_[2];
    const uint16_t C3 = coefficients_[3];
    const uint16_t C4 = coefficients_[4];
    const uint16_t C5 = coefficients_[5];
    const uint16_t C6 = coefficients_[6];

    // calculate 1st order pressure and temperature (MS5607 1st order algorithm)
    volatile const int32_t dT = (ThisSample->tempRaw) - ((int32_t)C5 << 8);
    ThisSample->temperatureCelcus = 2000 + (((int64_t)dT * C6) >> 23);

    volatile const int64_t OFF = ((int64_t)C2 << 16) + ((C4 * (int64_t)dT) >> 7);
    volatile const int64_t SENS = ((int64_t)C1 << 15) + ((C3 * (int64_t)dT) >> 8);
    ThisSample->pressureMbar = ((((ThisSample->pressureRaw * SENS) >> 21) - OFF) >> 15);
    ThisSample->pressureMbar = ThisSample->pressureMbar / 100;

    float lower = (((float)(ThisSample->pressureMbar)) / 1013.25);
    float exponent = 0.190284;

    float altFeet = (1 - pow(lower, exponent)) * 145366.45;
    return altFeet;
}

uint32_t readMS5803AdcResultsOLD()
{
    uint8_t _byte1, _byte2, _byte3;

    HAL_GPIO_WritePin(CS_BARO_GPIO_Port, CS_BARO_Pin, GPIO_PIN_RESET);
    HAL_SPI_Transmit(&hspi3, &cmdAdcRead_, 1, 100);
    HAL_SPI_Receive(&hspi3, &_byte1, 1, 100);
    HAL_SPI_Receive(&hspi3, &_byte2, 1, 100);
    HAL_SPI_Receive(&hspi3, &_byte3, 1, 100);
    HAL_GPIO_WritePin(CS_BARO_GPIO_Port, CS_BARO_Pin, GPIO_PIN_SET);

    uint32_t _receive = (_byte1 << 16) | (_byte2 << 8) | (_byte3);
    return _receive;
}

uint32_t MS5803_GetTemperature_Blocking()
{

    MS5803_SendCmdAdcD2_2048();
    HAL_Delay(6);
    uint32_t _temperature = readMS5803AdcResultsOLD();
    return _temperature;
}

uint32_t MS5803_GetPressure_Blocking()
{

    MS5803_SendCmdAdcD1_2048();
    HAL_Delay(6);
    uint32_t _pressure = readMS5803AdcResultsOLD();
    return _pressure;
}

uint32_t readMS5803AdcResults()
{
    uint32_t _AdcResult[3];

    HAL_GPIO_WritePin(CS_BARO_GPIO_Port, CS_BARO_Pin, GPIO_PIN_RESET);
    MS5803_Disable_SS_RX_Clpt = true;
    HAL_SPI_Transmit_IT(&hspi3, &cmdAdcRead_, 1);
    HAL_SPI_Receive_IT(&hspi3, (uint8_t *)&_AdcResult, 3);

    return (uint32_t)_AdcResult;
}

void MS5803_SendCmdAdcD1_2048()
{
    // cook pressure value request
    //MS5803_Disable_SS_TX_Clpt = true;
    HAL_GPIO_WritePin(CS_BARO_GPIO_Port, CS_BARO_Pin, GPIO_PIN_RESET);
    HAL_SPI_Transmit(&hspi3, &cmdAdcD1_2048_, 1, 100);
    HAL_GPIO_WritePin(CS_BARO_GPIO_Port, CS_BARO_Pin, GPIO_PIN_SET);

}

void MS5803_SendCmdAdcD2_2048()
{
    // cook temperature value request
    //MS5803_Disable_SS_TX_Clpt = true;
    HAL_GPIO_WritePin(CS_BARO_GPIO_Port, CS_BARO_Pin, GPIO_PIN_RESET);
    HAL_SPI_Transmit(&hspi3, &cmdAdcD2_2048_, 1, 100);
    HAL_GPIO_WritePin(CS_BARO_GPIO_Port, CS_BARO_Pin, GPIO_PIN_SET);

}

void MS5803_DisableSlaveRXCplt()
{
    if (MS5803_Disable_SS_RX_Clpt)
    {
        MS5803_Disable_SS_RX_Clpt = false;
        HAL_GPIO_WritePin(CS_BARO_GPIO_Port, CS_BARO_Pin, GPIO_PIN_SET);
    }
}

void MS5803_DisableSlaveTXCplt()
{
    if (MS5803_Disable_SS_TX_Clpt)
    {
        MS5803_Disable_SS_TX_Clpt = false;
        HAL_GPIO_WritePin(CS_BARO_GPIO_Port, CS_BARO_Pin, GPIO_PIN_SET);
    }
}
