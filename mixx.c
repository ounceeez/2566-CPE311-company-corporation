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

void TIM_BASE_Config_buzzer(uint16_t);
void TIM_OC_GPIO_Config_buzzer(void);
void TIM_OC_Config_buzzer(uint16_t);
void TIM_BASE_DurationConfig(uint16_t);

//array for music note
/*uint16_t Note[] = {E_O5,E_O5,G_05,C_05,C_05,MUTE,MUTE,A_05,A_05,C_06,F_05,F_05,MUTE,MUTE,B_05,S_MUTE,B_05,S_MUTE,C_06,S_MUTE,D_06,S_MUTE,C_06,C_06,C_06,C_06,
                   E_O5,E_O5,G_05,C_05,C_05,MUTE,MUTE,A_05,A_05,C_06,F_05,F_05,MUTE,MUTE,A_05,S_MUTE,B_05,S_MUTE,G_05,S_MUTE,A_05,S_MUTE,B_05,S_MUTE,D_06,S_MUTE,C_06,C_06,C_06,C_06};
*/
uint16_t Note[] = {E_O5,E_O5,MUTE,MUTE};
int i =0; // position in array

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

//********************************************** IR ************************************************

void IR_GPIO_Config(void);
void LED_Config(void);
void TIMBase_Config(void);
void CountPeople(void);
void GPIO_Config(void);
void PA8_EXTI_Config(void);

int IRout = 0, IRin = 0 , IRout_tim = 0, IRin_tim = 0;
int count_people = 0, cnt_tim = 0;

int jump1, jump2, jump3, jump4;
int cnt;
int sw_count = 0;

//********************************************** FAN ************************************************

void SystemClock_Config(void);
void TIM_BASE_Config(void);
void TIM_OC_GPIO_Config(void);
void TIM_OC_Config(void);
void Config_Motor(void);
void PA11_EXTI_Config(void);
int f_count = 0;

int p = 0;

