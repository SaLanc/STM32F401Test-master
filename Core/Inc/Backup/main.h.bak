/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.h
 * @brief          : Header for main.c file.
 *                   This file contains the common defines of the application.
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2023 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include "stdbool.h"

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

  typedef struct buzzer
  {
    bool buzzing;
    bool mute;
    uint32_t buzzDurration;
    uint32_t frequency;
    uint8_t dutyCycle;
    bool cycle_start_task;
    bool cycle_stop_task;
  } buzzer_t;

  typedef struct button
  {
    bool pressLogged;
    bool depressed;
    uint32_t pressStart;
    uint32_t lastRelease;
    uint32_t pressNumber;
  } button_t;

  typedef struct sample
  {
    bool takeNewSample;
    uint32_t tempRaw;
    uint32_t pressureRaw;
    float pressureMbar;
    float temperatureCelcus;
    float pressure;
    float sampleMeters;
    float lastSampleMeters;
    float sampleFeet;
    uint32_t lastSampleTime;
    uint32_t sampleTime;
    uint32_t sampleDT;
    uint16_t battRaw;
    float battVoltage;
  } sample_t;

  typedef struct GPS
  {
    char message[255];
    char finishedMessage[255];
    bool finishedMessageReady;
    bool gps_usb_bypass;
    uint16_t finishedMessageSize;
  } GPS_T;

  typedef struct vario
  {
    float lift;
    float dT;
    float dX;
    float dxdt;
    float AVGdXdT;
  } vario_t;

  typedef struct ParaBeep_t
  {
    float altitude;
    buzzer_t buzzer;
    sample_t sample;
    vario_t vario;
    button_t button;
    GPS_T GPS;
    bool USBConnected;
  } ParaBeep_t;

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define B1_Pin GPIO_PIN_13
#define B1_GPIO_Port GPIOC
#define USART_TX_Pin GPIO_PIN_2
#define USART_TX_GPIO_Port GPIOA
#define USART_RX_Pin GPIO_PIN_3
#define USART_RX_GPIO_Port GPIOA
#define CS_BARO_Pin GPIO_PIN_4
#define CS_BARO_GPIO_Port GPIOA
#define LD2_Pin GPIO_PIN_5
#define LD2_GPIO_Port GPIOA
#define TMS_Pin GPIO_PIN_13
#define TMS_GPIO_Port GPIOA
#define TCK_Pin GPIO_PIN_14
#define TCK_GPIO_Port GPIOA
#define SWO_Pin GPIO_PIN_3
#define SWO_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
