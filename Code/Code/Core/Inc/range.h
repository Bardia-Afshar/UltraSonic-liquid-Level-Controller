/*************************************************************************************
 Title	:   JSN-SR04T Ultrasonic Sensor Library With Mandatory uC Peripherals for STM32
 Author:    Bardia Alikhan Afshar <bardia.a.afshar@gmail.com>
 Software:  STM32CubeIDE
 Hardware:  Any STM32 device
*************************************************************************************/
#ifndef _RANGE_H
#define _RANGE_H
#include <stdbool.h>
//---------------------------------- PIN Defines ------------------------------------
#define TRIGPORT GPIOA                                          // GPIO PORT for Trig
#define TRIGPIN  GPIO_PIN_1                                     // GPIO PIN for Trig
#define ECOPIN GPIO_PIN_0                                       // External Interrupt for Echo Pin


// --------------------------------- Variables --------------------------------------
TIM_HandleTypeDef htim14;
uint32_t Time=0;                                                 // Time is MicroSecond
int distance=0;                                                  // Distance in CM
bool cond=0;                                                     // Trig & Echo Pin Condition

// --------------------------------- Functions --------------------------------------
// ----------------------------TIM14 Callback Routine -------------------------------
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim){
	if(htim->Instance==TIM14){
Time+=65535;                                                     // Adds 65535uS on Timer-overflow
}
}

//----------------------- External Interrupt Callback Routine -----------------------

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){
	if(GPIO_Pin==ECOPIN){                                        // External Interrupt on PIN1
	cond^=1;
	if(cond==1){                                                 // Rising Edge Detection
		htim14.Instance->CNT=0;                                  // Sets Timer value to Zero
		Time=0;
		HAL_TIM_Base_Start(&htim14);                             // Starts Timer On Rising Edge
}
	if(cond==0){                                                 // Falling Edge Detection
HAL_TIM_Base_Stop(&htim14);                                      // Stops Timer on Falling Edge Detection
Time+=htim14.Instance->CNT;                                      // Overflow Time + Counter value = Time in uS
distance = Time * .034/2;                                        // Speed of Sound = 340 M/S = 0.034 CM/uS - Travel Time divided by 2 to get Distance in CM
if(distance<25)distance=0;                                       // Distance Low Limit for JSN-SR04T is 25 in CM
if(distance>450)distance=450;
	}
}
}

// ---------------------------- Range Finder function -------------------------------
int Get_Range(void){
	htim14.Instance->CNT=0;
	Time=0;
	HAL_GPIO_WritePin(TRIGPORT, TRIGPIN, GPIO_PIN_SET);         // SETS Trig PIN
		  HAL_Delay(1);
		  HAL_GPIO_WritePin(TRIGPORT, TRIGPIN, GPIO_PIN_RESET); // Resets Trig PIN

		  HAL_Delay(50);
return distance;
}
#endif
