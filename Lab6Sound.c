// Sound.c
// This module contains the SysTick ISR that plays sound
// Runs on LM4F120 or TM4C123
// Program written by: put your names here
// Date Created: 3/6/17 
// Last Modified: 3/5/18 
// Lab number: 6
// Hardware connections
// TO STUDENTS "REMOVE THIS LINE AND SPECIFY YOUR HARDWARE********

// Code files contain the actual implemenation for public functions
// this file also contains an private functions and private data
#include <stdint.h>
#include "dac.h"
#include "../inc/tm4c123gh6pm.h"
#include "piano.h"
#define Db 2255
#define Bb 1341
#define Gb 1689
uint32_t array[33]={8,9,11,12,13,14,14,15,15,15,14,14,13,12,11,9,8,7,5,4,3,2,2,1,1,1,2,2,3,4,5,7,100};
uint32_t dataDac=0;
// **************Sound_Init*********************
// Initialize digital outputs and SysTick timer
// Called once, with sound/interrupts initially off
// Input: none
// Output: none

void Sound_Init(void){
	
  NVIC_ST_CTRL_R=0;
	NVIC_ST_RELOAD_R=0XFFF;
	NVIC_ST_CURRENT_R=0;
	NVIC_ST_CTRL_R=0X07;
}


// **************Sound_Play*********************
// Start sound output, and set Systick interrupt period 
// Sound continues until Sound_Play called again
// This function returns right away and sound is produced using a periodic interrupt
// Input: interrupt period
//           Units of period to be determined by YOU
//           Maximum period to be determined by YOU
//           Minimum period to be determined by YOU
//         if period equals zero, disable sound output
// Output: none
void Sound_Play(uint32_t period){
	
	if(period==0){
		GPIO_PORTB_DATA_R=0;
		NVIC_ST_RELOAD_R=0;
	} 
	else {
		NVIC_ST_RELOAD_R=period-1;
	}
}

void SysTick_Handler(){
	GPIO_PORTF_DATA_R^=0X4;
	if(array[dataDac]==100)
		{
			dataDac=0;
		}
		DAC_Out(array[dataDac]);
		dataDac++;
	}
	
