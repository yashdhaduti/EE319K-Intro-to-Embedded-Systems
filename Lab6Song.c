#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"
#include "Sound.h"
#include "Stop.h"
#include "piano.h"
#define quarter 10000000*4
#define eighth quarter*0.5
#define half quarter*2
#define sixteen quarter*0.25
#define sfourth sixteen/6
#define G 3189
#define F 1790
#define Bb 2681
#define C 2389
#define D 2128
int pointer=0;

void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
long StartCritical (void);    // previous I bit, disable interrupts
void EndCritical(long sr);    // restore I bit to previous value
void WaitForInterrupt(void);  // low power mode
void (*PeriodicTask)(void);   // user function

struct music{uint32_t note; uint32_t duration;};

typedef const struct music Song_t;

Song_t fLove[39]={
	
	{G, quarter},
	{F, half},
	{G, quarter},
	{F, half},
	{Bb, quarter},
	{F, half},
	{C, quarter},
	{F, half},
	{G, quarter},
	{F, half},
	{G, quarter},
	{F, half},
	{Bb, quarter},
	{F, half},
	{C, quarter},
	{F, eighth},
	{D, eighth},
	{0, sfourth},
	{D, eighth},
	{0, sfourth},
	{D, eighth},
	{0, sfourth},
	{D, eighth},
	{0, sfourth},
	{D, eighth},
	{0, sfourth},
	{D, eighth},
	{0, sfourth},
	{D, eighth},
	{0, sfourth},
	{D, eighth},
	{0, sfourth},
	{D, sixteen},
	{C, sixteen},
	{Bb, quarter},
	{D, eighth},
	{0, sfourth},
	{D, eighth},
	{0,0}
	
	
};

void Timer0A_Init(void){
 // sr = StartCritical(); 
  SYSCTL_RCGCTIMER_R |= 0x01;   // 0) activate TIMER0
  //PeriodicTask = task;          // user function
  TIMER0_CTL_R = 0x00000000;    // 1) disable TIMER0A during setup
  TIMER0_CFG_R = 0x00000000;    // 2) configure for 32-bit mode
  TIMER0_TAMR_R = 0x00000002;   // 3) configure for periodic mode, default down-count settings
  TIMER0_TAILR_R = 0xffff;    // 4) reload value
  TIMER0_TAPR_R = 0;            // 5) bus clock resolution
  TIMER0_ICR_R = 0x00000001;    // 6) clear TIMER0A timeout flag
  TIMER0_IMR_R = 0x00000001;    // 7) arm timeout interrupt
  NVIC_PRI4_R = (NVIC_PRI4_R&0x00FFFFFF)|0x80000000; // 8) priority 4
// interrupts enabled in the main program after all devices initialized
// vector number 35, interrupt number 19
  NVIC_EN0_R = 1<<19;           // 9) enable IRQ 19 in NVIC
    // 10) enable TIMER0A
 // EndCritical(sr);
}

void Timer0A_Handler(void){
	
  TIMER0_ICR_R = TIMER_ICR_TATOCINT;// acknowledge timer0A timeout
	TIMER0_TAILR_R=fLove[pointer].duration;
	Sound_Play(fLove[pointer].note);
	pointer=(pointer+1)%39;
	
}

void playSong(void){
	
	
	TIMER0_CTL_R=1;
	
	
}

