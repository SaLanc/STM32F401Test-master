#include "Utils.h"
#include "usbd_cdc_if.h"

uint8_t USBdataRx[64];
uint32_t USBBuffSize;

uint8_t UART2DMAdataRx[64];
uint8_t UART2dataRx[64];
uint32_t UART2BuffSize;

uint8_t UART1DMAdataRx[64];
uint8_t UART1dataRx[64];
uint32_t UART1BuffSize;

uint8_t USB_RX_RING_BUFFER[64];
struct ringbuffer USB_RX_RING_BUFFER_STRUCT;

uint8_t USART1_RX_RING_BUFFER[64];
struct ringbuffer USART1_RX_RING_BUFFER_STRUCT;

uint8_t USART2_RX_RING_BUFFER[64];
struct ringbuffer USART2_RX_RING_BUFFER_STRUCT;

void UtilsInit()
{
    ringbuffer_init(&USB_RX_RING_BUFFER_STRUCT, USB_RX_RING_BUFFER, sizeof(USB_RX_RING_BUFFER));
    ringbuffer_init(&USART2_RX_RING_BUFFER_STRUCT, USART2_RX_RING_BUFFER, sizeof(USART2_RX_RING_BUFFER));
    ringbuffer_init(&USART1_RX_RING_BUFFER_STRUCT, USART1_RX_RING_BUFFER, sizeof(USART1_RX_RING_BUFFER));
    HAL_UARTEx_ReceiveToIdle_DMA(&huart2, UART2DMAdataRx, 64);
    HAL_UARTEx_ReceiveToIdle_DMA(&huart1, UART1DMAdataRx, 64);

}

void USB_RX_RINGPUFFER_PUT(uint8_t *Buf, uint32_t *Len)
{
    for (uint32_t i = 0; i < *Len; i++)
    {
        ringbuffer_put(&USB_RX_RING_BUFFER_STRUCT, Buf[i]);
    }
}

void USART2_RX_RINGPUFFER_PUT(uint8_t *Buf, uint32_t *Len)
{
    for (uint32_t i = 0; i < *Len; i++)
    {
        ringbuffer_put(&USART2_RX_RING_BUFFER_STRUCT, Buf[i]);
    }
}

void USART1_RX_RINGPUFFER_PUT(uint8_t *Buf, uint32_t *Len)
{
    for (uint32_t i = 0; i < *Len; i++)
    {
        ringbuffer_put(&USART1_RX_RING_BUFFER_STRUCT, Buf[i]);
    }
}

void SendUART2RingBuffer()
{
    UART2BuffSize = ringbuffer_num(&USART2_RX_RING_BUFFER_STRUCT);

    if (UART2BuffSize != 0)
    {
        for (size_t i = 0; i < UART2BuffSize; i++)
        {
            ringbuffer_get(&USART2_RX_RING_BUFFER_STRUCT, &UART2dataRx[i]);
        }
        HAL_UART_Transmit_IT(&huart2, (uint8_t *)&UART2dataRx, UART2BuffSize);
    }
}

void SendUART1RingBuffer()
{
    UART1BuffSize = ringbuffer_num(&USART1_RX_RING_BUFFER_STRUCT);

    if (UART1BuffSize != 0)
    {
        for (size_t i = 0; i < UART1BuffSize; i++)
        {
            ringbuffer_get(&USART1_RX_RING_BUFFER_STRUCT, &UART1dataRx[i]);
        }
        HAL_UART_Transmit_IT(&huart1, (uint8_t *)&UART1dataRx, UART1BuffSize);
    }
}

void SendUSBRingBuffer()
{
    USBBuffSize = ringbuffer_num(&USB_RX_RING_BUFFER_STRUCT);

    if (USBBuffSize != 0)
    {
        for (size_t i = 0; i < USBBuffSize; i++)
        {
            ringbuffer_get(&USB_RX_RING_BUFFER_STRUCT, &USBdataRx[i]);
        }
        CDC_Transmit_FS((uint8_t *)&USBdataRx, USBBuffSize);
    }
}

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
    uint32_t Len = (uint32_t)Size;

    if (huart->Instance == USART2)
    {
        USART2_RX_RINGPUFFER_PUT((uint8_t *)&UART2DMAdataRx , &Len);
        USB_RX_RINGPUFFER_PUT((uint8_t *)&UART2DMAdataRx , &Len);

        HAL_UARTEx_ReceiveToIdle_DMA(&huart2, UART2DMAdataRx, 64);
    }
    if (huart->Instance == USART1)
    {
        USART1_RX_RINGPUFFER_PUT((uint8_t *)&UART1DMAdataRx , &Len);
        USB_RX_RINGPUFFER_PUT((uint8_t *)&UART2DMAdataRx , &Len);

        HAL_UARTEx_ReceiveToIdle_DMA(&huart1, UART1DMAdataRx, 64);
    }
}