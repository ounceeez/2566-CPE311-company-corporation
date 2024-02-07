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
#include "stm32l1xx_ll_exti.h"

void SystemClock_Config(void);
void IR_GPIO_Config(void);
void LED_Config(void);
void TIMBase_Config(void);
void CountPeople(void);
void GPIO_Config(void);
void PA10_EXTI_Config(void);

int IRout = 0, IRin = 0 , IRout_tim = 0, IRin_tim = 0;
int count_people = 0, cnt_tim = 0;

int jump1, jump2, jump3;
int cnt;

int main()
{
	SystemClock_Config();
	IR_GPIO_Config();
	LED_Config();
	TIMBase_Config();
	PA10_EXTI_Config();
	
	while(1)
	{
		LL_GPIO_ResetOutputPin(GPIOA, LL_GPIO_PIN_8);
		LL_GPIO_ResetOutputPin(GPIOA, LL_GPIO_PIN_9);
		CountPeople();
		if(count_people >= 1){
			LL_GPIO_SetOutputPin(GPIOA, LL_GPIO_PIN_8);
			LL_GPIO_SetOutputPin(GPIOA, LL_GPIO_PIN_9);
		}
		else{
			LL_GPIO_ResetOutputPin(GPIOA, LL_GPIO_PIN_8);
			LL_GPIO_ResetOutputPin(GPIOA, LL_GPIO_PIN_9);
		}
	}
}

void CountPeople(void)
{	
		LL_TIM_EnableCounter(TIM2);  //Enable counter of Timer2
		
		IRout = LL_GPIO_IsInputPinSet(GPIOA, LL_GPIO_PIN_1);
		IRin = LL_GPIO_IsInputPinSet(GPIOA, LL_GPIO_PIN_2);
		 
		cnt = LL_TIM_GetCounter(TIM2);
		cnt_tim = 0;
		
		if(IRout == 0){
			jump1:
			IRout_tim = LL_TIM_GetCounter(TIM2);
			//add
			while(cnt <= (IRout_tim+5000))
			{
				cnt = LL_TIM_GetCounter(TIM2);
				
				IRout = LL_GPIO_IsInputPinSet(GPIOA, LL_GPIO_PIN_1);
				IRin = LL_GPIO_IsInputPinSet(GPIOA, LL_GPIO_PIN_2);
				
				if(IRout == 0)
					goto jump1;
				
				if(IRin == 0){
					IRin_tim = LL_TIM_GetCounter(TIM2);
					LL_mDelay(100);
					count_people += 1;
					LL_mDelay(500);
					goto jump3;
				}
			}
			LL_TIM_SetCounter(TIM2, 0);
		}
		
		if(IRin == 0){
			jump2:
			IRin_tim = LL_TIM_GetCounter(TIM2);
			//add
			while(cnt <= (IRin_tim+5000))
			{
				cnt = LL_TIM_GetCounter(TIM2);
				
				IRout = LL_GPIO_IsInputPinSet(GPIOA, LL_GPIO_PIN_1);
				IRin = LL_GPIO_IsInputPinSet(GPIOA, LL_GPIO_PIN_2);
				
				if(IRin == 0)
					goto jump2;
				
				if(IRout == 0){
					IRout_tim = LL_TIM_GetCounter(TIM2);
					LL_mDelay(100);
					count_people -= 1;
					LL_mDelay(500);
					goto jump3;
				}
			}
			LL_TIM_SetCounter(TIM2, 0);
		}
		
		jump3:
		
		if(IRin_tim != 0 && IRout_tim != 0){
			LL_TIM_DisableCounter(TIM2); //Disable counter of Timer2
			LL_TIM_SetCounter(TIM2, 0); //Set counter Timer2 is 0
			IRin_tim = 0;
			IRout_tim = 0;
		}
}

