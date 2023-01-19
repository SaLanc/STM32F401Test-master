
#include "main.h"
#include "tim.h"



void buzzerStop(buzzer_t *buzzer);

void buzzerStart(buzzer_t *buzzer);

void buzzerBeep(buzzer_t *buzzer, uint32_t frequency, uint32_t duration);

void buzzerSetFrequency(buzzer_t *buzzer, uint32_t frequency);

void buzzerSetDuration(buzzer_t *buzzer, uint32_t duration);

void buzzerSetDutyCycle(buzzer_t *buzzer, uint32_t dutyCycle);

void buzzerMuteToggle(buzzer_t *buzzer);