int main()
{
 	SystemClock_Config();
	IR_GPIO_Config();
	DWT_Init();
	DS1820_GPIO_Configure();
	seg_config();
	//TIM_OC_Config_buzzer(ARR_CALCULATE(100000));
	//TIM_BASE_DurationConfig(10); //cal function
	
	Config_Motor();
	LED_Config();
	TIMBase_Config();
	GPIO_Config();
	PA8_EXTI_Config();
	PA11_EXTI_Config();
	TIM_OC_Config();
	
	while(1)
	{
		//LL_TIM_EnableCounter(TIM2);
		CountPeople();
		if(count_people >= 1){
			
			if(sw_count == 1){
				LL_GPIO_SetOutputPin(GPIOA, LL_GPIO_PIN_9);
				LL_GPIO_SetOutputPin(GPIOA, LL_GPIO_PIN_10);
			}
			else{
			LL_GPIO_ResetOutputPin(GPIOA, LL_GPIO_PIN_9);
				LL_GPIO_ResetOutputPin(GPIOA, LL_GPIO_PIN_10);
			}
				
			if(f_count == 1){
				LL_GPIO_ResetOutputPin(GPIOB, LL_GPIO_PIN_3);
			}
			else{
				LL_GPIO_SetOutputPin(GPIOB, LL_GPIO_PIN_3);
			LL_GPIO_SetOutputPin(GPIOB, LL_GPIO_PIN_5);
			LL_GPIO_ResetOutputPin(GPIOB, LL_GPIO_PIN_4);
			
			}
			//LL_GPIO_SetOutputPin(GPIOB, LL_GPIO_PIN_3);
			//LL_GPIO_SetOutputPin(GPIOB, LL_GPIO_PIN_5);
			//LL_GPIO_ResetOutputPin(GPIOB, LL_GPIO_PIN_4);
		}
		if(count_people == 0){
			LL_GPIO_SetOutputPin(GPIOA, LL_GPIO_PIN_8);
			LL_GPIO_SetOutputPin(GPIOA, LL_GPIO_PIN_9);
			sw_count = 0;
			
			LL_GPIO_ResetOutputPin(GPIOB, LL_GPIO_PIN_3);
		}
		
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
		
		if(temp >= 2600.0 && count_people == 0){
			LL_GPIO_SetOutputPin(GPIOB, LL_GPIO_PIN_3);
			LL_GPIO_SetOutputPin(GPIOB, LL_GPIO_PIN_5);
			LL_GPIO_ResetOutputPin(GPIOB, LL_GPIO_PIN_4);
			
			TIM_BASE_DurationConfig(Note[i]==S_MUTE?10:200);
			LL_TIM_SetAutoReload(TIM4, ARR_CALCULATE((sizeof(Note)/sizeof(Note[i]))>i?Note[i++]:MUTE+(i=0))); //Change ARR of Timer PWM
			
			LL_mDelay(150);
			
			TIM_OC_Config_buzzer(ARR_CALCULATE(70000));
			
		}
		
		

		
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
			//LL_GPIO_SetOutputPin(GPIOB, seg[7]);
			
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


void OW_WriteBit(uint8_t d)
{
	if(d == 1) //Write 1
	{
		OW_Master(); //uC occupies wire bus
		LL_GPIO_ResetOutputPin(GPIOB, LL_GPIO_PIN_7);
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
	LL_GPIO_ResetOutputPin(GPIOB, LL_GPIO_PIN_7);
	DWT_Delay(2);
	OW_Slave();
	
	return LL_GPIO_IsInputPinSet(GPIOB, LL_GPIO_PIN_7);	
}


// Change pin definition from PB6 to PB7
void DS1820_GPIO_Configure(void)
{
    LL_GPIO_InitTypeDef ds1820_io;
    
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOB);
    
    ds1820_io.Mode = LL_GPIO_MODE_OUTPUT;
    ds1820_io.Pin = LL_GPIO_PIN_7; // Change from LL_GPIO_PIN_6 to LL_GPIO_PIN_7
    ds1820_io.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    ds1820_io.Pull = LL_GPIO_PULL_NO;
    ds1820_io.Speed = LL_GPIO_SPEED_FREQ_LOW;
    LL_GPIO_Init(GPIOB, &ds1820_io);
}

// Update the Master and Slave functions accordingly
void OW_Master(void)
{
    LL_GPIO_SetPinMode(GPIOB, LL_GPIO_PIN_7, LL_GPIO_MODE_OUTPUT); // Change from PB6 to PB7
    LL_GPIO_SetPinPull(GPIOB, LL_GPIO_PIN_7, LL_GPIO_PULL_NO); // Change from PB6 to PB7
}

void OW_Slave(void)
{
    LL_GPIO_SetPinMode(GPIOB, LL_GPIO_PIN_7, LL_GPIO_MODE_INPUT); // Change from PB6 to PB7
    LL_GPIO_SetPinPull(GPIOB, LL_GPIO_PIN_7, LL_GPIO_PULL_UP); // Change from PB6 to PB7
}

// Adjust the Read and Write functions as needed


uint8_t DS1820_ResetPulse(void)
{	
	OW_Master();
	LL_GPIO_ResetOutputPin(GPIOB, LL_GPIO_PIN_7);
	DWT_Delay(480);
	OW_Slave();
	DWT_Delay(70);
	
	if(LL_GPIO_IsInputPinSet(GPIOB, LL_GPIO_PIN_7) == 0)
	{
		while(LL_GPIO_IsInputPinSet(GPIOB, LL_GPIO_PIN_7) == 0);
		return 0;
	}
	else
	{
		while(LL_GPIO_IsInputPinSet(GPIOB, LL_GPIO_PIN_7) == 1);
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

void TIM_BASE_Config_buzzer(uint16_t ARR)
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


void TIM_OC_GPIO_Config_buzzer(void)
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

void TIM_OC_Config_buzzer(uint16_t note) 
{
	LL_TIM_OC_InitTypeDef tim_oc_initstructure;
	
	TIM_OC_GPIO_Config_buzzer();
	TIM_BASE_Config_buzzer(note);
	
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

void CountPeople(void)
{	
		//LL_TIM_EnableCounter(TIM2);  //Enable counter of Timer2
		
		IRout = LL_GPIO_IsInputPinSet(GPIOA, LL_GPIO_PIN_1);
		IRin = LL_GPIO_IsInputPinSet(GPIOA, LL_GPIO_PIN_4);
		 
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
				IRin = LL_GPIO_IsInputPinSet(GPIOA, LL_GPIO_PIN_4);
				
				if(IRout == 0)
					goto jump1;
				
				if(IRin == 0){
					IRin_tim = LL_TIM_GetCounter(TIM2);
					LL_mDelay(100);
					count_people += 1;
					LL_TIM_DisableCounter(TIM2); //Disable counter of Timer2
					LL_TIM_SetCounter(TIM2, 0);
					
					LL_TIM_ClearFlag_UPDATE(TIM2); //clear interrupt
					TIM_OC_Config_buzzer(ARR_CALCULATE(10));
					TIM_BASE_DurationConfig(10); //cal function
					
					
				
					LL_mDelay(500);
					
					
					
					LL_TIM_ClearFlag_UPDATE(TIM2); //clear interrupt
				  TIM_OC_Config_buzzer(ARR_CALCULATE(79999));
					TIM_BASE_DurationConfig(10); //cal function
					
					
					
					
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
				IRin = LL_GPIO_IsInputPinSet(GPIOA, LL_GPIO_PIN_4);
				
				if(IRin == 0)
					goto jump2;
				
				if(IRout == 0){
					IRout_tim = LL_TIM_GetCounter(TIM2);
					LL_mDelay(100);
					count_people -= 1;
					
					LL_TIM_ClearFlag_UPDATE(TIM2); //clear interrupt
					TIM_OC_Config_buzzer(ARR_CALCULATE(5000));
					TIM_BASE_DurationConfig(10); //cal function
					
					
				
					LL_mDelay(500);
					
					
					
					LL_TIM_ClearFlag_UPDATE(TIM2); //clear interrupt
				  TIM_OC_Config_buzzer(ARR_CALCULATE(79999));
					TIM_BASE_DurationConfig(10); //cal function
					
					if(count_people < 0)
						count_people = 0;
					LL_mDelay(500);
					goto jump3;
				}
			}
			LL_TIM_SetCounter(TIM2, 0);
		}
		
		jump3:
		
		if(IRin_tim != 0 && IRout_tim != 0){
			//LL_TIM_DisableCounter(TIM2); //Disable counter of Timer2
			//LL_TIM_SetCounter(TIM2, 0); //Set counter Timer2 is 0
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
	LL_TIM_EnableCounter(TIM2);  //Enable counter of Timer2
}


void LED_Config(void)
{
	LL_GPIO_InitTypeDef GPIO_InitStruct;  //Declare struct for GPIO configure
	
	LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA);  //Enable GPIOB clock

	//Configure GPIOB using struct
	GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;  //Set GPIO output mode
	GPIO_InitStruct.Pin = LL_GPIO_PIN_9;  //Configure PB6
	GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;  //Set output type
	GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;  //Disable internal pull resistor
	GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;  //Set output speed
	 
	LL_GPIO_Init(GPIOA, &GPIO_InitStruct);  //Write configure to GPIOB register
	
	GPIO_InitStruct.Pin = LL_GPIO_PIN_10;  //Configure PB7
	LL_GPIO_Init(GPIOA, &GPIO_InitStruct);  //Write configure to GPIOB register
}


void IR_GPIO_Config(void)
{
	LL_GPIO_InitTypeDef gpio_initstructure;
	
	LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA);
	
	gpio_initstructure.Mode = LL_GPIO_MODE_INPUT;
	gpio_initstructure.Pin =  LL_GPIO_PIN_1 | LL_GPIO_PIN_4;
	gpio_initstructure.Pull = LL_GPIO_PULL_NO;
	gpio_initstructure.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
	
	LL_GPIO_Init(GPIOA, &gpio_initstructure);
	
}

void GPIO_Config(void){
    LL_GPIO_InitTypeDef GPIO_InitStuct;
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA);
    
    // Configure PA10 instead of PA0
    GPIO_InitStuct.Mode = LL_GPIO_MODE_INPUT;
    GPIO_InitStuct.Pin = LL_GPIO_PIN_10 | LL_GPIO_PIN_11; // Change pin to PA10
    GPIO_InitStuct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStuct.Pull = LL_GPIO_PULL_NO;
    GPIO_InitStuct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
    LL_GPIO_Init(GPIOA, &GPIO_InitStuct);
}

void PA8_EXTI_Config(void){
    LL_EXTI_InitTypeDef PA10_EXTI_InitStruct;
    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_SYSCFG);
    LL_SYSCFG_SetEXTISource(LL_SYSCFG_EXTI_PORTA, LL_SYSCFG_EXTI_LINE8); // Change EXTI line to LL_SYSCFG_EXTI_LINE8
    PA10_EXTI_InitStruct.Line_0_31 = LL_EXTI_LINE_8; // Change EXTI line to LL_EXTI_LINE_8
    PA10_EXTI_InitStruct.LineCommand = ENABLE;
    PA10_EXTI_InitStruct.Mode = LL_EXTI_MODE_IT;
    PA10_EXTI_InitStruct.Trigger = LL_EXTI_TRIGGER_FALLING;
    LL_EXTI_Init(&PA10_EXTI_InitStruct);
    // Setup NVIC
    NVIC_EnableIRQ(EXTI9_5_IRQn); // Change IRQ to handle EXTI line 5 to EXTI line 9
    NVIC_SetPriority(EXTI9_5_IRQn, 0);
}

void EXTI9_5_IRQHandler(void){
    if(LL_EXTI_IsActiveFlag_0_31(LL_EXTI_LINE_8) == SET){ // Change EXTI line to LL_EXTI_LINE_8
        // Handle interrupt for PA8
			LL_TIM_EnableCounter(TIM2);
			if(sw_count == 0)
		{
			LL_GPIO_SetOutputPin(GPIOA, LL_GPIO_PIN_9);
			LL_GPIO_SetOutputPin(GPIOA, LL_GPIO_PIN_10);
			sw_count = 1;
			//IRout = 0;
		}
		else if(sw_count == 1){
			LL_GPIO_ResetOutputPin(GPIOA, LL_GPIO_PIN_9);
			LL_GPIO_ResetOutputPin(GPIOA, LL_GPIO_PIN_10);
			sw_count = 0;
			//IRout = 0;
		}
		//LL_TIM_DisableCounter(TIM2);
    }
    LL_EXTI_ClearFlag_0_31(LL_EXTI_LINE_8); // Change EXTI line to LL_EXTI_LINE_8
}

//********************************************** FAN ************************************************

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
	l293d_initstruct.Pin = LL_GPIO_PIN_3 | LL_GPIO_PIN_5;
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

void PA11_EXTI_Config(void) // EXTI Config for PA11
{
    LL_EXTI_InitTypeDef PA11_EXTI_InitStruct;
    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_SYSCFG); // Enable Bus Clock
    LL_SYSCFG_SetEXTISource(LL_SYSCFG_EXTI_PORTA, LL_SYSCFG_EXTI_LINE11); // Change EXTI line to LL_SYSCFG_EXTI_LINE11
    PA11_EXTI_InitStruct.Line_0_31 = LL_EXTI_LINE_11; // Change EXTI line to LL_EXTI_LINE_11
    PA11_EXTI_InitStruct.LineCommand = ENABLE;
    PA11_EXTI_InitStruct.Mode = LL_EXTI_MODE_IT;
    PA11_EXTI_InitStruct.Trigger = LL_EXTI_TRIGGER_FALLING;
    LL_EXTI_Init(&PA11_EXTI_InitStruct);
    // Setup NVIC
    NVIC_EnableIRQ(EXTI15_10_IRQn); // Use EXTI15_10_IRQn for lines 10 to 15
    NVIC_SetPriority(EXTI15_10_IRQn, 0);
}



void EXTI15_10_IRQHandler(void) // EXTI Handler for lines 10 to 15
{
    if(LL_EXTI_IsActiveFlag_0_31(LL_EXTI_LINE_11) == SET)
		{ // Change EXTI line to LL_EXTI_LINE_11
			/*if(p>=0 && p<100)
			{
				p += 25;
				LL_TIM_OC_SetCompareCH2(TIM3, p);
			}
			else
			{
				p=0;
				LL_TIM_OC_SetCompareCH2(TIM3, p);
			}*/
			
			if(f_count == 0)
		{
			LL_GPIO_ResetOutputPin(GPIOB, LL_GPIO_PIN_3);
			f_count = 1;
			//IRout = 0;
		}
		else if(f_count == 1){
			LL_GPIO_SetOutputPin(GPIOB, LL_GPIO_PIN_3);
			f_count = 0;
			//IRout = 0;
		}
			
			//TIM_OC_Config();
    }
    LL_EXTI_ClearFlag_0_31(LL_EXTI_LINE_11); // Change EXTI line to LL_EXTI_LINE_11
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
