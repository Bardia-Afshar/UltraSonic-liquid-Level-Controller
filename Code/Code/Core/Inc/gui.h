/*************************************************************************************
 Title	:   GUI for Water Level Controller
 Author:    Bardia Alikhan Afshar <bardia.a.afshar@gmail.com>
 Software:  STM32CubeIDE
 Hardware:  Any STM32 device
*************************************************************************************/
#ifndef _GUI_H
#define _GUI_H
#include "main.h"
#include "lcd.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "eeprom.h"

//###################################### Defines ########################################
#define KeyM            HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_2)                   // Minus Key PIN
#define KeyR            HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_7)                   // Right Key PIN
#define KeyOK           HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_3)                   // OK Key PIN
#define KeyP            HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_4)                   // Plus Key PIN
#define KeyL            HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_8)                   // LEFT Key PIN
#define RelayCH          HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET)   // Control Relay ON
#define RelayCL			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET)  // Control Relay OFF
#define Pi 3.1415

//##################################### Variables #######################################
int cursor=0;
int r_vol=0;                       // Real-time Tanker Volume
int hlimit=0;                      // High Limit Range in Litre
int llimit=0;                      // Low Limit Range in Litre
uint16_t dia=0;                    // Tanker Diameter in CM
int height=0;             	       // Tanker Height in CM
bool RC_Flag=0;                    // Control Relay Flag
bool run_flag=1;				   // Run Flag
char c[25];                        // Char Variable for LCD
int PMval=1;          		 	   // Multiplier for variables (Ones-Tens-Hundreds-Thousands)
uint8_t cursor_colmn=12;		   // Cursor value (ROW)
uint16_t EEPROM_value_r[3];        // EEPROM Read Value
IWDG_HandleTypeDef hiwdg;
// ############################# Configuration Function ##################################


// --------------------------------- LCD Initializing -----------------------------------
void gui_config(void){
    lcd_Init();                   // Sets LCD in 4 BIT MODE
	HAL_Delay(200);
	lcd_Init();					  // Sets LCD in 4 BIT MODE
	lcd_clrscr();                 // Clears Screen
	lcd_gotoxy(0,0);              // Moves Cursor to 0,0
	return;
}
//################################   GUI Functions   #####################################

// ----------------------------------- Main Menu ---------------------------------
void main_menu(void){
	lcd_clrscr();				  // Clears LCD
	lcd_pos_puts(14,cursor,"<");  // Puts cursor on desired Position
	lcd_pos_puts(0,0,"1-RUN");
	lcd_pos_puts(0,1,"2-SETTING");
}

// ---------------------------------- Setting Menu --------------------------------

void setting_menu(void){
		lcd_clrscr();
		if(cursor<=3){							// Checks cursor number for page (if cursor<3 -> Page Number=0)
		sprintf(c,"DIAMETER=%4.4d CM",dia);
		lcd_pos_puts(0,0,c);
		sprintf(c,"HEIGHT  =%4.4d CM",height);
		lcd_pos_puts(0,1,c);
		sprintf(c,"H-Limit =%4.4d Litre",hlimit);
		lcd_pos_puts(0,2,c);
		sprintf(c,"L-Limit =%4.4d Litre",llimit);
				lcd_pos_puts(0,3,c);
		}
		else if(cursor>3){						// Checks cursor number for page (if cursor>3 -> Page Number=1(Next Page))
		lcd_pos_puts(0,0,"BACK");
}
		cursor>3? lcd_pos_puts(19,cursor-4,"<"):lcd_pos_puts(19,cursor,"<"); // Cursor Position Tuning
}

// ------------------------------ Parameter Setting Menu -----------------------------

