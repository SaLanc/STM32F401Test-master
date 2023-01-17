#include "main.h"
#include "usart.h"
#include "tim.h"

#include "ringbuffer.h"
#include "MS5803.h"
#include "ZOEM8B.h"
#include "ANNAB112.h"

void ParaBeep_Init(ParaBeep_t *ParaBeep);

void USB_RX_RINGPUFFER_PUT(uint8_t *Buf, uint32_t *Len);

void USART2_RX_RINGPUFFER_PUT(uint8_t *Buf, uint32_t *Len);

void USART1_RX_RINGPUFFER_PUT(uint8_t *Buf, uint32_t *Len);

void SendUART2RingBuffer();

void SendUART1RingBuffer();

void SendUSBRingBuffer();

void ButtonTick(ParaBeep_t *ParaBeep);

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);

void Enter_Standby();
