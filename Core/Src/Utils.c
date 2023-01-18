#include "Utils.h"
#include "usbd_cdc_if.h"
#include <reent.h>
#include "main.h"

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

void ParaBeep_Init(ParaBeep_t *ParaBeep)
{
    ringbuffer_init(&USB_RX_RING_BUFFER_STRUCT, USB_RX_RING_BUFFER, sizeof(USB_RX_RING_BUFFER));
    ringbuffer_init(&USART2_RX_RING_BUFFER_STRUCT, USART2_RX_RING_BUFFER, sizeof(USART2_RX_RING_BUFFER));
    ringbuffer_init(&USART1_RX_RING_BUFFER_STRUCT, USART1_RX_RING_BUFFER, sizeof(USART1_RX_RING_BUFFER));
    HAL_UARTEx_ReceiveToIdle_DMA(&huart2, UART2DMAdataRx, 64);
    HAL_UARTEx_ReceiveToIdle_DMA(&huart1, UART1DMAdataRx, 64);

    MS5803_Init();

    HAL_TIM_PWM_Start_IT(&htim2, TIM_CHANNEL_1);
    HAL_TIM_Base_Start_IT(&htim2);
    TIM2->ARR = 10000;
    TIM2->CCR1 = 500;
    TIM2->CNT = 4900;

    HAL_TIM_Base_Start_IT(&htim4);

    ParaBeep->MS5803.state = MS5803_STATE_NONE;
    ParaBeep->MS5803.takeNewSample = true;
    ParaBeep->MS5803.SampleReady = false;
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

void ButtonTick(ParaBeep_t *ParaBeep)
{
    if (ParaBeep->button.depressed)
    {
        if ((ParaBeep->Tick - ParaBeep->button.pressStart) > 1000)
        {
            Enter_Standby();
        }
    }
    if (ParaBeep->button.pressNumber > 0)
    {
        if ((ParaBeep->Tick - ParaBeep->button.lastRelease) > 500)
        {
            ParaBeep->button.pressNumber = 0;
        }
        if (ParaBeep->button.pressNumber == 2)
        {
            if (ParaBeep->buzzer.mute)
            {
               ParaBeep->buzzer.mute = false;
                HAL_TIM_PWM_Start_IT(&htim2, TIM_CHANNEL_1);
                HAL_TIM_Base_Start_IT(&htim2);
                TIM2->CNT = 1;


            } else
            {
                ParaBeep->buzzer.mute = true;
                HAL_TIM_PWM_Stop_IT(&htim2, TIM_CHANNEL_1);
                HAL_TIM_Base_Stop_IT(&htim2);


            }
            
        }
        
        
    }
    
}

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
    uint32_t Len = (uint32_t)Size;

    if (huart->Instance == USART2)
    {
        USART2_RX_RINGPUFFER_PUT((uint8_t *)&UART2DMAdataRx, &Len);
        USB_RX_RINGPUFFER_PUT((uint8_t *)&UART2DMAdataRx, &Len);
        // Restart DMA Callback
        HAL_UARTEx_ReceiveToIdle_DMA(&huart2, UART2DMAdataRx, 64);
    }
    if (huart->Instance == USART1)
    {
        USART1_RX_RINGPUFFER_PUT((uint8_t *)&UART1DMAdataRx, &Len);
        USB_RX_RINGPUFFER_PUT((uint8_t *)&UART2DMAdataRx, &Len);
        // Restart DMA Callback
        HAL_UARTEx_ReceiveToIdle_DMA(&huart1, UART1DMAdataRx, 64);
    }
}

void Enter_Standby()
{
    /* Clear the WU FLAG */
    __HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU);

    /* Enable the WAKEUP PIN */
    HAL_PWR_EnableWakeUpPin(PWR_WAKEUP_PIN1);

    HAL_PWR_EnterSTANDBYMode();
}

_ssize_t _write_r(struct _reent *ptr, /* Don't worry about what's in this for the simple case */
                  int fd,             /* ignored */
                  const void *buf,    /* the data to be sent out the UART */
                  size_t cnt)         /* the number of bytes to be sent */
{
    /* Replace "huart3" with the pointer to the UART or USART instance you are using
     * in your project
     */
    CDC_Transmit_FS((uint8_t *)buf, cnt);
    return (_ssize_t)cnt;
}
