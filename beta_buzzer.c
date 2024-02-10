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


#include "stm32l1xx_ll_usart.h"
#include "stm32l1xx_ll_lcd.h"
#include "stm32l152_glass_lcd.h"

// Key ???? 06 ????????????????? 05 ??? ??? 05 ????????????? 04
#define E_O6					(uint16_t)569
#define C_06					(uint16_t)523
#define Cx_06					(uint16_t)554
#define D_06					(uint16_t)587
#define Ex_06					(uint16_t)569
#define F_06					(uint16_t)698
#define Fx_06					(uint16_t)739
#define G_06					(uint16_t)784
#define Ax_06					(uint16_t)830
#define A_06					(uint16_t)880
#define Bx_06					(uint16_t)932
#define B_06					(uint16_t)987
#define E_O5					(uint16_t)329
#define C_05					(uint16_t)261
#define Cx_05					(uint16_t)277
#define D_05					(uint16_t)293
#define Ex_05					(uint16_t)311
#define F_05					(uint16_t)349
#define Fx_05					(uint16_t)369
#define G_05					(uint16_t)392
#define Ax_05					(uint16_t)415
#define A_05					(uint16_t)440
#define Bx_05					(uint16_t)466
#define B_05					(uint16_t)493
#define MUTE					(uint16_t) 100000
#define S_MUTE					(uint16_t) 1000000

/*for 10ms update event*/
#define TIMx_PSC			2 

/*Macro function for ARR calculation*/
#define ARR_CALCULATE(N) ((32000000) / ((TIMx_PSC) * (N)))

/*Function Prototype*/

void TIM_BASE_Config(uint16_t);
void TIM_OC_GPIO_Config(void);
void TIM_OC_Config(uint16_t);
void TIM_BASE_DurationConfig(uint16_t);

//array for music note
/*uint16_t Note[] = {E_O5,E_O5,G_05,C_05,C_05,MUTE,MUTE,A_05,A_05,C_06,F_05,F_05,MUTE,MUTE,B_05,S_MUTE,B_05,S_MUTE,C_06,S_MUTE,D_06,S_MUTE,C_06,C_06,C_06,C_06,
                   E_O5,E_O5,G_05,C_05,C_05,MUTE,MUTE,A_05,A_05,C_06,F_05,F_05,MUTE,MUTE,A_05,S_MUTE,B_05,S_MUTE,G_05,S_MUTE,A_05,S_MUTE,B_05,S_MUTE,D_06,S_MUTE,C_06,C_06,C_06,C_06};
*/
uint16_t Note[] = {E_O5,E_O5,MUTE,MUTE};
int i =0; // position in array

void SystemClock_Config(void);

int main()
{
 	SystemClock_Config();
	DWT_Init();
	
	TIM_OC_Config(ARR_CALCULATE(E_O5)); //call function
	TIM_BASE_DurationConfig(10); //cal function

	while(1)
	{
		
		
		
			if(LL_TIM_IsActiveFlag_UPDATE(TIM2) == SET) //if cause interrupt
			{
				LL_TIM_ClearFlag_UPDATE(TIM2); //clear interrupt
				TIM_BASE_DurationConfig(Note[i]==S_MUTE?10:200);
				LL_TIM_SetAutoReload(TIM4, ARR_CALCULATE((sizeof(Note)/sizeof(Note[i]))>i?Note[i++]:MUTE+(i=0))); //Change ARR of Timer PWM
				LL_TIM_SetCounter(TIM2, 0);
			}
		
		
		
			DWT_Delay(1000);
		
	}
}

void TIM_BASE_DurationConfig(uint16_t ARR) 
{
	LL_TIM_InitTypeDef timbase_initstructure;
	
	LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM2);
	//Time-base configure
	timbase_initstructure.ClockDivision = LL_TIM_CLOCKDIVISION_DIV1;
	timbase_initstructure.CounterMode = LL_TIM_COUNTERMODE_UP;
	timbase_initstructure.Autoreload = ARR - 1;
	timbase_initstructure.Prescaler =  32000 - 1;
	LL_TIM_Init(TIM2, &timbase_initstructure);
	
	LL_TIM_EnableCounter(TIM2); 
	LL_TIM_ClearFlag_UPDATE(TIM2); //Force clear update flag
}

void TIM_BASE_Config(uint16_t ARR)
{
	LL_TIM_InitTypeDef timbase_initstructure;
	
	LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM4);
	//Time-base configure
	timbase_initstructure.ClockDivision = LL_TIM_CLOCKDIVISION_DIV1;
	timbase_initstructure.CounterMode = LL_TIM_COUNTERMODE_UP;
	timbase_initstructure.Autoreload = ARR - 1;
	timbase_initstructure.Prescaler =  TIMx_PSC- 1;
	LL_TIM_Init(TIM4, &timbase_initstructure);
	
	LL_TIM_EnableCounter(TIM4); 
}


void TIM_OC_GPIO_Config(void)
{
	LL_GPIO_InitTypeDef gpio_initstructure;
	
	LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOB);
	
	gpio_initstructure.Mode = LL_GPIO_MODE_ALTERNATE;
	gpio_initstructure.Alternate = LL_GPIO_AF_2;
	gpio_initstructure.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
	gpio_initstructure.Pin = LL_GPIO_PIN_6;
	gpio_initstructure.Pull = LL_GPIO_PULL_NO;
	gpio_initstructure.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
	LL_GPIO_Init(GPIOB, &gpio_initstructure);
}

void TIM_OC_Config(uint16_t note) 
{
	LL_TIM_OC_InitTypeDef tim_oc_initstructure;
	
	TIM_OC_GPIO_Config();
	TIM_BASE_Config(note);
	
	tim_oc_initstructure.OCState = LL_TIM_OCSTATE_DISABLE;
	tim_oc_initstructure.OCMode = LL_TIM_OCMODE_PWM1;
	tim_oc_initstructure.OCPolarity = LL_TIM_OCPOLARITY_HIGH;
	tim_oc_initstructure.CompareValue = LL_TIM_GetAutoReload(TIM4) / 2;
	LL_TIM_OC_Init(TIM4, LL_TIM_CHANNEL_CH1, &tim_oc_initstructure);
	/*Interrupt Configure*/
	NVIC_SetPriority(TIM4_IRQn, 1);
	NVIC_EnableIRQ(TIM4_IRQn);
	LL_TIM_EnableIT_CC1(TIM4);
	
	/*Start Output Compare in PWM Mode*/
	LL_TIM_CC_EnableChannel(TIM4, LL_TIM_CHANNEL_CH1);
	LL_TIM_EnableCounter(TIM4);
}

void TIM4_IRQHandler(void)
{
	if(LL_TIM_IsActiveFlag_CC1(TIM4) == SET)
	{
		LL_TIM_ClearFlag_CC1(TIM4);
	}
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
