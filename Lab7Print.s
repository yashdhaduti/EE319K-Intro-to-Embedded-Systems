; Print.s
; Student names: Yash Dhaduti and Harish Rajanala
; Last modification date: idk
; Runs on LM4F120 or TM4C123
; EE319K lab 7 device driver for any LCD
;
; As part of Lab 7, students need to implement these LCD_OutDec and LCD_OutFix
; This driver assumes two low-level LCD functions
; ST7735_OutChar   outputs a single 8-bit ASCII character
; ST7735_OutString outputs a null-terminated string 
    IMPORT   ST7735_OutChar
    IMPORT   ST7735_OutString
    EXPORT   LCD_OutDec
    EXPORT   LCD_OutFix
	
    AREA    |.text|, CODE, READONLY, ALIGN=2
    THUMB
REM	EQU 0x0
	
  

;-----------------------LCD_OutDec-----------------------
; Output a 32-bit number in unsigned decimal format
; Input: R0 (call by value) 32-bit unsigned number
; Output: none
; Invariables: This function must not permanently modify registers R4 to R11
LCD_OutDec
	PUSH{R0,LR}
	MOV R11, SP
recursion
	MOV R2, #10
	MOV R3, R0
	CMP R3, #10
	BLO Out
	SUB SP, #8
	UDIV R0, R3, R2
	MUL R2, R0, R2
	SUB R3, R3, R2
	STR R3, [SP, #REM]
	B recursion
Out
	ADD R0, R3, #0x30
	BL ST7735_OutChar
loop
	CMP R11, SP
	BEQ done
	LDR R0, [SP, #REM]
	ADD R0, R0, #0x30
	BL ST7735_OutChar
	ADD SP, #8
	B loop
done
	  POP{R0,LR}
      BX  LR
;* * * * * * * * End of LCD_OutDec * * * * * * * *

; -----------------------LCD _OutFix----------------------
; Output characters to LCD display in fixed-point format
; unsigned decimal, resolution 0.001, range 0.000 to 9.999
; Inputs:  R0 is an unsigned 32-bit number
; Outputs: none
; E.g., R0=0,    then output "0.000 "
;       R0=3,    then output "0.003 "
;       R0=89,   then output "0.089 "
;       R0=123,  then output "0.123 "
;       R0=9999, then output "9.999 "
;       R0>9999, then output "*.*** "
; Invariables: This function must not permanently modify registers R4 to R11
LCD_OutFix
	PUSH{R0, LR}
	MOV R2, R0
	MOV R3, #1000
	UDIV R1, R0, R3
	CMP R1, #10
	BHS special
	ADD R0, R1, #0x30
	PUSH{R0,R1,R2,R3}
	BL ST7735_OutChar
	MOV R0, #0x2E
	BL ST7735_OutChar
	POP{R0,R1,R2,R3}
	MUL R0,	R1, R3
	SUB R0, R2, R0
	
	MOV R11, SP
	MOV R2, #10
	UDIV R2, R0, R2
	CMP R2, #0
	BEQ onesplace
	MOV R2, #100
	UDIV R2, R0, R2
	CMP R2, #0
	BEQ tensplace
	B recursion2
onesplace
	MOV R1,R0
	MOV R0, #0x30
	PUSH{R0,R1}
	BL ST7735_OutChar
	MOV R0, #0x30
	BL ST7735_OutChar
	POP{R0,R1}
	MOV R0, R1
	B recursion2
tensplace
	MOV R1, R0
	MOV R0, #0x30
	PUSH{R0,R1}
	BL ST7735_OutChar
	POP{R0,R1}
	MOV R0, R1
	B recursion2
	
recursion2
	MOV R2, #10
	MOV R3, R0
	CMP R3, #10
	BLO Out
	SUB SP, #8
	UDIV R0, R3, R2
	MUL R2, R0, R2
	SUB R3, R3, R2
	STR R3, [SP, #REM]
	B recursion
Out2
	ADD R0, R3, #0x30
	BL ST7735_OutChar
	
loop2
	ADD R12, #1
	CMP R11, SP
	BEQ done2
	LDR R0, [SP, #REM]
	ADD R0, R0, #0x30
	BL ST7735_OutChar
	ADD SP, #8
	B loop2
special
	MOV R0, #0x2A
	BL ST7735_OutChar
	MOV R0, #0x2E
	BL ST7735_OutChar
	MOV R0, #0x2A
	BL ST7735_OutChar
	MOV R0, #0x2A
	BL ST7735_OutChar
	MOV R0, #0x2A
	BL ST7735_OutChar
done2
	POP{R0,LR}	
     BX   LR
 
     ALIGN
;* * * * * * * * End of LCD_OutFix * * * * * * * *

     ALIGN                           ; make sure the end of this section is aligned
     END                             ; end of file