void set_menu(uint8_t row,uint8_t curpos)
{											// Prints Special column from Setting Menu to set specific parameter With Blinking Cursor on its variable
											// Based on PMval
	switch(row){
	case 0:									// Prints just column 0 on LCD and puts blinking cursor on special digit based on curpos variable
	sprintf(c,"DIAMETER=%4.4d CM",dia);
	lcd_pos_puts(0,0,c);
	lcd_gotoxy(curpos,row);
	break;
	case 1:
	sprintf(c,"HEIGHT  =%4.4d CM",height);// Prints just column 1 on LCD and puts blinking cursor on special digit based on curpos variable
	lcd_pos_puts(0,1,c);
	lcd_gotoxy(curpos,row);
	break;
	case 2:
	sprintf(c,"H-Limit =%4.4d Litre",hlimit);// Prints just column 2 on LCD and puts blinking cursor on special digit based on curpos variable
	lcd_pos_puts(0,2,c);
	lcd_gotoxy(curpos,row);
	break;
	case 3:
	sprintf(c,"L-Limit =%4.4d Litre",llimit);// Prints just column 3 on LCD and puts blinking cursor on special digit based on curpos variable
	lcd_pos_puts(0,3,c);
	lcd_gotoxy(curpos,row);
	break;
}
}

// ------------------------------------ Variable Set function --------------------------
// This function is for setting variables based with changing ones,tens, hundreds & thousands

void set(uint8_t set){
lcd_clrscr();            	// Clears LCD
lcd_BlinkOn();				// Turns On build in LCD Blinking Cursor
set_menu(set,12);			// Loads Special Line of Setting Menu(Based on set Variable) & Initially Puts Cursor on column 12

while(1){
	HAL_IWDG_Refresh(&hiwdg); 	// Watchdog Reset
// ----------------------- Specific row Parameter Increase or Decrease

if(KeyP==0){ 					// Plus Key Pressed
HAL_Delay(200);					// Delay Based Debouncing
	switch(set){				// Based on Set Variable it changes specific parameter (Diameter-Height-High Limit-Low limit)
	case 0:
	dia+=PMval;					// Increases value Based on PMval which can be based on ones, tens, hundreds or thousands
	if(dia>9999)dia=9999;		// Limits Maximum value to 9999
	set_menu(set,cursor_colmn);
	break;
	case 1:
	height+=PMval;
	if(height>9999)height=9999;
	set_menu(set,cursor_colmn);
	break;
	case 2:
	hlimit+=PMval;
	if(hlimit>9999)hlimit=9999;
	set_menu(set,cursor_colmn);
	break;
	case 3:
	llimit+=PMval;
	if(llimit>9999)llimit=9999;
	set_menu(set,cursor_colmn);
	break;
	}
}
								// Exactly Like Plus button. However, it is for minus Button.
if(KeyM==0){
HAL_Delay(200);
	switch(set){
	case 0:
	dia-=PMval;				   // Decreases value Based on PMval which can be based on ones, tens, hundreds or thousands
	if(dia<0)dia=0;			   // Limits Minimun Value to 0
	set_menu(set,cursor_colmn);
	break;
	case 1:
	height-=PMval;
	if(height<0)height=0;
	set_menu(set,cursor_colmn);
	break;
	case 2:
	hlimit-=PMval;
	if(hlimit<0)hlimit=0;
	set_menu(set,cursor_colmn);
	break;
	case 3:
	llimit-=PMval;
	if(llimit<0)llimit=0;
	set_menu(set,cursor_colmn);
	break;
		}
	}

// ------------------------------------- Plus Variable Size --------------------------------------------
// this function is for increasing or decreasing variable based on ones, tens, hundreds or thousands.
if(KeyL==0){
HAL_Delay(200);
    cursor_colmn--;
	if(cursor_colmn<9){
	cursor_colmn=12;
	}
	switch(cursor_colmn){	// Based on Specific column, calculates PMval variables
	case 9:
	PMval=1000;
	break;
	case 10:
	PMval=100;
	break;
	case 11:
	PMval=10;
	break;
	case 12:
	PMval=1;
	break;
}
set_menu(set,cursor_colmn);
}


if(KeyR==0){            // Exactly like KeyL Case
HAL_Delay(200);
	cursor_colmn++;
	if(cursor_colmn>12){
	cursor_colmn=9;
}
	switch(cursor_colmn){
	case 9:
	PMval=1000;
	break;
	case 10:
	PMval=100;
	break;
	case 11:
	PMval=10;
	break;
	case 12:
	PMval=1;
	break;
}
	set_menu(set,cursor_colmn);
}


if(KeyOK==0){		   // Returns From Parameter set to Setting Menu
HAL_Delay(200);
	lcd_clrscr();
	setting_menu();
	lcd_BlinkOff();
	break;
}
}
}
// ---------------------------------- Setting Control -----------------------------------
void setting(void){
cursor=0;
setting_menu();
while(1){
	HAL_IWDG_Refresh(&hiwdg);			  // Watchdog Reset
	if(KeyM==0){
	HAL_Delay(200);                	      // Delay Based debouncing
	cursor++;                  			  // Changes Cursor Position
	if(cursor>4)cursor=0;				  // Limits Cursor Position
	setting_menu();                       // Main Menu Load
	}
	if(KeyP==0){
	HAL_Delay(200);                  	  // Delay Based debouncing
	cursor--; 							  // Changes Cursor Position
	if(cursor<0)cursor=4;				  // Limits Cursor Position
	setting_menu();                       // Main Menu Load
	}
if(KeyOK==0&&cursor==4){				  // Backs to Main Menu
	HAL_Delay(200);
	cursor=1;
	EE_WriteVariable(0, dia);			  // Writes Diameter Value on Address 0 of EEPROM
	EE_WriteVariable(1, height);		  // Writes Height Value on Address 1 of EEPROM
	EE_WriteVariable(2, hlimit);		  // Writes High Limit Value on Address 2 of EEPROM
	EE_WriteVariable(3, llimit);		  // Writes Low Limit Value on Address 3 of EEPROM
	run_flag=0;
	break;
	        }
if(KeyOK==0&&cursor<4){					 // Goes to Specific Parameter Set
	HAL_Delay(200);
	cursor_colmn=12;
	PMval=1;
	set(cursor);
}
}
return;
}

