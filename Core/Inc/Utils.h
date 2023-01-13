#include "main.h"
#include "usart.h"
#include "ringbuffer.h"

void UtilsInit();

void USB_RX_RINGPUFFER_PUT(uint8_t *Buf, uint16_t Len);

void USART_RX_RINGPUFFER_PUT(uint8_t *Buf, uint16_t Len);

void USART1_RX_RINGPUFFER_PUT(uint8_t *Buf, uint16_t Len);


void SendUARTRingBuffer();

void SendUART1RingBuffer();


void SendUSBRingBuffer();

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);
