// StepperMotorController.c starter file EE319K Lab 5
// Runs on TM4C123
// Finite state machine to operate a stepper motor.  
// Yash Dhaduti and Harish Rajanala
// January 18, 2019

// Hardware connections (External: two input buttons and four outputs to stepper motor)
//  PA5 is Wash input  (1 means pressed, 0 means not pressed)
//  PA4 is Wiper input  (1 means pressed, 0 means not pressed)
//  PE5 is Water pump output (toggle means washing)
//  PE4-0 are stepper motor outputs 
//  PF1 PF2 or PF3 control the LED on Launchpad used as a heartbeat
//  PB6 is LED output (1 activates external LED on protoboard)

#include "SysTick.h"
#include "TExaS.h"
#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"
volatile int nop;
void EnableInterrupts(void);
// edit the following only if you need to move pins from PA4, PE3-0      
// logic analyzer on the real board
#define PA4       (*((volatile unsigned long *)0x40004040))
#define PE50      (*((volatile unsigned long *)0x400240FC))
void SendDataToLogicAnalyzer(void){
  UART0_DR_R = 0x80|(PA4<<2)|PE50;
}

struct state{uint8_t out; uint32_t next[4];};

typedef const struct state State_t;

State_t fsm[43]={
	{0x00,{0,1,22,22}},//Start
	{0x01,{2,2,23,23}},//Wiper1
	{0x02,{3,3,24,24}},//Wiper2
	{0x04,{4,4,25,25}},//Wiper4
	{0x08,{5,5,26,26}},//Wiper8
	{0x10,{6,6,27,27}},//Wiper16
	{0x01,{7,7,28,28}},//Wiper1.1
	{0x02,{8,8,29,29}},//Wiper1.2
	{0x04,{9,9,30,30}},//Wiper1.4
	{0x08,{10,10,31,31}},//Wiper1.8
	{0x10,{11,11,32,32}},//Wiper1.16
	{0x01,{12,12,33,33 }},//WiperTOP
	{0x10,{13,13,34,34}},//Wiper2.16
	{0x08,{14,14,35,35}},//Wiper2.8
	{0x04,{15,15,36,36}},//Wiper2.4
	{0x02,{16,16,37,37}},//Wiper2.2
	{0x01,{17,17,38,38}},//Wiper2.1
	{0x10,{18,18,39,39}},//Wiper3.16
	{0x08,{19,19,40,40}},//Wiper3.8
	{0x04,{20,20,41,41}},//Wiper3.4
	{0x02,{21,21,42,42}},//Wiper3.2
	{0x01,{0,1,22,22}},//WiperEND
	{0x21,{2,2,23,23}},//Wash1
	{0x02,{3,3,24,24}},//Wash2
	{0x24,{4,4,25,25}},//Wash4
	{0x08,{5,5,26,26}},//Wash8
	{0x30,{6,6,27,27}},//Wash16
	{0x01,{7,7,28,28}},//Wash1.1
	{0x22,{8,8,29,29}},//Wash1.2
	{0x04,{9,9,30,30}},//Wash1.4
	{0x28,{10,10,31,31}},//Wash1.8
	{0x10,{11,11,32,32}},//Wash1.16
	{0x21,{12,12,33,33}},//WashTOP
	{0x10,{13,13,34,34}},//Wash2.16
	{0x28,{14,14,35,35}},//Wash2.8
	{0x04,{15,15,36,36}},//Wash2.4
	{0x22,{16,16,37,37}},//Wash2.2
	{0x01,{17,17,38,38}},//Wash2.1
	{0x30,{18,18,39,39}},//Wash3.16
	{0x08,{19,19,40,40}},//Wash3.8
	{0x24,{20,20,41,41}},//Wash3.4
	{0x02,{21,21,42,42}},//Wash3.2
	{0x21,{0,1,22,22}},//WashEND
	
};

uint32_t input;
uint32_t pt = 0;
int main(void){ 
  TExaS_Init(&SendDataToLogicAnalyzer);    // activate logic analyzer and set system clock to 80 MHz
  SysTick_Init();   
// you initialize your system here
	SYSCTL_RCGCGPIO_R |= 0x11;
	nop++;
	nop--;
	GPIO_PORTA_DIR_R &= 0xCF;
	GPIO_PORTA_DEN_R |= 0x30;
	GPIO_PORTE_DIR_R |= 0x3F;
	GPIO_PORTE_DEN_R |= 0x3F;
	
  EnableInterrupts(); 
  while(1){
		// output
		GPIO_PORTE_DATA_R = fsm[pt].out;
		SysTick_Wait10ms(5);
		// input
		input = (GPIO_PORTA_DATA_R&0x30)>>4;
		// next		
		pt = fsm[pt].next[input];
  }
}



