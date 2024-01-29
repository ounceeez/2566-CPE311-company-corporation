/*Base register adddress header file*/
#include "stm32l1xx.h"
/*Library related header files*/
#include "stm32l1xx_ll_gpio.h"
#include "stm32l1xx_ll_pwr.h"
#include "stm32l1xx_ll_rcc.h"
#include "stm32l1xx_ll_bus.h"
#include "stm32l1xx_ll_utils.h"
#include "stm32l1xx_ll_system.h"
#include "stm32l1xx_ll_tim.h"
#include "stdio.h"
#include "math.h"
#include "dwt_delay.h"

/*Function Prototype*/
void SystemClock_Config(void);
void OW_WriteBit(uint8_t d);
uint8_t OW_ReadBit(void);
void DS1820_GPIO_Configure(void);
uint8_t DS1820_ResetPulse(void);
void seg_config(void);

void OW_Master(void);
void OW_Slave(void);
void OW_WriteByte(uint8_t data);
uint16_t OW_ReadByte(void);

uint8_t LSB, MSB;
uint16_t temp_data;
float temp;
char disp_str[4];
uint16_t seg[10] = {0x7C04, 0x0C00, 0xB404, 0x9C04, 0xCC00, 0xD804, 0xF804, 0x0C04, 0xFC04, 0xDC04}; //0 1 2 3 4 5 6 7 8 9
uint32_t digit[4] = {LL_GPIO_PIN_0 , LL_GPIO_PIN_1 , LL_GPIO_PIN_2 , LL_GPIO_PIN_3};

int main()
{
	SystemClock_Config();
	DWT_Init();
	DS1820_GPIO_Configure();
	seg_config();

	while(1)
	{
		//Send reset pulse
		DS1820_ResetPulse();
		//Send 'Skip Rom (0xCC)' command
		OW_WriteByte(0xCC);
		//Send 'Temp Convert (0x44)' command
		OW_WriteByte(0x44);
		//Delay at least 200ms (typical conversion time)
		DWT_Delay(1000);
		//Send reset pulse
		DS1820_ResetPulse();
		//Send 'Skip Rom (0xCC)' command
		OW_WriteByte(0xCC);
		//Send 'Read Scractpad (0xBE)' command
		OW_WriteByte(0xBE);
		
		//Read byte 1 (Temperature data in LSB)
		LSB = OW_ReadByte() & 0xff;
		//Read byte 2 (Temperature data in MSB)
		MSB = OW_ReadByte() & 0x0f;
		//Convert to readable floating point temperature
		temp = (temp_data = (LSB | (MSB << 8))) / 16.0;
		
		//change temp from float to char
		temp *= 100;		//Ex. change 23.45 to 2345
		/*
		temp[0] = '0' + ((int)REAL_TEMP /1000);
		temp[1] = '0' + ((int)REAL_TEMP %1000) /100;
		temp[2] = '0' + ((int)REAL_TEMP %100) /10;
		temp[3] = '0' + ((int)REAL_TEMP %10);
		*/
		sprintf(disp_str, "%d", (int)temp);		//Ex. change 2345 to "2345"
		
		// display 7 segment
		for(int k = 0; k < 4; k++)
		{
			// if-else for show (dp) on digit2
			if(k == 1)
				LL_GPIO_SetOutputPin(GPIOA, LL_GPIO_PIN_5);
			else 
				LL_GPIO_ResetOutputPin(GPIOA, LL_GPIO_PIN_5);
			
			LL_GPIO_ResetOutputPin(GPIOC, LL_GPIO_PIN_0 | LL_GPIO_PIN_1 | LL_GPIO_PIN_2 | LL_GPIO_PIN_3);
			LL_GPIO_ResetOutputPin(GPIOB, seg[8]);
			
			LL_GPIO_SetOutputPin(GPIOC, digit[k]);
			LL_GPIO_SetOutputPin(GPIOB, seg[(int)disp_str[k] - '0']);
			
			DWT_Delay(1000);
		}
	}
}

void OW_WriteByte(uint8_t data)
{
	int i = 0;
	while(i < 8)
	{
		OW_WriteBit(data & 1);
		data >>= 1;
		i++;
	}
}

uint16_t OW_ReadByte(void)
{
	uint8_t value = 0;
  for (int i = 0; i < 8; i++) 
	{
    int bit = OW_ReadBit();
    if (bit == 1)
      value |= (1 << i);
    DWT_Delay(60);
  }
  return value;
}

void OW_Master(void)
{
	LL_GPIO_SetPinMode(GPIOB,LL_GPIO_PIN_6 ,LL_GPIO_MODE_OUTPUT);
	LL_GPIO_SetPinPull(GPIOB,LL_GPIO_PIN_6 ,LL_GPIO_PULL_NO);
}

void OW_Slave(void)
{
	LL_GPIO_SetPinMode(GPIOB,LL_GPIO_PIN_6 ,LL_GPIO_MODE_INPUT);
	LL_GPIO_SetPinPull(GPIOB,LL_GPIO_PIN_6 ,LL_GPIO_PULL_UP);
}

