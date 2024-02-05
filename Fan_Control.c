/* Includes ------------------------------------------------------------------*/
#include "stm32l1xx.h"
#include "stm32l1xx_ll_system.h"
#include "stm32l1xx_ll_bus.h"
#include "stm32l1xx_ll_utils.h"
#include "stm32l1xx_ll_rcc.h"
#include "stm32l1xx_ll_pwr.h"
#include "stm32l1xx_ll_gpio.h"
#include "stm32l1xx_ll_usart.h"
#include "stm32l1xx_ll_lcd.h"
#include "stm32l152_glass_lcd.h"
#include "stm32l1xx_ll_tim.h"
#include "stm32l1xx_ll_exti.h"
#include "stdio.h"

void SystemClock_Config(void);
void TIM_BASE_Config(void);
void TIM_OC_GPIO_Config(void);
void TIM_OC_Config(void);
void PA3_Button_Config(void); 
void GPIO_Config(void);
void Control_Fan(void);

int cnt = 101;
char disp_str[7];

int main(void)
{
  SystemClock_Config();
	LCD_GLASS_Init();

	PA3_Button_Config();
	
	GPIO_Config();
	
	TIM_OC_Config();
 
	while(1)
	{
		Control_Fan();
		LL_LCD_Clear(); //Clear string on LCD
		sprintf(disp_str,"   %d",cnt); //print cnt value(%duty cycle)
		LCD_GLASS_DisplayString((uint8_t*)disp_str); //Show cnt value(%duty cycle) on LCD
	}
}

void Control_Fan(void)
{
	if(LL_GPIO_IsInputPinSet(GPIOA, LL_GPIO_PIN_3) == 0)
	{
		if(cnt <= 100) //If cnt >= 1 set ARR = cnt - 20
		{
			cnt += 34;
			LL_TIM_OC_SetCompareCH2(TIM3,cnt);
		}
		else	//If cnt < 1 set ARR = 100
		{
			cnt = 0;
			LL_TIM_OC_SetCompareCH2(TIM3,cnt);
	
		}
	}
	
	LL_mDelay(100);
	
	//Set DC motor controller turn left function
	LL_GPIO_SetOutputPin(GPIOB, LL_GPIO_PIN_7);
	LL_GPIO_SetOutputPin(GPIOB, LL_GPIO_PIN_5);
	LL_GPIO_ResetOutputPin(GPIOB, LL_GPIO_PIN_4);
}

void PA3_Button_Config(void)
{
	LL_GPIO_InitTypeDef gpio_initstructure;
	
	LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA);
	
	gpio_initstructure.Mode = LL_GPIO_MODE_INPUT;
	gpio_initstructure.Pin =  LL_GPIO_PIN_3;
	gpio_initstructure.Pull = LL_GPIO_PULL_NO;
	gpio_initstructure.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
	
	LL_GPIO_Init(GPIOA, &gpio_initstructure);
}


void GPIO_Config(void)
{
	LL_GPIO_InitTypeDef gpio_initstructure;
	
	LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOB);
	
	gpio_initstructure.Mode = LL_GPIO_MODE_OUTPUT;
	gpio_initstructure.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
	gpio_initstructure.Pin =  LL_GPIO_PIN_5 | LL_GPIO_PIN_7;
	gpio_initstructure.Pull = LL_GPIO_PULL_NO;
	gpio_initstructure.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
	LL_GPIO_Init(GPIOB, &gpio_initstructure);
}

void TIM_BASE_Config(void)
{
	LL_TIM_InitTypeDef timbase_initstructure;
	
	LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM3);
	//TIM3 configure
	timbase_initstructure.ClockDivision = LL_TIM_CLOCKDIVISION_DIV2;
	timbase_initstructure.CounterMode = LL_TIM_COUNTERMODE_UP;
	timbase_initstructure.Autoreload = 100 - 1;
	timbase_initstructure.Prescaler =  32000- 1;
	LL_TIM_Init(TIM3, &timbase_initstructure);
	
	LL_TIM_EnableCounter(TIM3); 
}


void TIM_OC_GPIO_Config(void)
{
	LL_GPIO_InitTypeDef gpio_initstructure;
	//Config PB5 in alternate mode for TIM3 CH2 output

	LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOB);
	
	gpio_initstructure.Mode = LL_GPIO_MODE_ALTERNATE;
	gpio_initstructure.Alternate = LL_GPIO_AF_2;
	gpio_initstructure.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
	gpio_initstructure.Pin = LL_GPIO_PIN_5;
	gpio_initstructure.Pull = LL_GPIO_PULL_NO;
	gpio_initstructure.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
	LL_GPIO_Init(GPIOB, &gpio_initstructure);
}

void TIM_OC_Config()
{
	LL_TIM_OC_InitTypeDef tim_oc_initstructure;
	
	TIM_OC_GPIO_Config();
	TIM_BASE_Config();
	//Config TIM3 CH2 output compare in PWM mode
	tim_oc_initstructure.OCState = LL_TIM_OCSTATE_DISABLE;
	tim_oc_initstructure.OCMode = LL_TIM_OCMODE_PWM1;
	tim_oc_initstructure.OCPolarity = LL_TIM_OCPOLARITY_HIGH;
	tim_oc_initstructure.CompareValue = 100;
	LL_TIM_OC_Init(TIM3, LL_TIM_CHANNEL_CH2, &tim_oc_initstructure);

	
	/*Start Output Compare in PWM Mode*/
	LL_TIM_CC_EnableChannel(TIM3, LL_TIM_CHANNEL_CH2);
	LL_TIM_EnableCounter(TIM3);
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

