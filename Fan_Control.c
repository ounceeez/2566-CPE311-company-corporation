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
void TIM_BASE_Config(void);
void TIM_OC_GPIO_Config(void);
void TIM_OC_Config(void);
void Config_Motor(void);
void PA0_EXTI_Config(void);
void GPIO_Config(void);

int p = 0;

int main(void)
{
	SystemClock_Config(); // Call Function SystemClock_Config
	Config_Motor(); // Call Function Config Motor
	GPIO_Config();
	PA0_EXTI_Config();
	TIM_OC_Config(); // Call Function TIM_OC_Config
	
	while(1)
	{
		
	}
}

void TIM_BASE_Config(void)
{
	LL_TIM_InitTypeDef timbase_initstructure;
	
	LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM3);
	//Time-base configure
	timbase_initstructure.ClockDivision = LL_TIM_CLOCKDIVISION_DIV2;
	timbase_initstructure.CounterMode = LL_TIM_COUNTERMODE_UP;
	timbase_initstructure.Autoreload = 100 - 1;
	timbase_initstructure.Prescaler =  32000 - 1;
	LL_TIM_Init(TIM3, &timbase_initstructure);
	
	LL_TIM_EnableCounter(TIM3); 
}
//Function Config Motor
void Config_Motor(void)
{
	LL_GPIO_InitTypeDef l293d_initstruct;
	//configure l293d
	LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOB);
	
	l293d_initstruct.Mode = LL_GPIO_MODE_OUTPUT;
	l293d_initstruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
	l293d_initstruct.Pull = LL_GPIO_PULL_NO;
	l293d_initstruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
	l293d_initstruct.Pin = LL_GPIO_PIN_3 | LL_GPIO_PIN_4 | LL_GPIO_PIN_5;
	LL_GPIO_Init(GPIOB, &l293d_initstruct);
}
//Function GPIO Config
void TIM_OC_GPIO_Config(void)
{
	LL_GPIO_InitTypeDef gpio_initstructure;
	
	LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOB);
	
	gpio_initstructure.Mode = LL_GPIO_MODE_ALTERNATE;
	gpio_initstructure.Alternate = LL_GPIO_AF_2;
	gpio_initstructure.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
	gpio_initstructure.Pin = LL_GPIO_PIN_4;
	gpio_initstructure.Pull = LL_GPIO_PULL_NO;
	gpio_initstructure.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
	LL_GPIO_Init(GPIOB, &gpio_initstructure);
}

void GPIO_Config(void){
    LL_GPIO_InitTypeDef GPIO_InitStuct;
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA);
    
    // Configure PA11
    GPIO_InitStuct.Mode = LL_GPIO_MODE_INPUT;
    GPIO_InitStuct.Pin = LL_GPIO_PIN_0; // Change pin to PA10
    GPIO_InitStuct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStuct.Pull = LL_GPIO_PULL_NO;
    GPIO_InitStuct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
    LL_GPIO_Init(GPIOA, &GPIO_InitStuct);
}

//Function TIM OC Config
void TIM_OC_Config()
{
	LL_TIM_OC_InitTypeDef tim_oc_initstructure;
	
	TIM_OC_GPIO_Config();
	TIM_BASE_Config();
	
	tim_oc_initstructure.OCState = LL_TIM_OCSTATE_DISABLE;
	tim_oc_initstructure.OCMode = LL_TIM_OCMODE_PWM1;
	tim_oc_initstructure.OCPolarity = LL_TIM_OCPOLARITY_HIGH;
	tim_oc_initstructure.CompareValue = p; // CCR
	LL_TIM_OC_Init(TIM3, LL_TIM_CHANNEL_CH1, &tim_oc_initstructure);
	
	/*Start Output Compare in PWM Mode*/
	LL_TIM_CC_EnableChannel(TIM3, LL_TIM_CHANNEL_CH1);
	LL_TIM_EnableCounter(TIM3);
}

void PA0_EXTI_Config(void) // EXTI Config for PA11
{
    LL_EXTI_InitTypeDef PA0_EXTI_InitStruct;
    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_SYSCFG); // Enable Bus Clock
    LL_SYSCFG_SetEXTISource(LL_SYSCFG_EXTI_PORTA, LL_SYSCFG_EXTI_LINE0); // Change EXTI line to LL_SYSCFG_EXTI_LINE11
    PA0_EXTI_InitStruct.Line_0_31 = LL_EXTI_LINE_0; // Change EXTI line to LL_EXTI_LINE_11
    PA0_EXTI_InitStruct.LineCommand = ENABLE;
    PA0_EXTI_InitStruct.Mode = LL_EXTI_MODE_IT;
    PA0_EXTI_InitStruct.Trigger = LL_EXTI_TRIGGER_RISING;
    LL_EXTI_Init(&PA0_EXTI_InitStruct);
    // Setup NVIC
    NVIC_EnableIRQ((IRQn_Type)6); // Use EXTI15_10_IRQn for lines 10 to 15
    NVIC_SetPriority((IRQn_Type)6, 0);
}

void EXTI0_IRQHandler(void) // EXTI Handler for lines 10 to 15
{
    if(LL_EXTI_IsActiveFlag_0_31(LL_EXTI_LINE_0) == SET)
		{ // Change EXTI line to LL_EXTI_LINE_11
			if(p>=0 && p<100)
			{
				p += 25;
				LL_TIM_OC_SetCompareCH2(TIM3, p);
			}
			else
			{
				p=0;
				LL_TIM_OC_SetCompareCH2(TIM3, p);
			}
			//TIM_OC_Config();
    }
    LL_EXTI_ClearFlag_0_31(LL_EXTI_LINE_0); // Change EXTI line to LL_EXTI_LINE_11
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
