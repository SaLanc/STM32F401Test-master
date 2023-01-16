#include "MS5803.h"
#include "spi.h"
#include "tim.h"
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

void MS5803_Tick(ParaBeep_t *ParaBeep)
{
    // if new sample requested, 
    //sends temp conv request which will start timer to advance state
    if ((ParaBeep->MS5803.state == MS5803_STATE_NONE) && (ParaBeep->MS5803.takeNewSample))
    {
        MS5803_SendCmdAdcD2_2048();
        ParaBeep->MS5803.takeNewSample = false;
        ParaBeep->MS5803.ADC_CONVERTING_FINISHED = false;
        ParaBeep->MS5803.state = MS5803_STATE_D2_CONV_WAIT;

        //add timer start code
        __HAL_TIM_ENABLE(&htim4);
       //HAL_Delay(4);
        //ParaBeep->MS5803.ADC_CONVERTING_FINISHED = true;
    }
    // waiting 3.5ms for MS5803 to take ADC
    if (ParaBeep->MS5803.state == MS5803_STATE_D2_CONV_WAIT)
    {
        //waiting untill timer triggers, indicating ready to read
        if (ParaBeep->MS5803.ADC_CONVERTING_FINISHED)
        {
            ParaBeep->MS5803.ADC_CONVERTING_FINISHED = false;
            //sends read command, 3 bytes will be placed into _AdcResult[3], to be moved into rawtemp in next step
            readMS5803AdcResults(&ParaBeep->MS5803);
        }
    }

    // Send ADC read request and recive data, IT will trigger when data has been read to start new ADC Request.
    if (ParaBeep->MS5803.state == MS5803_STATE_D2_ADC_READ)
    {
        ParaBeep->MS5803.tempRaw = (ParaBeep->MS5803._AdcResult[0] << 16) | (ParaBeep->MS5803._AdcResult[1] << 8) | (ParaBeep->MS5803._AdcResult[2]);

        MS5803_SendCmdAdcD1_2048();
        ParaBeep->MS5803.state = MS5803_STATE_D1_CONV_WAIT;


        //add start timer code
        __HAL_TIM_ENABLE(&htim4);

        //HAL_Delay(4);
        //ParaBeep->MS5803.ADC_CONVERTING_FINISHED = true;
    }

    // waiting 3.5ms for MS5803 to take ADC
    if (ParaBeep->MS5803.state == MS5803_STATE_D1_CONV_WAIT)
    {
        //waiting untill timer triggers, indicating ready to read
        if (ParaBeep->MS5803.ADC_CONVERTING_FINISHED)
        {
            ParaBeep->MS5803.ADC_CONVERTING_FINISHED = false;
            //sends read command, 3 bytes will be placed into _AdcResult[3], to be moved into rawtemp in next step
            readMS5803AdcResults(&ParaBeep->MS5803);
        }
    }
    
    // Send ADC read request and recive data, IT will trigger when data has been read to start TEMP/Press conv.
    if (ParaBeep->MS5803.state == MS5803_STATE_D1_ADC_READ)
    {
        ParaBeep->MS5803.pressureRaw = (ParaBeep->MS5803._AdcResult[0] << 16) | (ParaBeep->MS5803._AdcResult[1] << 8) | (ParaBeep->MS5803._AdcResult[2]);
        ParaBeep->MS5803.state = MS5803_STATE_DATA_READY;
    }

    // data is ready, convert press/temp to alt
    if (ParaBeep->MS5803.state == MS5803_STATE_DATA_READY)
    {
        convertTempPress(ParaBeep);
        ParaBeep->MS5803.SampleReady = true;
        ParaBeep->MS5803.state = MS5803_STATE_NONE;
    }

}

/*
float MS5803_GetAltitudeBlocking(ParaBeep_t *ParaBeep)
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
    ThisSample->pressureMbar = ThisSample->pressureMbar / 10;

    float lower = (((float)(MS5803->pressureMbar)) / 1013.25);
    float exponent = 0.190284;

    float altFeet = (1 - pow(lower, exponent)) * 145366.45;
    ThisSample->sampleFeet = altFeet;
    return altFeet;
}
*/


/*
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
*/

