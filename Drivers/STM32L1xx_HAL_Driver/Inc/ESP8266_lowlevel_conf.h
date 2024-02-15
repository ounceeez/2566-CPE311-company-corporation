#ifndef __ESP8266_LOWLEVEL_CONF_H__
#define __ESP8266_LOWLEVEL_CONF_H__

#include "stm32l1xx.h"
#include "stm32l1xx_ll_gpio.h"
#include "stm32l1xx_ll_usart.h"
#include "stm32l1xx_ll_bus.h"
#include <stdint.h>


/* Definition --------------------------------------------*/
#define USARTx_RX_PIN_CLK_EN()		LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA)
#define USARTx_RX_PIN							LL_GPIO_PIN_3
#define USARTx_RX_PORT						GPIOA
#define USARTx_RX_AF							LL_GPIO_AF_7

#define USARTx_TX_PIN_CLK_EN()		LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA)
#define USARTx_TX_PIN							LL_GPIO_PIN_2
#define USARTx_TX_PORT						GPIOA
#define USARTx_TX_AF							LL_GPIO_AF_7

#define USARTx_CLK_EN()						LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_USART2)
#define USARTx_BAUDRATE						115200
#define USARTx										USART2

#define USARTx_IRQn								USART2_IRQn
#define USARTx_Callback()					USART2_IRQHandler(void)

#define MAX_CIR_BUFFER_SIZE				250

/* Private TypeDef -----------------------------------------------*/
typedef struct {
	uint8_t read;
	uint8_t write;
	uint8_t cb[MAX_CIR_BUFFER_SIZE];
	uint8_t nbr_byte_to_read;
}cbuf_t;

/* Prototpye ---------------------------------------------*/
void ESP_USART_LOWLEVEL_Conf(void);
void ESP_USART_LOWLEVEL_Transmit(uint8_t*);
uint8_t ESP_USART_LOWLEVEL_Recv(uint8_t*, uint8_t);
void ESP_USART_Start(void);

void peek(void);

#endif
