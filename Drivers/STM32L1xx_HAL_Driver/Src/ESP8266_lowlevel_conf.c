#include "ESP8266_lowlevel_conf.h"
#include "string.h"

/* Private variable ----------------------------------------------*/
static cbuf_t esp_recv_buffer;

/* Private Prototype ----------------------------------------------*/
void peek(void)
{
	__NOP();
}

/*Circular buffer for packet managmeent*/
void cb_init(cbuf_t* c)
{
	c->nbr_byte_to_read = 0;
	c->read = 0;
	c->write = 0;
	
	memset(c->cb, NULL, MAX_CIR_BUFFER_SIZE);
}

void cb_put(cbuf_t* c, uint8_t d)
{
		c->cb[c->write] = d;
		c->write = (c->write + 1) % MAX_CIR_BUFFER_SIZE;
		c->nbr_byte_to_read = (c->nbr_byte_to_read + 1) % MAX_CIR_BUFFER_SIZE;
}

void cb_pop(cbuf_t* c, uint8_t* b)
{
	//check if there's data to be read
	if(c->nbr_byte_to_read > 0)
	{
		*b = c->cb[c->read];
		c->read = (c->read + 1) % MAX_CIR_BUFFER_SIZE;
		c->nbr_byte_to_read = (c->nbr_byte_to_read - 1);
	}
}

void eps_usart_gpio(void)
{
	LL_GPIO_InitTypeDef usart_gpio;
	
	USARTx_RX_PIN_CLK_EN();
	USARTx_TX_PIN_CLK_EN();
	
	usart_gpio.Mode = LL_GPIO_MODE_ALTERNATE;
	usart_gpio.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
	usart_gpio.Pin = USARTx_RX_PIN;
	usart_gpio.Pull = LL_GPIO_PULL_UP;
	usart_gpio.Alternate = USARTx_RX_AF;
	usart_gpio.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
	LL_GPIO_Init(USARTx_RX_PORT, &usart_gpio);
	
	usart_gpio.Pin = USARTx_TX_PIN;
	usart_gpio.Alternate = USARTx_TX_AF;
	LL_GPIO_Init(USARTx_TX_PORT, &usart_gpio);
}

void ESP_USART_LOWLEVEL_Conf(void)
{
	LL_USART_InitTypeDef esp_usart;
	
	eps_usart_gpio();
	
	USARTx_CLK_EN()	;
	esp_usart.BaudRate = USARTx_BAUDRATE;
	esp_usart.DataWidth = LL_USART_DATAWIDTH_8B;
	esp_usart.HardwareFlowControl = LL_USART_HWCONTROL_NONE;
	esp_usart.OverSampling = LL_USART_OVERSAMPLING_16;
	esp_usart.Parity = LL_USART_PARITY_NONE;
	esp_usart.StopBits = LL_USART_STOPBITS_1;
	esp_usart.TransferDirection = LL_USART_DIRECTION_TX_RX;
	
	LL_USART_Init(USARTx, &esp_usart);
	
	
	//Interrupt configure
	NVIC_SetPriority(USARTx_IRQn, 0);
	NVIC_EnableIRQ(USARTx_IRQn);
	
	/*Circular buffer cofig*/
	cb_init(&esp_recv_buffer);
}

void ESP_USART_Start(void)
{
	LL_USART_Enable(USARTx);
	cb_init(&esp_recv_buffer);
}

void ESP_USART_LOWLEVEL_Transmit(uint8_t* cmd)
{
	uint8_t i;
	LL_USART_DisableIT_RXNE(USARTx);
	for(i = 0; cmd[i] != '\0'; ++i)
	{
		LL_USART_TransmitData8(USARTx, cmd[i]);
		while(LL_USART_IsActiveFlag_TXE(USARTx) == RESET);
	}
	LL_USART_EnableIT_RXNE(USARTx);
}



uint8_t ESP_USART_LOWLEVEL_Recv(uint8_t* d, uint8_t idx)
{
	uint8_t tmp = 1;
	
	cb_pop(&esp_recv_buffer, &tmp);
	if(tmp != 1)
	{
		d[idx] = tmp;
		return 0;
	}
	return 1;
	
}


void USARTx_Callback()
{
	uint8_t tmp;
	if(LL_USART_IsActiveFlag_RXNE(USARTx) == SET)
	{
		tmp = LL_USART_ReceiveData8(USARTx);
		cb_put(&esp_recv_buffer, tmp);
	}
}
