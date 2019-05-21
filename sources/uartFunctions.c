/* 10/31/09	Uart functions.
   4/20/13  We will be using rs232 instead of the
            touchscreen to control the lcd. 
            Ideally, you don't want to do a lot of work
            in these isr routines. Just get the data
            and let some non-isr routines do the
            work. */

/* prototypes */
void uart0IntSetUp(void);	
void uart0_handler(void);
/***************************/

/* functions begin */

void uart0IntSetUp(void)
{
	//unmask UART0~UART2 interrupts in INTC0
 	MCF_INTC0_IMRL &= ~(0
        | MCF_INTC_IMRL_INT_MASK13 
        | MCF_INTC_IMRL_MASKALL);
        
  /* Setup the interrupt level and priority for UART0~UART2 */
	MCF_INTC0_ICR13 = MCF_INTC_ICR_IL(UART0IL) | MCF_INTC_ICR_IP(UART0PL);
	
	MCF_UART0_UBG1 = 0x00;
	MCF_UART0_UBG2 = 16; /* 16 dec. This is uart divider for 38,400. */
}
/* uart0 isr ***********/
/* must use globals for isr */
/* this routine retreive the serial
   hCoeffs. bytes from LabVIEW. 
   This time we are only sending half of the
   h coefficients array.  This is because of
   even symmetry.  The micro controller does
   not know this.  Therefore, we don't have
   to change this routine at all. 
   We just have to change the
   h coefficients lcd routines. 7/17/11 */
void uart0_handler(void)
{	
	/* Basically, this isr will continue to fill up the adc array
     until it encounters the 0xaa flag (COEFFENDFLG). 
     Right now we'll assume that the entire packet will be 
     transmitted sucessfully from start to finish. */
	/* the interrupt is automatically cleared */

	/* fill the uart0Arry with the incoming byte,
	   the isr gets called for every byte that comes
	   in. */
	*(uart0Arry+uart0ArryIndex) = MCF_UART0_URB;

	/* if we have come to the end of the transmission
	with the xAA flag. */
	if(MCF_UART0_URB == COEFFENDFLG)
	{
		if (uart0ArryIndex > 2)
		{				
			/* we have reached the end of the transmission,
			tell the program to repack the h coefficients. */
			pack = 1;	
		}
		else
		{
			/* Screen command. */
			cmd = *(uart0Arry); /* command sent from labview */
		}
		
		uart0ArryIndex = 0; /* reset to wrap around */
	}	
	else
	{
		/* if not at the end, increment the index
		/* increment uart0ArryIndex and continue. */		
		uart0ArryIndex++;	
	}
	
	/* We should also restart the array if the
	   packet was not completed.  Every transmission
	   should start at zero index.  We could use
	   sizeof array to find this out. */
	
	/* Need to find out which button was activated by labview*/
	/*
	switch(*uart0Arry)
	{
		case 1: blueBut = 1;
		case 2: redBut = 1;
		case 3: blueBut = 1;
		case 4: redBut = 1;
	}
	if (*uart0Arry == 1)
	{
		blueBut = 1;	
	}
	if (*uart0Arry == 2)
	{
		redBut = 1;
	}	
	if (*uart0Arry == 3)
	{
		blkBut = 1;	
	}
	if (*uart0Arry == 4)
	{
		hcoeffBut = 1;
	}	
	if (*uart0Arry == 5)
	{
		sigBut = 1;	
	}
	if (*uart0Arry == 6)
	{
		filtrBut = 1;
	}*/
		
	/* remember that the array starts at 0 */
	/* when we come pass the array size, reset
	   the index. */
	/*if(uart0ArryIndex==BYTESIZE*COEFFSIZE+1)
	{
		uart0ArryIndex = 0; /* reset to wrap around */
	//}
	
	/*if(uart0ArryIndex-1<0) /* if 0xAA is at the end of the array */
	/*{
		if(*(uart0Arry+BYTESIZE*COEFFSIZE)==COEFFENDFLG)
		{
			pack = 1;
		}	
	}
	else /* 0xAA is not at the end of the array */
	/*{
		if(*(uart0Arry+(uart0ArryIndex-1))==COEFFENDFLG)
		{
			pack = 1;
		}	
	}	*/
}