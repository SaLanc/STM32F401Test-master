#include "Utils.h"
#include "usbd_cdc_if.h"

uint8_t USBdataRx[64];
uint32_t USBBuffSize;
uint8_t UARTdataRx[64];
uint32_t UARTBuffSize;

uint8_t UARTdataRx2[64];

uint8_t USB_RX_RING_BUFFER[64];
struct ringbuffer USB_RX_RING_BUFFER_STRUCT;

uint8_t USART3_RX_RING_BUFFER[64];
struct ringbuffer USART2_RX_RING_BUFFER_STRUCT;

void UtilsInit()
{
    ringbuffer_init(&USB_RX_RING_BUFFER_STRUCT, USB_RX_RING_BUFFER, sizeof(USB_RX_RING_BUFFER));
    ringbuffer_init(&USART2_RX_RING_BUFFER_STRUCT, USART3_RX_RING_BUFFER, sizeof(USART3_RX_RING_BUFFER));
    HAL_UARTEx_ReceiveToIdle_DMA(&huart2, UARTdataRx2, 64);
}

void USB_RX_RINGPUFFER_PUT(uint8_t *Buf, uint16_t Len)
{
    for (uint16_t i = 0; i < Len; i++)
    {
        ringbuffer_put(&USB_RX_RING_BUFFER_STRUCT, Buf[i]);
    }
}

void USART_RX_RINGPUFFER_PUT(uint8_t *Buf, uint16_t Len)
{
    for (uint16_t i = 0; i < Len; i++)
    {
        ringbuffer_put(&USART2_RX_RING_BUFFER_STRUCT, Buf[i]);
    }
}

void SendUARTRingBuffer()
{
    UARTBuffSize = ringbuffer_num(&USART2_RX_RING_BUFFER_STRUCT);

    if (UARTBuffSize != 0)
    {
        for (size_t i = 0; i < UARTBuffSize; i++)
        {
            ringbuffer_get(&USART2_RX_RING_BUFFER_STRUCT, &UARTdataRx[i]);
        }
        HAL_UART_Transmit_IT(&huart2, (uint8_t *)&UARTdataRx, UARTBuffSize);
    }
}

void SendUSBRingBuffer()
{
    USBBuffSize = ringbuffer_num(&USB_RX_RING_BUFFER_STRUCT);

    if (USBBuffSize != 0)
    {
        for (size_t i = 0; i < USBBuffSize; i++)
        {
            ringbuffer_get(&USART2_RX_RING_BUFFER_STRUCT, &USBdataRx[i]);
        }
        CDC_Transmit_FS((uint8_t *)&USBdataRx, USBBuffSize);
    }
}

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
    if (huart->Instance == USART2)
    {
        uint8_t bytes = (uint8_t)Size;
        for (uint8_t i = 0; i < bytes; i++)
        {
            ringbuffer_put(&USART2_RX_RING_BUFFER_STRUCT, UARTdataRx2[i]);
        }
        HAL_UARTEx_ReceiveToIdle_DMA(&huart2, UARTdataRx2, 64);
    }
}