/* 10/31/09	globals for all functions and routines. */

/* globals */
/* The UART ISR will need global variables. */
uint16 	uart0ArryIndex = 0; /* tells where uart0Arry begins */
/* hCoeffs in bytes, need extra byte for 0xAA and two extra
   bytes for the h coeff. array size. 
   We will set the array to 403 bytes, but we may not use all
   of it if we don't need that many h coefficients.*/
uint8 	uart0Arry[SETCOEFFSIZE+3]; /* although we only transmit half
                                      the h coefficient array. */ 
uint8 	pack = 0;		/* tells when to pack the 32 bits hCoeffs. */
uint8		sigBut = 0x00; /* Signals button is pushed from labview */
uint8 	filtrBut = 0x00; /* Filtered button is pushed from labview */
uint8 	*setGrphColorPtr, *setColorHstry; /* set graphic color ptr */
uint8 	cmd = 0; /* A command sent from labview. */
/***********************/	