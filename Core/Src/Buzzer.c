

#include "Buzzer.h"


float tonalArray[8][4] = {
    {-0.51, 200, 800, 100},
    {-0.50, 300, 800, 5},
    {0.10,  500, 500, 20},
    {0.30,  510, 400, 30},
    {1.16,  650, 352, 52},
    {2.67,  863, 183, 55},
    {4.24,  1085, 212, 58},
    {6.00, 1334, 322, 66}
};

void buzzerStop(buzzer_t *buzzer)
{
    buzzer->mute = true;
    HAL_TIM_Base_Stop_IT(&htim2);
}

void buzzerStart(buzzer_t *buzzer)
{
    buzzer->mute = false;
    TIM2->CNT = 1;
    HAL_TIM_Base_Start_IT(&htim2);
}

void buzzerBeep(buzzer_t *buzzer, uint32_t frequency, uint32_t duration)
{
    buzzerSetFrequency(buzzer, frequency);
    buzzerSetDuration(buzzer, duration);
    buzzerSetDutyCycle(buzzer, 100);
    buzzerStart(buzzer);
    buzzer->oneShot = true;
}

void buzzerSetFrequency(buzzer_t *buzzer, uint32_t frequency)
{
    uint32_t per = (84000000/84) / frequency;
    TIM3->ARR = per;
    buzzer->frequency = frequency;
}

void buzzerSetDuration(buzzer_t *buzzer, uint32_t duration)
{
    TIM2->ARR = duration;
    buzzer->buzzDurration = duration;
}

void buzzerSetDutyCycle(buzzer_t *buzzer, uint32_t dutyCycle)
{
    TIM2->CCR1 = TIM2->ARR * (dutyCycle/100);
    buzzer->dutyCycle = buzzer->buzzDurration * dutyCycle/100;
}

void buzzerMuteToggle(buzzer_t *buzzer)
{
    if (buzzer->mute)
    {
        buzzerStart(buzzer);
    } else
    {   
        buzzerStop(buzzer);
    }  
}
