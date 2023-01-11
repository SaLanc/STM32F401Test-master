
#include "main.h"

void MS5803_Init();

float MS5803_GetAltitudeBlocking();

uint32_t readMS5803AdcResultsOLD();

uint32_t MS5803_GetTemperature_Blocking();

uint32_t MS5803_GetPressure_Blocking();

uint32_t readMS5803AdcResults();

void MS5803_SendCmdAdcD1_2048();

void MS5803_SendCmdAdcD2_2048();

void MS5803_DisableSlaveRXCplt();

void MS5803_DisableSlaveTXCplt();
