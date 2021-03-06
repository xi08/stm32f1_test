// code = utf-8
#include "uart.h"
#include "stdio.h"
/**
 * @brief 串口接收缓冲区
 *
 */
uint8_t uartRxBuffer[uartNum][uartRxBufferSize];

/**
 * @brief 缓冲区使用计数
 *
 */
uint8_t uartRxBufferIdx[uartNum];

/**
 * @brief 缓冲区覆写标记
 *
 */
uint8_t uartRxBufferDirtyFlag;

/**
 * @brief 初始化串口1
 *
 * @param baudRate 比特率
 */
void uart1Init(uint32_t baudRate)
{
    GPIO_InitTypeDef initStruct_GPIO;
    USART_InitTypeDef initStruct_USART;
    NVIC_InitTypeDef initStruct_NVIC;

    /* 时钟设置 */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

    /* GPIO设置 */
    initStruct_GPIO.GPIO_Speed = GPIO_Speed_10MHz;     // 接口速率
    initStruct_GPIO.GPIO_Pin = GPIO_Pin_9;             // Tx引脚
    initStruct_GPIO.GPIO_Mode = GPIO_Mode_AF_PP;       // Tx引脚
    GPIO_Init(GPIOA, &initStruct_GPIO);                // 设置Tx
    initStruct_GPIO.GPIO_Pin = GPIO_Pin_10;            // Rx引脚
    initStruct_GPIO.GPIO_Mode = GPIO_Mode_IN_FLOATING; // Rx引脚
    GPIO_Init(GPIOA, &initStruct_GPIO);                // 设置Rx

    /* USART设置 */
    initStruct_USART.USART_BaudRate = baudRate;
    initStruct_USART.USART_WordLength = USART_WordLength_8b;                     // 8位数据
    initStruct_USART.USART_StopBits = USART_StopBits_1;                          // 1位停止
    initStruct_USART.USART_Parity = USART_Parity_No;                             // 无校验
    initStruct_USART.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;                 // 双向通信
    initStruct_USART.USART_HardwareFlowControl = USART_HardwareFlowControl_None; // 无硬件流控
    USART_Init(USART1, &initStruct_USART);                                       // 设置USART模块
    USART_Cmd(USART1, ENABLE);                                                   // 使能USART1

    /* 接收中断设置 */
    initStruct_NVIC.NVIC_IRQChannel = USART1_IRQn;         // 设置中断号
    initStruct_NVIC.NVIC_IRQChannelCmd = ENABLE;           // 设置中断使能状态
    initStruct_NVIC.NVIC_IRQChannelPreemptionPriority = 0; // 设置抢占优先级
    initStruct_NVIC.NVIC_IRQChannelSubPriority = 3;        // 设置子优先级
    NVIC_Init(&initStruct_NVIC);                           // 设置中断
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);         // 允许USART接收时产生中断
}

/**
 * @brief 初始化串口2
 *
 * @param baudRate 比特率
 */
void uart2Init(uint32_t baudRate)
{
    GPIO_InitTypeDef initStruct_GPIO;
    USART_InitTypeDef initStruct_USART;
    NVIC_InitTypeDef initStruct_NVIC;

    /* 时钟设置 */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); // 使能GPIOA时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE); // 使能USART2时钟

    /* GPIO设置 */
    initStruct_GPIO.GPIO_Speed = GPIO_Speed_10MHz;     // 接口速率
    initStruct_GPIO.GPIO_Pin = GPIO_Pin_2;             // Tx引脚
    initStruct_GPIO.GPIO_Mode = GPIO_Mode_AF_PP;       // Tx引脚
    GPIO_Init(GPIOA, &initStruct_GPIO);                // 设置Tx
    initStruct_GPIO.GPIO_Pin = GPIO_Pin_3;             // Rx引脚
    initStruct_GPIO.GPIO_Mode = GPIO_Mode_IN_FLOATING; // Rx引脚
    GPIO_Init(GPIOA, &initStruct_GPIO);                // 设置Rx

    /* USART设置 */
    initStruct_USART.USART_BaudRate = baudRate;
    initStruct_USART.USART_WordLength = USART_WordLength_8b;                     // 8位数据
    initStruct_USART.USART_StopBits = USART_StopBits_1;                          // 1位停止
    initStruct_USART.USART_Parity = USART_Parity_No;                             // 无校验
    initStruct_USART.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;                 // 双向通信
    initStruct_USART.USART_HardwareFlowControl = USART_HardwareFlowControl_None; // 无硬件流控
    USART_Init(USART2, &initStruct_USART);                                       // 设置USART模块
    USART_Cmd(USART2, ENABLE);                                                   // 使能USART2

    /* 接收中断设置 */
    initStruct_NVIC.NVIC_IRQChannel = USART2_IRQn;         // 设置中断号
    initStruct_NVIC.NVIC_IRQChannelCmd = ENABLE;           // 设置中断使能状态
    initStruct_NVIC.NVIC_IRQChannelPreemptionPriority = 0; // 设置抢占优先级
    initStruct_NVIC.NVIC_IRQChannelSubPriority = 3;        // 设置子优先级
    NVIC_Init(&initStruct_NVIC);                           // 设置中断
    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);         // 允许USART接收时产生中断
}

/**
 * @brief 串口1接收中断
 *
 */
void USART1_IRQHandler(void)
{
    uint8_t ch;
    if (USART_GetITStatus(USART1, USART_IT_RXNE)) // 检测到USART1接收非空
    {
        ch = USART_ReceiveData(USART1); // 暂存USART1接收到的数据

        /* 发送结束检测 */
        if (ch == (uint8_t)uartStopSymbol) 
        {
            uartRxBuffer[0][uartRxBufferIdx[0]] = '\0';
            uartRxBufferIdx[0] = 0;
            uartRxBufferDirtyFlag |= (1<<0);
            
        }
        else
            uartRxBuffer[0][uartRxBufferIdx[0]++] = ch;
    }
}

/**
 * @brief 串口2接收中断
 *
 */
void USART2_IRQHandler(void)
{
    uint8_t ch;
    if (USART_GetITStatus(USART2, USART_IT_RXNE)) // 检测到USART2接收非空
    {
        ch = USART_ReceiveData(USART2); // 暂存USART2接收到的数据

        /* 发送结束检测 */
        if (ch == (uint8_t)uartStopSymbol)
        {
            uartRxBuffer[1][uartRxBufferIdx[1]] = '\0'; // 写入字符串结束位
            uartRxBufferIdx[1] = 0; // 清空
            uartRxBufferDirtyFlag |= (1<<1);
        }
        else
            uartRxBuffer[1][uartRxBufferIdx[1]++] = ch;
    }
}

/**
 * @brief 串口发送单字符
 *
 * @param commPort 通信接口
 * @param Char 发送的字符
 * @return uint8_t 发送的字符
 */
uint8_t uartSendChar(USART_TypeDef *commPort, uint8_t Char)
{
    while (!USART_GetFlagStatus(commPort, USART_FLAG_TXE))
        ;
    USART_SendData(commPort, Char);
    return Char;
}

/**
 * @brief 串口发送字符串
 *
 * @param commPort 通信接口
 * @param Str 发送的字符串
 */
void uartSend(USART_TypeDef *commPort, uint8_t *Str)
{
    while (*Str != '\0')
        uartSendChar(commPort, *Str++);
}

// 重定向stdout到uart
int fputc(int ch, FILE *f)
{
    return (uartSendChar(USART2, ch));
}