void OW_WriteBit(uint8_t d)
{
	if(d == 1) //Write 1
	{
		OW_Master(); //uC occupies wire bus
		LL_GPIO_ResetOutputPin(GPIOB, LL_GPIO_PIN_6);
		DWT_Delay(1);
		OW_Slave(); //uC releases wire bus
		DWT_Delay(60);
	}
	else //Write 0
	{
		OW_Master(); //uC occupies wire bus
		DWT_Delay(60);
		OW_Slave(); //uC releases wire bus
	}
}

uint8_t OW_ReadBit(void)
{
	OW_Master();
	LL_GPIO_ResetOutputPin(GPIOB, LL_GPIO_PIN_6);
	DWT_Delay(2);
	OW_Slave();
	
	return LL_GPIO_IsInputPinSet(GPIOB, LL_GPIO_PIN_6);	
}


void DS1820_GPIO_Configure(void)
{
	LL_GPIO_InitTypeDef ds1820_io;
	
	LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOB);
	
	ds1820_io.Mode = LL_GPIO_MODE_OUTPUT;
	ds1820_io.Pin = LL_GPIO_PIN_6;
	ds1820_io.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
	ds1820_io.Pull = LL_GPIO_PULL_NO;
	ds1820_io.Speed = LL_GPIO_SPEED_FREQ_LOW;
	LL_GPIO_Init(GPIOB, &ds1820_io);
}

uint8_t DS1820_ResetPulse(void)
{	
	OW_Master();
	LL_GPIO_ResetOutputPin(GPIOB, LL_GPIO_PIN_6);
	DWT_Delay(480);
	OW_Slave();
	DWT_Delay(70);
	
	if(LL_GPIO_IsInputPinSet(GPIOB, LL_GPIO_PIN_6) == 0)
	{
		while(LL_GPIO_IsInputPinSet(GPIOB, LL_GPIO_PIN_6) == 0);
		return 0;
	}
	else
	{
		while(LL_GPIO_IsInputPinSet(GPIOB, LL_GPIO_PIN_6) == 1);
		return 1;
	}
	DWT_Delay(410);
}

void seg_config(void)	//Set ALL PIN for 7segment
{
	LL_GPIO_InitTypeDef GPIO_set;
	LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA);
	LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOB);
	LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOC);
	GPIO_set.Mode = LL_GPIO_MODE_OUTPUT;
	GPIO_set.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
	GPIO_set.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_set.Pull = LL_GPIO_PULL_NO;
	GPIO_set.Pin = LL_GPIO_PIN_5;
	LL_GPIO_Init(GPIOA, &GPIO_set);
	GPIO_set.Pin = LL_GPIO_PIN_2 | LL_GPIO_PIN_10 | LL_GPIO_PIN_11 | LL_GPIO_PIN_12 | LL_GPIO_PIN_13 | LL_GPIO_PIN_14 | LL_GPIO_PIN_15;
	LL_GPIO_Init(GPIOB, &GPIO_set);
	GPIO_set.Pin = LL_GPIO_PIN_0 | LL_GPIO_PIN_1 | LL_GPIO_PIN_2 | LL_GPIO_PIN_3;
	LL_GPIO_Init(GPIOC, &GPIO_set);
}

void SystemClock_Config(void)
{
  /* Enable ACC64 access and set FLASH latency */ 
  LL_FLASH_Enable64bitAccess();; 
  LL_FLASH_SetLatency(LL_FLASH_LATENCY_1);

  /* Set Voltage scale1 as MCU will run at 32MHz */
  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_PWR);
  LL_PWR_SetRegulVoltageScaling(LL_PWR_REGU_VOLTAGE_SCALE1);
  
  /* Poll VOSF bit of in PWR_CSR. Wait until it is reset to 0 */
  while (LL_PWR_IsActiveFlag_VOSF() != 0)
  {
  };
  
  /* Enable HSI if not already activated*/
  if (LL_RCC_HSI_IsReady() == 0)
  {
    /* HSI configuration and activation */
    LL_RCC_HSI_Enable();
    while(LL_RCC_HSI_IsReady() != 1)
    {
    };
  }
  
	
  /* Main PLL configuration and activation */
  LL_RCC_PLL_ConfigDomain_SYS(LL_RCC_PLLSOURCE_HSI, LL_RCC_PLL_MUL_6, LL_RCC_PLL_DIV_3);

  LL_RCC_PLL_Enable();
  while(LL_RCC_PLL_IsReady() != 1)
  {
  };
  
  /* Sysclk activation on the main PLL */
  LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
  LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL);
  while(LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL)
  {
  };
  
  /* Set APB1 & APB2 prescaler*/
  LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_1);
  LL_RCC_SetAPB2Prescaler(LL_RCC_APB2_DIV_1);

  /* Set systick to 1ms in using frequency set to 32MHz                             */
  /* This frequency can be calculated through LL RCC macro                          */
  /* ex: __LL_RCC_CALC_PLLCLK_FREQ (HSI_VALUE, LL_RCC_PLL_MUL_6, LL_RCC_PLL_DIV_3); */
  LL_Init1msTick(32000000);
  
  /* Update CMSIS variable (which can be updated also through SystemCoreClockUpdate function) */
  LL_SetSystemCoreClock(32000000);
}
