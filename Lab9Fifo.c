// Fifo.c
// Runs on LM4F120/TM4C123
// Provide functions that implement the Software FiFo Buffer
// Last Modified: November 14, 2018
// Student names: change this to your names or look very silly
// Last modification date: change this to the last modification date or look very silly

#include <stdint.h>
// --UUU-- Declare state variables for Fifo
//        buffer, put and get indexes


// *********** Fifo_Init**********
// Initializes a software FIFO of a
// fixed size and sets up indexes for
// put and get operations
int putI;
int getI;
int N;
int FIFO[16];
void Fifo_Init(){
// --UUU-- Complete this
putI=getI=0;
N=16;
}

// *********** Fifo_Put**********
// Adds an element to the FIFO
// Input: Character to be inserted
// Output: 1 for success and 0 for failure
//         failure is when the buffer is full
uint32_t Fifo_Put(char data){
		if((putI+1)%N==getI)
			return 0;
		FIFO[putI]=data;
		putI=(putI+1)%N;
		return 1;
	}

// *********** FiFo_Get**********
// Gets an element from the FIFO
// Input: Pointer to a character that will get the character read from the buffer
// Output: 1 for success and 0 for failure
//         failure is when the buffer is empty
uint32_t Fifo_Get(char *datapt){ 
//--UUU-- Complete this routine
		if(putI==getI)
			return 0;
		*datapt=FIFO[getI];
		getI=(getI+1)%N;
		return 1;
}



