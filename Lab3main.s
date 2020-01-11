;****************** main.s ***************
; Program written by: ***Harish Rajanala and Yash Dhaduti***
; Date Created: 2/4/2017
; Last Modified: 1/18/2019
; Brief description of the program
;   The LED toggles at 2 Hz and a varying duty-cycle
; Hardware connections (External: One button and one LED)
;  PE2 is Button input  (1 means pressed, 0 means not pressed)
;  PE3 is LED output (1 activates external LED on protoboard)
;  PF4 is builtin button SW1 on Launchpad (Internal) 
;        Negative Logic (0 means pressed, 1 means not pressed)
; Overall functionality of this system is to operate like this
;   1) Make PE3 an output and make PE2 and PF4 inputs.
;   2) The system starts with the the LED toggling at 2Hz,
;      which is 2 times per second with a duty-cycle of 30%.
;      Therefore, the LED is ON for 150ms and off for 350 ms.
;   3) When the button (PE1) is pressed-and-released increase
;      the duty cycle by 20% (modulo 100%). Therefore for each
;      press-and-release the duty cycle changes from 30% to 70% to 70%
;      to 90% to 10% to 30% so on
;   4) Implement a "breathing LED" when SW1 (PF4) on the Launchpad is pressed:
;      a) Be creative and play around with what "breathing" means.
;         An example of "breathing" is most computers power LED in sleep mode
;         (e.g., https://www.youtube.com/watch?v=ZT6siXyIjvQ).
;      b) When (PF4) is released while in breathing mode, resume blinking at 2Hz.
;         The duty cycle can either match the most recent duty-
;         cycle or reset to 30%.
;      TIP: debugging the breathing LED algorithm using the real board.
; PortE device registers
GPIO_PORTE_DATA_R  EQU 0x400243FC
GPIO_PORTE_DIR_R   EQU 0x40024400
GPIO_PORTE_AFSEL_R EQU 0x40024420
GPIO_PORTE_DEN_R   EQU 0x4002451C
; PortF device registers
GPIO_PORTF_DATA_R  EQU 0x400253FC
GPIO_PORTF_DIR_R   EQU 0x40025400
GPIO_PORTF_AFSEL_R EQU 0x40025420
GPIO_PORTF_PUR_R   EQU 0x40025510
GPIO_PORTF_DEN_R   EQU 0x4002551C
GPIO_PORTF_LOCK_R  EQU 0x40025520
GPIO_PORTF_CR_R    EQU 0x40025524
GPIO_LOCK_KEY      EQU 0x4C4F434B  ; Unlocks the GPIO_CR register
SYSCTL_RCGCGPIO_R  EQU 0x400FE608
TOTAL EQU 0xFFD74F56
TOTALTWO EQU 0XFFFE7960
       IMPORT  TExaS_Init
       THUMB
       AREA    DATA, ALIGN=2
;global variables go here
       AREA    |.text|, CODE, READONLY, ALIGN=2
       THUMB
       EXPORT  Start
Start
 ; TExaS_Init sets bus clock at 80 MHz
     BL  TExaS_Init ; voltmeter, scope on PD3
	LDR R0, =SYSCTL_RCGCGPIO_R
	LDR R1, [R0]
	ORR R1, #0X30
	STR R1, [R0] ; CLOCK START
	NOP
	NOP
	LDR R0, =GPIO_PORTE_DEN_R
	LDR R1, [R0]
	ORR R1, #0XC
	STR R1, [R0] ; ENABLE THE PORTS
	LDR R0, =GPIO_PORTE_DIR_R
	LDR R1, [R0]
	ORR R1, #0X8
	STR R1, [R0] ; OUT/IN PUT
	LDR R0, =GPIO_PORTF_DEN_R
	LDR R1, [R0]
	ORR R1, #0X10
	STR R1, [R0] ; ENABLE THE PORTS
	LDR R0, =GPIO_PORTF_DIR_R
	LDR R1, [R0]
	ORR R1, #0X0
	STR R1, [R0] ; OUT/IN PUT
	LDR R0, =GPIO_LOCK_KEY
	LDR R2, =GPIO_PORTF_LOCK_R
	STR R0, [R2]
	LDR R2, =GPIO_PORTF_CR_R
	MOV R1, #0X1
	STR R1, [R2]
	LDR R0, =GPIO_PORTE_AFSEL_R
	MOV R1, #0X0
	STR R1, [R0]
	LDR R0, =GPIO_PORTF_AFSEL_R
	MOV R1, #0X0
	STR R1, [R0]
	LDR R0, =GPIO_PORTF_PUR_R
	MOV R1, #0X10
	STR R1, [R0]
	; Initialization goes here


     CPSIE  I    ; TExaS voltmeter, scope runs on interrupts
	MOV R2, #0x0
	MOV R3, #0X0
	MOV R12, #0X0
	MOV R10, #-1
	LDR R8, =TOTAL
	B NEXT
	 
DELAYON
	LDR R0, =GPIO_PORTE_DATA_R
	LDR R1, [R0]
	AND R1, #0X4
	TST R1, R1
	BEQ SKIPFO	;
	ORR R6, R6, #1 ;
	B DELAYON ;