void TIMBase_Config(void)
{
	LL_TIM_InitTypeDef timbase_intstructure;  //Declare struct for Timer configure
	
	LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM2);  //Enable clock for Timer2
	
	timbase_intstructure.ClockDivision = LL_TIM_CLOCKDIVISION_DIV1;   //Set the clock division to division 1

	timbase_intstructure.CounterMode = LL_TIM_COUNTERMODE_UP;  //Set counter mode is up

	timbase_intstructure.Autoreload = 32000 - 1;  //Set ARR = 32000
	timbase_intstructure.Prescaler = 60000 - 1;  //Set PSC = 60000
	
	LL_TIM_Init(TIM2, &timbase_intstructure);  //Initialize Timer2
	//LL_TIM_EnableCounter(TIM2);  //Enable counter of Timer2
}


void LED_Config(void)
{
	LL_GPIO_InitTypeDef GPIO_InitStruct;  //Declare struct for GPIO configure
	
	LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA);  //Enable GPIOB clock

	//Configure GPIOB using struct
	GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;  //Set GPIO output mode
	GPIO_InitStruct.Pin = LL_GPIO_PIN_8;  //Configure PB6
	GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;  //Set output type
	GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;  //Disable internal pull resistor
	GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;  //Set output speed
	 
	LL_GPIO_Init(GPIOA, &GPIO_InitStruct);  //Write configure to GPIOB register
	
	GPIO_InitStruct.Pin = LL_GPIO_PIN_9;  //Configure PB7
	LL_GPIO_Init(GPIOA, &GPIO_InitStruct);  //Write configure to GPIOB register
}


void IR_GPIO_Config(void)
{
	LL_GPIO_InitTypeDef gpio_initstructure;
	
	LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA);
	
	gpio_initstructure.Mode = LL_GPIO_MODE_INPUT;
	gpio_initstructure.Pin =  LL_GPIO_PIN_1 | LL_GPIO_PIN_2;
	gpio_initstructure.Pull = LL_GPIO_PULL_NO;
	gpio_initstructure.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
	
	LL_GPIO_Init(GPIOA, &gpio_initstructure);
	
}

void GPIO_Config(void){
	LL_GPIO_InitTypeDef GPIO_InitStuct;
	LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA);
	
	//PIN PA10
	GPIO_InitStuct.Mode = LL_GPIO_MODE_INPUT;
	GPIO_InitStuct.Pin = LL_GPIO_PIN_10;
	GPIO_InitStuct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
	GPIO_InitStuct.Pull = LL_GPIO_PULL_NO;
	GPIO_InitStuct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
	LL_GPIO_Init(GPIOA, &GPIO_InitStuct);
}

void PA10_EXTI_Config(void){
	LL_EXTI_InitTypeDef PA10_EXTI_InitStruct;
	LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_SYSCFG);
	LL_SYSCFG_SetEXTISource(LL_SYSCFG_EXTI_PORTA, LL_SYSCFG_EXTI_LINE10);
	PA10_EXTI_InitStruct.Line_0_31 = LL_EXTI_LINE_10;
	PA10_EXTI_InitStruct.LineCommand = ENABLE;
	PA10_EXTI_InitStruct.Mode = LL_EXTI_MODE_IT;
	PA10_EXTI_InitStruct.Trigger = LL_EXTI_TRIGGER_RISING;
	LL_EXTI_Init(&PA10_EXTI_InitStruct);
	//Setup NVIC
	NVIC_EnableIRQ((IRQn_Type) 47);
	NVIC_SetPriority((IRQn_Type)47,0);
}

void EXTI15_10_IRQHandler(void){
	if((EXTI->PR & (1<<10)) == 1)
	{
		LL_GPIO_SetOutputPin(GPIOA, LL_GPIO_PIN_8);
		LL_GPIO_SetOutputPin(GPIOA, LL_GPIO_PIN_9);
	}
	LL_EXTI_ClearFlag_0_31(LL_EXTI_LINE_10);
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
