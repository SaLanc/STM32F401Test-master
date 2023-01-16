
#include "main.h"







void MS5803_Init();

void MS5803_Tick(ParaBeep_t *ParaBeep);

void convertTempPress(ParaBeep_t *ParaBeep);

float MS5803_GetAltitudeBlocking(sample_t *ThisSample);

uint32_t readMS5803AdcResultsOLD();

uint32_t MS5803_GetTemperature_Blocking();

uint32_t MS5803_GetPressure_Blocking();

void readMS5803AdcResults(MS5803_t *MS5803);

void MS5803_SendCmdAdcD1_2048();

void MS5803_SendCmdAdcD2_2048();

void MS5803_DisableSlaveRXCplt(MS5803_t *MS5803);

void MS5803_DisableSlaveTXCplt();

uint8_t MS5803_CRC4();