SKIPFO 
	CMP R6, R1
	BNE ADDONE
	SUBS R2, #1
	BNE DELAYON
	MOV R1, #0X0
	STR R1, [R0]
	ADD R7, R7, R8	;gets the value of off delay
	MUL R7, R10		;multiplies by -1
DELAYOFF
		LDR R0, =GPIO_PORTE_DATA_R
		LDR R1, [R0]
		AND R1, #0X4
		TST R1, R1
		BEQ SKIPTHRE
		ORR R6, R6, #1
		B DELAYOFF
SKIPTHRE 
		CMP R6, R1
		BNE ADDONE
		SUBS R7, #1
		BNE DELAYOFF
	B NEXT
	
NEXT
	MOV R12, #0X0
	LDR R0, =GPIO_PORTF_DATA_R
	LDR R1, [R0]
	AND R1, #0X10
	TST R1, R1
	BEQ BREATHING
	LDR R0, =GPIO_PORTE_DATA_R
	MOV R1, #0X8
	STR R1, [R0]	;turns LED on
	LDR R1, [R0]	
	AND R6, R1, #0X4	;R6 is 1 if on, R6 is 0 if off
	CMP R6, #0
	BEQ SKIP
	ADD R3, R3, #4
SKIP 
	CMP R3, #20
	BNE SKIPTWO
	SUB R3, R3, #20
SKIPTWO 
	LDR R7, =BLINK
	LDR R7, [R7,R3]
	ADD R2, R7, #0
	MOV R6, #0X0
	B DELAYON
    BX LR
ADDONE
	ADD R3, R3, #4
	B NEXT
BREATHING
	LDR R0, =GPIO_PORTE_DATA_R
	LDR R1, [R0]
	ORR R1, R1, #0x8
	STR R1, [R0]
	LDR R11, =BREATHE
	LDR R11, [R11, R12]
	LDR R2, =TOTALTWO
	ADD R2, R11, R2 ;r2 is off r11 is on
	MUL R2, R10
	B DELAYB
RETURN
	ADD R12, R12, #4
	CMP R12, #396
	BEQ SKIPFIVE
	B BREATHING
SKIPFIVE
	SUB R12, R12, #4
	B CHANGEELEV
DELAYB
	LDR R0, =GPIO_PORTF_DATA_R
	LDR R1, [R0]
	TST R1, #0X10
	BNE NEXT
	SUBS R11, #1
	BNE DELAYB
	LDR R0, =GPIO_PORTE_DATA_R
	MOV R1, #0
	STR R1, [R0]
DELAYBOF
	LDR R1, [R0]
	TST R1, #0X10
	BNE NEXT
	SUBS R2, #1
	BNE  DELAYBOF
	B RETURN
CHANGEELEV
	LDR R0, =GPIO_PORTE_DATA_R
	LDR R1, [R0]
	ORR R1, R1, #0x8
	STR R1, [R0]
	CMP R12, #0
	BEQ BREATHING
	LDR R11, =BREATHE
	LDR R11, [R11, R12]	;on
	LDR R2, =TOTALTWO
	ADD R2, R11, R2
	MUL R2, R10
	
DELAYBTWO
	LDR R0, =GPIO_PORTF_DATA_R
	LDR R1, [R0]
	TST R1, #0X10
	BNE NEXT
	SUBS R11, #1
	BNE DELAYBTWO
	LDR R0, =GPIO_PORTE_DATA_R
	MOV R1, #0
	STR R1, [R0]
DELAYBOFTWO
	LDR R0, =GPIO_PORTF_DATA_R
	LDR R1, [R0]
	TST R1, #0X10
	BNE NEXT
	SUBS R2, #1
	BNE  DELAYBOFTWO
	B SKIPFIVE	
BLINK DCD 0xC3500, 0x145854, 0x1C7BAA, 0x249F00, 0x4100A
BREATHE DCD 1000, 2000, 3000, 4000, 5000, 6000, 7000, 8000, 9000, 10000, 11000, 12000, 13000, 14000, 15000, 16000, 17000, 18000, 19000, 20000, 21000, 22000, 23000, 24000, 25000, 26000, 27000, 28000, 29000, 30000, 31000, 32000, 33000, 34000, 35000, 36000, 37000, 38000, 39000, 40000, 41000, 42000, 43000, 44000, 45000, 46000, 47000, 48000, 49000, 50000, 51000, 52000, 53000, 54000, 55000, 56000, 57000, 58000, 59000, 60000, 61000, 62000, 63000, 64000, 65000, 66000, 67000, 68000, 69000, 70000, 71000, 72000, 73000, 74000, 75000, 76000, 77000, 78000, 79000, 80000, 81000, 82000, 83000, 84000, 85000, 86000, 87000, 88000, 89000, 90000, 91000, 92000, 93000, 94000, 95000, 96000, 97000, 98000, 99000, 100000
     ALIGN  
		 ; make sure the end of this section is aligned
     END        ; end of file