// ------------------------------------- RUN Menu ----------------------------------------
void run(void){


	char vl[5];
	lcd_clrscr();
	int radius=(int)(dia/2);
	int r_dis=0;
   if(hlimit==0||llimit==0||dia==0||height==0){ // Error if one of parameters=0
	   lcd_pos_puts(7,0,"ERROR!");
	   lcd_pos_puts(5,1,"PLEASE SET");
	   lcd_pos_puts(3,2,"ALL PARAMETERS");
	   HAL_Delay(3000);
	   lcd_clrscr();
	   main_menu();
	   return;
}

   if(((Pi*((dia/2)*(dia/2))*height)/1000)>9999){  // Error if Volume of tank (Based on Height & Diameter)>999(out of range)
	   lcd_pos_puts(7,0,"ERROR!");
	   lcd_pos_puts(3,1,"TANK VOLUME IS");
	   lcd_pos_puts(4,2,"OUT OF RANGE");
	   HAL_Delay(3000);
	   lcd_clrscr();
	   dia=0;
	   height=0;
	   EE_WriteVariable(0, dia);			  // Writes Diameter Value on Address 0 of EEPROM
	   	EE_WriteVariable(1, height);		  // Writes Height Value on Address 1 of EEPROM
	   main_menu();
	   return;
   }

	while(1){
	HAL_IWDG_Refresh(&hiwdg);													      // Watchdog Reset
	r_dis=Get_Range();                                                                // Gets Range in CM From UltraSonic Sensor
	if(r_dis>height)r_dis=height;						            		          // if UltraSonic Range> Tanker Height ----> UltraSonic Range=Height
	r_vol=abs((int)((Pi*radius*radius*height)-(Pi*radius*radius*r_dis))/1000);  	  // Calculating Volume from UltraSonic RangeFinder = Tanker total Volume - UltraSonic Calculated Volume
	if(r_vol>((Pi*radius*radius*height)/1000)){
	r_vol=(Pi*radius*radius*height)/1000;
	}
	if(hlimit>((Pi*((dia/2)*(dia/2))*height)/1000)){  							      // If High Limit> Total Tank Volume ---> High Limit = Tank Volume
	hlimit=((Pi*((dia/2)*(dia/2))*height)/1000);
}
	if(llimit>hlimit){																  // Low Limit cannot be Bigger than High Limit. If so --> Low limit = High limit
	llimit=hlimit;
}
	sprintf(vl,"%4.4d",(int)(Pi*((dia/2)*(dia/2))*height)/1000);                      //Variable to Character Conversion and Printing real time volume
	// Printing Data on LCD
	sprintf(c,"VOLUME=%4.4d/",r_vol);
	lcd_pos_puts(0,0,c);
	lcd_puts(vl);																	  // Prints Tank Volume
	lcd_puts(" Lit");
	sprintf(c,"H-Limit=%4.4d     Lit",hlimit);										  // Prints High Limit
	lcd_pos_puts(0,1,c);
	sprintf(c,"L-Limit=%4.4d     Lit",llimit);										  // Prints Low Limit
	lcd_pos_puts(0,2,c);
	RC_Flag==0 ? lcd_pos_puts(0,3,"PUMP:OFF"): lcd_pos_puts(0,3,"PUMP: ON");          // Prints Control & Safety Relay Status
	//----------------------
	// Relay Control
	if(r_vol>hlimit){ 																  // Higher than High Limit ----> Control Relay(Pump) Off

		RelayCL;
	RC_Flag=0;

	}
	if(r_vol<llimit){ 															   	  // Lower than Low Limit ----> Control Relay(Pump) ON

		RelayCH;
	RC_Flag=1;

	}
	// Returns to Main Menu
    if(KeyOK==0){
    	lcd_clrscr();
    	HAL_Delay(200);
    RC_Flag=0;
    RelayCL;
    run_flag=0;

    main_menu();

    break;
        }
}
return;
}

