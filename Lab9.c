// Lab9.c
// Runs on LM4F120 or TM4C123
// Student names: change this to your names or look very silly
// Last modification date: change this to the last modification date or look very silly
// Last Modified: 11/14/2018

// Analog Input connected to PE2=ADC1
// displays on Sitronox ST7735
// PF3, PF2, PF1 are heartbeats
// This U0Rx PC4 (in) is connected to other LaunchPad PC5 (out)
// This U0Tx PC5 (out) is connected to other LaunchPad PC4 (in)
// This ground is connected to other LaunchPad ground
// * Start with where you left off in Lab8. 
// * Get Lab8 code working in this project.
// * Understand what parts of your main have to move into the UART1_Handler ISR
// * Rewrite the SysTickHandler
// * Implement the s/w Fifo on the receiver end 
//    (we suggest implementing and testing this first)

#include <stdint.h>

#include "ST7735.h"
#include "PLL.h"
#include "ADC.h"
#include "print.h"
#include "../inc/tm4c123gh6pm.h"
#include "Uart.h"
#include "FiFo.h"

//*****the first three main programs are for debugging *****
// main1 tests just the ADC and slide pot, use debugger to see data
// main2 adds the LCD to the ADC and slide pot, ADC data is on Nokia
// main3 adds your convert function, position data is no Nokia

void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts

#define PF1       (*((volatile uint32_t *)0x40025008))
#define PF2       (*((volatile uint32_t *)0x40025010))
#define PF3       (*((volatile uint32_t *)0x40025020))
uint32_t Data;      // 12-bit ADC
uint32_t Position;  // 32-bit fixed-point 0.001 cm


// Initialize Port F so PF1, PF2 and PF3 are heartbeats
void PortF_Init(void){
// Intialize PortF for hearbeat
	SYSCTL_RCGCGPIO_R |= 0x20;
	volatile int delay;
	delay++;
	GPIO_PORTF_DIR_R |= 0x8;
	GPIO_PORTF_DEN_R |= 0x8;

}
void SysTick_Init(void){
	NVIC_ST_CTRL_R = 0;
	NVIC_ST_RELOAD_R = 1333333; //80Mhz/120
	NVIC_ST_CURRENT_R = 0;
	NVIC_ST_CTRL_R = 0x00000007;
}



uint32_t Status[20];             // entries 0,7,12,19 should be false, others true
char GetData[10];  // entries 1 2 3 4 5 6 7 8 should be 1 2 3 4 5 6 7 8
int main1(void){ // Make this main to test FiFo
  Fifo_Init();   // Assuming a buffer of size 6
  for(;;){
    Status[0]  = Fifo_Get(&GetData[0]);  // should fail,    empty
    Status[1]  = Fifo_Put(1);            // should succeed, 1 
    Status[2]  = Fifo_Put(2);            // should succeed, 1 2
    Status[3]  = Fifo_Put(3);            // should succeed, 1 2 3
    Status[4]  = Fifo_Put(4);            // should succeed, 1 2 3 4
    Status[5]  = Fifo_Put(5);            // should succeed, 1 2 3 4 5
    Status[6]  = Fifo_Put(6);            // should succeed, 1 2 3 4 5 6
    Status[7]  = Fifo_Put(7);            // should fail,    1 2 3 4 5 6 
    Status[8]  = Fifo_Get(&GetData[1]);  // should succeed, 2 3 4 5 6
    Status[9]  = Fifo_Get(&GetData[2]);  // should succeed, 3 4 5 6
    Status[10] = Fifo_Put(7);            // should succeed, 3 4 5 6 7
    Status[11] = Fifo_Put(8);            // should succeed, 3 4 5 6 7 8
    Status[12] = Fifo_Put(9);            // should fail,    3 4 5 6 7 8 
    Status[13] = Fifo_Get(&GetData[3]);  // should succeed, 4 5 6 7 8
    Status[14] = Fifo_Get(&GetData[4]);  // should succeed, 5 6 7 8
    Status[15] = Fifo_Get(&GetData[5]);  // should succeed, 6 7 8
    Status[16] = Fifo_Get(&GetData[6]);  // should succeed, 7 8
    Status[17] = Fifo_Get(&GetData[7]);  // should succeed, 8
    Status[18] = Fifo_Get(&GetData[8]);  // should succeed, empty
    Status[19] = Fifo_Get(&GetData[9]);  // should fail,    empty
  }
}

// Get fit from excel and code the convert routine with the constants
// from the curve-fit
uint32_t Convert(uint32_t input){
return ((0.4632*input)+222.1);
}


// final main program for bidirectional communication
// Sender sends using SysTick Interrupt
// Receiver receives using RX
int main(void){ 
  
  PLL_Init(Bus80MHz);     // Bus clock is 80 MHz 
  ST7735_InitR(INITR_REDTAB);
  ADC_Init();// initialize to sample ADC
  PortF_Init();
  Uart_Init();       // initialize UART
  SysTick_Init();
//Enable SysTick Interrupt by calling SysTick_Init()
  EnableInterrupts();
	char zemo;
  while(1){
		ST7735_SetCursor(0,0);
    //--UUU--Complete this  - see lab manual
		while(zemo!=0x02)
		{
			Fifo_Get(&zemo);
		}
		Fifo_Get(&zemo);
		ST7735_OutChar(zemo);
		Fifo_Get(&zemo);
		ST7735_OutChar(zemo);
		Fifo_Get(&zemo);
		ST7735_OutChar(zemo);
		Fifo_Get(&zemo);
		ST7735_OutChar(zemo);
		Fifo_Get(&zemo);
		ST7735_OutChar(zemo);
		while(zemo!=0x03)
		{
			Fifo_Get(&zemo);
		}
  }
}

/* SysTick ISR
*/
int TxCounter=0;
int ADCMail=0;
int ADCStatus=0;

void SysTick_Handler(void){ // every 20 ms
 //Sample ADC, convert to distance, create 8-byte message, send message out UART1
		PF3^=0x8;
		ADCMail=ADC_In();
		ADCMail=Convert(ADCMail);
		//PF3^=0x8;
		ADCStatus=1;
		int one=ADCMail/1000;
		int two=(ADCMail-one*1000)/100;
		int three=(ADCMail-one*1000-two*100)/10;
		int four=(ADCMail-one*1000-two*100-three*10);
		Uart_OutChar(0x02);
		Uart_OutChar(0x30+one);
		Uart_OutChar(0x2e);
		Uart_OutChar(0x30+two);
		Uart_OutChar(0x30+three);
		Uart_OutChar(0x30+four);
		Uart_OutChar(0x0d);
		Uart_OutChar(0x03);
	TxCounter++;
	PF3^=0x8;
}

