/************************************************************************************
 *  File:     main.c
 *  Purpose:  Cortex-M3 main file.
 *            Replace with your code.
 *  Date:     05 July 2013
 *  Info:     If __NO_SYSTEM_INIT is defined in the Build options,
 *            the startup code will not branch to SystemInit()
 *            and the function can be removed
 ************************************************************************************/
#include "stm32f10x.h"

//#define USE_UART_INTR

void InitUSART(void)
{
    USART_InitTypeDef USART_InitStructure;
    USART_InitStructure.USART_BaudRate = 115200;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

    /* Configure USART Tx as push-pull */
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /* Configure USART Rx as input floating */
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /* USART configuration */
    USART_Init(USART2, &USART_InitStructure);

    /* Enable USART */
    USART_Cmd(USART2, ENABLE);

#ifdef USE_UART_INTR
	NVIC_InitTypeDef NVIC_InitStructure;

    // Interrupt Enable
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);

	/* Enable the USARTy Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = 1;
	NVIC_Init(&NVIC_InitStructure);
#endif
}

void outbyte(char data)
{
    while ((USART2->SR & USART_FLAG_TC) == (uint16_t) RESET) {
    }
    USART2->DR = (data & (uint16_t) 0x01FF);
}

int UART_GetChar(USART_TypeDef *pPort, char *pdata)
{
	int ret = 0;
	if ( USART_GetFlagStatus(pPort, USART_FLAG_RXNE) != RESET)
	{
		*pdata = (u8)pPort->DR;
		ret = 1;
	}
	else
	{
		ret = 0;
	}
	return ret;
}

#ifdef USE_UART_INTR
void USART2_IRQHandler(void)
{
	char rxData;
	int rxEmpty = 0;
	int count = 0;

	while (!rxEmpty)
	{
		if (UART_GetChar(USART2, &rxData) == 0)
		{
			if(count == 0x1000)
				rxEmpty = 1;
			count++;
		}
		else
		{
            outbyte(rxData);
            if(rxdata==0x0d)
                outbyte(0x0a);
		}
	}
}
#endif

/*********************************************************************
 *
 *  main()
 *
 *********************************************************************/
void main()
{
    char rxdata;
  /******************************************************************
   *
   *  Place your code here.
   ******************************************************************/
  int cnt;
  cnt = 0;
  InitUSART();
  cm_printf("this is example for small printf \r\n");
  do {
#ifndef USE_UART_INTR
		if (UART_GetChar(USART2, &rxdata) == 0)
		{
		}
		else
		{
            outbyte(rxdata);
            if(rxdata==0x0d)
                outbyte(0x0a);
		}
#endif
    cnt++;
  } while (1);
}