void manual(void){
	lcd_clrscr();
	lcd_pos_puts(0,0,"MANUAL PUMP CONTROL");
	RC_Flag=0;
	RelayCL;
	while(1){
	HAL_IWDG_Refresh(&hiwdg);													   // Watchdog Reset
	RC_Flag==0 ? lcd_pos_puts(0,1,"PUMP:OFF"): lcd_pos_puts(0,1,"PUMP: ON");  	   // Relay situation Print
if(KeyOK==0){																	   // Toggles Relay when OK key Pressed
	HAL_Delay(250);
	RC_Flag^=1;
	HAL_GPIO_TogglePin(GPIOA,GPIO_PIN_5);
	}
if(KeyL==0&&KeyR==0){															   // Backs to Normal Mode (Main Menu)
	HAL_Delay(250);
	RC_Flag=0;
	RelayCL;
	lcd_clrscr();
	main_menu();
	break;
}
}
return;
}
// ----------------------------- GUI Main Function ---------------------------------

void gui(void){

gui_config();  							   // Configs LCD
main_menu();							   // Loads Main Menu
EE_ReadVariable(0, &EEPROM_value_r[0]);    // Reads Value from Address 0 of EEPROM
EE_ReadVariable(1, &EEPROM_value_r[1]);	   // Reads Value from Address 1 of EEPROM
EE_ReadVariable(2, &EEPROM_value_r[2]);    // Reads Value from Address 2 of EEPROM
EE_ReadVariable(3, &EEPROM_value_r[3]);    // Reads Value from Address 3 of EEPROM

dia=EEPROM_value_r[0];					   // Puts Value on Address 0 in to the Diameter Variable
height=EEPROM_value_r[1];				   // Puts Value on Address 1 in to the Height Variable
hlimit=EEPROM_value_r[2];				   // Puts Value on Address 2 in to the High Limit Variable
llimit=EEPROM_value_r[3];				   // Puts Value on Address 3 in to the Low Limit Variable


if(hlimit!=0&&llimit!=0&&dia!=0&&height!=0&&run_flag==1){ // Error if one of parameters=0
	HAL_Delay(50);
	run();
}



while(1){
	HAL_IWDG_Refresh(&hiwdg);			   // Watchdog Reset
	if(KeyL==0&&KeyR==0){			       // Manual Mode
	HAL_Delay(200);
	manual();
	}

	if(KeyM==0){
	HAL_Delay(200);                    // Delay Based debouncing
	cursor=((cursor&1)^1);             // Changes Cursor Position
	main_menu();                       // Main Menu Load
}
	if(KeyP==0){
	HAL_Delay(200);                    // Delay Based debouncing
	cursor=((cursor&1)^1);             // Changes Cursor Position
	main_menu();                       // Main Menu Load
}

if(KeyOK==0){
	HAL_Delay(200);                       // Delay Based debouncing
	cursor==1 ? setting() : run(); 			  // Choosing Menu Function(Setting or Run)
	gui();
}
}
}
#endif
