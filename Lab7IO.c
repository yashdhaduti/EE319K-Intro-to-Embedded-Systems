// IO.c
// This software configures the switch and LED
// You are allowed to use any switch and any LED, 
// although the Lab suggests the SW1 switch PF4 and Red LED PF1
// Runs on LM4F120 or TM4C123
// Program written by: Yash Dhaduti and Harish Rajanala
// Date Created: March 30, 2018
// Last Modified: idk
// Lab number: 7


#include "../inc/tm4c123gh6pm.h"
#include <stdint.h>
#include "ST7735.h"


//------------IO_Init------------
// Initialize GPIO Port for a switch and an LED
// Input: none
// Output: none
void IO_Init(void) {
  SYSCTL_RCGCGPIO_R|=0x20;
	volatile int nop;
	nop++;			
	GPIO_PORTF_DEN_R|=0x14;	//2 and 4
	GPIO_PORTF_DIR_R|=0x04;
	GPIO_PORTF_DIR_R&=~(0x10);
	GPIO_PORTF_PUR_R|=0x10;
	GPIO_PORTF_LOCK_R = GPIO_LOCK_KEY;
	GPIO_PORTF_CR_R = 0xFFFFFFFF;
}

//------------IO_HeartBeat------------
// Toggle the output state of the  LED.
// Input: none
// Output: none
void IO_HeartBeat(void) {
 GPIO_PORTF_DATA_R ^=0x04;
}


//------------IO_Touch------------
// wait for release and press of the switch
// Delay to debounce the switch
// Input: none
// Output: none
void IO_Touch(void) {
 // --UUU-- wait for release; delay for 20ms; and then wait for press
	while((GPIO_PORTF_DATA_R&0x10) == 0x10){}
	Delay1ms(20);
	while((GPIO_PORTF_DATA_R&0x10) == 0){}
}  