/*
uint32_t readMS5803AdcResultsOLD()
{
    uint8_t _AdcResult[3];

    HAL_GPIO_WritePin(CS_BARO_GPIO_Port, CS_BARO_Pin, GPIO_PIN_RESET);
    MS5803_Disable_SS_RX_Clpt = true;
    HAL_SPI_Transmit_IT(&hspi3, &cmdAdcRead_, 1);
    HAL_SPI_Receive_IT(&hspi3, _AdcResult, 3);
    HAL_Delay(1);
    
    uint32_t _receive = (_AdcResult[0] << 16) | (_AdcResult[1] << 8) | (_AdcResult[2]);
    return (uint32_t)_receive;
}
*/

void readMS5803AdcResults(MS5803_t *MS5803)
{
    HAL_GPIO_WritePin(CS_BARO_GPIO_Port, CS_BARO_Pin, GPIO_PIN_RESET);
    MS5803_Disable_SS_RX_Clpt = true;
    HAL_SPI_Transmit_IT(&hspi3, &cmdAdcRead_, 1);
    HAL_SPI_Receive_IT(&hspi3, (uint8_t *)&MS5803->_AdcResult, 3);
}


void convertTempPress(ParaBeep_t *ParaBeep){

    const uint16_t C1 = coefficients_[1];
    const uint16_t C2 = coefficients_[2];
    const uint16_t C3 = coefficients_[3];
    const uint16_t C4 = coefficients_[4];
    const uint16_t C5 = coefficients_[5];
    const uint16_t C6 = coefficients_[6];

    // calculate 1st order pressure and temperature (MS5607 1st order algorithm)
    volatile const int32_t dT = (ParaBeep->MS5803.tempRaw) - ((int32_t)C5 << 8);
    ParaBeep->sample.temperatureCelcus = 2000 + (((int64_t)dT * C6) >> 23);

    volatile const int64_t OFF = ((int64_t)C2 << 16) + ((C4 * (int64_t)dT) >> 7);
    volatile const int64_t SENS = ((int64_t)C1 << 15) + ((C3 * (int64_t)dT) >> 8);
    ParaBeep->sample.pressureMbar = ((((ParaBeep->MS5803.pressureRaw * SENS) >> 21) - OFF) >> 15);
    ParaBeep->sample.pressureMbar = ParaBeep->sample.pressureMbar / 10;

    float lower = (((float)(ParaBeep->sample.pressureMbar)) / 1013.25);
    float exponent = 0.190284;

    float altFeet = (1 - pow(lower, exponent)) * 145366.45;
    ParaBeep->sample.sampleFeet = altFeet;
}

void MS5803_SendCmdAdcD1_2048()
{
    // cook pressure value request
    MS5803_Disable_SS_TX_Clpt = true;
    HAL_GPIO_WritePin(CS_BARO_GPIO_Port, CS_BARO_Pin, GPIO_PIN_RESET);
    HAL_SPI_Transmit_IT(&hspi3, &cmdAdcD1_2048_, 1);

}

void MS5803_SendCmdAdcD2_2048()
{
    // cook temperature value request
    MS5803_Disable_SS_TX_Clpt = true;
    HAL_GPIO_WritePin(CS_BARO_GPIO_Port, CS_BARO_Pin, GPIO_PIN_RESET);
    HAL_SPI_Transmit_IT(&hspi3, &cmdAdcD2_2048_, 1);

}

void MS5803_DisableSlaveRXCplt(MS5803_t *MS5803)
{
    if (MS5803_Disable_SS_RX_Clpt)
    {
        MS5803_Disable_SS_RX_Clpt = false;
        HAL_GPIO_WritePin(CS_BARO_GPIO_Port, CS_BARO_Pin, GPIO_PIN_SET);
        MS5803->state++;
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

uint8_t MS5803_CRC4() {

	uint8_t count;
	uint8_t n_rem;
	uint8_t crc_read;
	uint8_t n_bit;

	n_rem = 0x00;
	crc_read = coefficients_[7];
	coefficients_[7] = (0xFF00 & (coefficients_[7]));

	for (count = 0; count < 16; count++) {
		if (count%2 == 1) {
			n_rem ^= (unsigned short) ((coefficients_[count>>1]) & 0x00FF);
			} else {
			n_rem ^= (unsigned short) (coefficients_[count>>1]>>8);
		}
		for (n_bit = 8; n_bit > 0; n_bit--) {
			if(n_rem & (0x8000)) {
				n_rem = (n_rem << 1) ^ 0x3000;
				} else {
				n_rem = (n_rem << 1);
			}
		}
	}
	n_rem = (0x000F & (n_rem >> 12));
	coefficients_[7] = crc_read;
	return (n_rem & 0x00);
}