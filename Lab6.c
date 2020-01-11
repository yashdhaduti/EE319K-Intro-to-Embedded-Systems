// Lab6.c
// Runs on LM4F120 or TM4C123
// Use SysTick interrupts to implement a 3-key digital piano
// MOOC lab 13 or EE319K lab6 starter
// Program written by: HARISH RAJANALA AND YASH DHADUTI
// Date Created: 3/6/17 
// Last Modified: 3/5/18 
// Lab number: 6
// Hardware connections
// TO STUDENTS "REMOVE THIS LINE AND SPECIFY YOUR HARDWARE********


#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"
#include "Sound.h"
#include "Piano.h"
#include "TExaS.h"
#include "dac.h"
#include "Song.h"
#include "Stop.h"
#define Db 2009
#define Bb 2128
#define Gb 2255
// basic functions defined at end of startup.s
void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
//void Timer0A_Init(void);

int main(void){      
  TExaS_Init(SW_PIN_PE3210,DAC_PIN_PB3210,ScopeOn);    // bus clock at 80 MHz
	DisableInterrupts();
  Piano_Init();
  Sound_Init();
	DAC_Init();
	Timer0A_Init();
	
	SYSCTL_RCGCGPIO_R|=0x20;
	while(SYSCTL_RCGCGPIO_R==0){};
	GPIO_PORTF_DEN_R|=0x4;
	GPIO_PORTF_DIR_R|=0x4;
  // other initialization
  EnableInterrupts();
		
  while(1){
		
		if(Piano_In()==0x8){//song
			EnableInterrupts();
			playSong();
			
		}
		else{
			
			songStop();
			EnableInterrupts();
			if(Piano_In()==0x1){
			Sound_Play(Bb);
		} else if(Piano_In()==0x2){
			Sound_Play(Db);
		} else if(Piano_In()==0x4){
			Sound_Play(Gb);
		}else if (Piano_In()==0)
		Sound_Play(0);
		else
			DisableInterrupts();
		}
		
		

		
		
		
		
}

}
