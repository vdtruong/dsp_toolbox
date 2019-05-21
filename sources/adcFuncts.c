/* 11/1/09	ADC Functions and routines. */

/********* Prototypes **************************************************/
void adcSetup(void);
void adcInit(void);
void startAdcSmpl(void);
void stopAdcSmpl(void);
uint16 *adcArryPtr(void);
uint16 *bldAdcResBaseYCoord(void);
uint16 *bldLcdAdcBaseYCoord(void);
uint16 *bldAdcResYCoord(uint16 *adcResBaseYCoordPtr, uint16 *adcResPtr,
												uint16 *lcdAdcBaseYCoordPtr);
uint16 *bldAdcResYCoordForYFir(uint16 *adcResBaseYCoordPtr, int32 *yFirPtr,
												uint16 *lcdAdcBaseYCoordPtr);
lcdCoord *bldLcdAdcRes(uint16 *adcResYCoordPtr, uint16 *xCoordPtr);
cmndXhyh 
	*bldXhyhAdcCmnd(uint8 cmnd, lcdCoord *adcYCoordPtr, uint16 *xCoordPtr);
/***********************************************************************/

/******************** Functions/Routines begin ********************************/
void adcSetup(void)
{
	/**** initialize Interrupt Controller ****/
	
	//MCF_INTC0_ICR49 = MCF_INTC_ICR_IP(1) | MCF_INTC_ICR_IL(1);
	//MCF_INTC0_ICR50 = MCF_INTC_ICR_IP(2) | MCF_INTC_ICR_IL(1);
	//MCF_INTC0_ICR51 = MCF_INTC_ICR_IP(3) | MCF_INTC_ICR_IL(1);
	//MCF_INTC0_IMRH &= ~MCF_INTC_IMRH_INT_MASK49; // ADCA conversion complete
	//MCF_INTC0_IMRH &= ~MCF_INTC_IMRH_INT_MASK50; // ADCB conversion complete
	//MCF_INTC0_IMRH &= ~MCF_INTC_IMRH_INT_MASK51; // ADC Zero Crossing or Limit Error
	//MCF_INTC0_IMRL &= ~MCF_INTC_IMRL_MASKALL; // global 
	//asm (move.w #0x2000,SR); // unmask interrupts in Core status register
	
	
	/**** initialize General Purpose I/O Module ****/

	MCF_GPIO_PANPAR = MCF_GPIO_PANPAR_AN0_AN0 |
	                  MCF_GPIO_PANPAR_AN1_AN1 |
	                  MCF_GPIO_PANPAR_AN2_AN2 |
	                  MCF_GPIO_PANPAR_AN3_AN3 |
	                  MCF_GPIO_PANPAR_AN4_AN4 |
	                  MCF_GPIO_PANPAR_AN5_AN5 |
	                  MCF_GPIO_PANPAR_AN6_AN6 |
	                  MCF_GPIO_PANPAR_AN7_AN7 ;  
	                  // all ANn pins set for their primary function
}
void adcInit(void)
{
	/**** initialize Analog-to-Digital Converter (possibility using CFInit) ****/

	MCF_ADC_POWER &= ~(MCF_ADC_POWER_PD0 | MCF_ADC_POWER_PD1 | MCF_ADC_POWER_PD2);
		// power-up converter A, converter B and voltage reference circuit 

	while (MCF_ADC_POWER & (MCF_ADC_POWER_PSTS0 | MCF_ADC_POWER_PSTS1 | MCF_ADC_POWER_PSTS2)) 
	{}; // stay here as long as converter A, B and voltage reference circuit are power-down
	
	MCF_ADC_CTRL1 = MCF_ADC_CTRL1_EOSIE0    |  // ADCA end of scan interrupt enable
	                MCF_ADC_CTRL1_CHNCFG(0) |  // all inputs as single ended
	                MCF_ADC_CTRL1_SMODE(2)  |  // 0 - once sequential, 1 - once parallel,
	                                           // 2 - loop sequential, 3 - loop parallel,
	                                           // 4 - triggered sequential, 5-triggered parallel           
	                MCF_ADC_CTRL1_STOP0;       // stop until button pushing

	if (SYSTEM_CLOCK_KHZ % 2000) // set conversion time as short as possible but don't exceed 5MHz frequency
		MCF_ADC_CTRL2 = MCF_ADC_CTRL2_DIV(SYSTEM_CLOCK_KHZ / 2000); // divison remainder != 0	
	else
		MCF_ADC_CTRL2 = MCF_ADC_CTRL2_DIV((SYSTEM_CLOCK_KHZ / 2000) - 1); // divison remainder = 0
					
	/* choosing which channel to use */
	MCF_ADC_ADLST1 = MCF_ADC_ADLST1_SAMPLE0(0)/* | // sample slot 0 for channel 0
	                 MCF_ADC_ADLST1_SAMPLE1(4) | // sample slot 1 for channel 4
	                 MCF_ADC_ADLST1_SAMPLE2(5) | // sample slot 2 for channel 5
	                 MCF_ADC_ADLST1_SAMPLE3(6)*/ ; // sample slot 3 for channel 6
	                 
	MCF_ADC_ADSDIS = MCF_ADC_ADSDIS_DS1; // disable SAMPLE1 slot (and all higher !!)
	
	MCF_ADC_ADOFS0 = MCF_ADC_ADOFS_OFFSET(0); 
	/*MCF_ADC_ADOFS1 = MCF_ADC_ADOFS_OFFSET(2047);
	MCF_ADC_ADOFS2 = MCF_ADC_ADOFS_OFFSET(2047);
	MCF_ADC_ADOFS3 = MCF_ADC_ADOFS_OFFSET(2047); */
	// offset value (0 - positive results, 2047 - signed results, 4095 - negative results)			
}
void startAdcSmpl(void)
{
	/* write 1 to START0 to initiate conversion */
	MCF_ADC_CTRL1 |= MCF_ADC_CTRL1_START0/*(uint16)0x2000*/;	
}
void stopAdcSmpl(void)
{
	/* write 1 to STOP0 to end conversion */
	MCF_ADC_CTRL1 |= MCF_ADC_CTRL1_STOP0/*(uint16)0x4000*/;	
}
uint16 *adcArryPtr(void)
{
	int i;	
	static uint16 adcRes[ARRAYSIZE];
	
	for(i=0; i<ARRAYSIZE; i++){
		if ((MCF_ADC_ADSTAT & MCF_ADC_ADSTAT_EOSI0) == MCF_ADC_ADSTAT_EOSI0)
		{
			MCF_ADC_ADSTAT &= ~MCF_ADC_ADSTAT_EOSI0; // clear interrupt flag
			*(adcRes+i) = MCF_ADC_ADRSLT0;
		}
	}
	
	return adcRes;
}
uint16 *bldAdcResBaseYCoord(void)
{
	uint16 i;
	static uint16 adcResBaseYCoord[LCDADCYCOORDLGTH];

	/* highest value of adc res. */
	/* adcResBaseYCoord(0) = ADCRES3_3V */
	*adcResBaseYCoord  = ADCRES3_3V; /* ADCRES3_3V = 32760 */    
	
	/* Range of SEV */
	/* create an array between 32760 and 0,
		 starting with 32760 and subtracting
		 ADCRESRES downward. 
		 This is the adc result array that will 
		 match closely to the raw adc result from
		 the adc conversion.  The difference will
		 be within ADCRESRES, the resolution.  */
	for(i=1; i<LCDADCYCOORDLGTH; i++)
 	{
 		*(adcResBaseYCoord+i) = 
 		*(adcResBaseYCoord + i-1) - ADCRESRES; 
 	}
 	
 	return adcResBaseYCoord;
}
uint16 *bldLcdAdcBaseYCoord(void)
{
	uint16 i;
	static uint16 lcdAdcBaseYCoord[LCDADCYCOORDLGTH];
	
	/* Range of LCD Y coords. */
	/* pixel correspondence of lcdAdcBaseYCoord.
		 Should be between STARTYPIXEL and STARTYPIXEL+LCDADCYCOORDLGTH. 
		 Start at STARTYPIXEL and add 1 until
		 we get to STARTYPIXEL+LCDADCYCOORDLGTH. */
	*lcdAdcBaseYCoord  = STARTYPIXEL;    
	
	for(i=1; i<LCDADCYCOORDLGTH; i++)
 	{
 		/* exact pixel on LCD screen */
 		*(lcdAdcBaseYCoord+i) = 
 			*(lcdAdcBaseYCoord + i-1) + (uint16)YPIXELRES; 
 	}
 	
 	return lcdAdcBaseYCoord;
}
uint16 *bldAdcResYCoord(uint16 *adcResBaseYCoordPtr, uint16 *adcResPtr,
												uint16 *lcdAdcBaseYCoordPtr)
{
	/* find the y coord. of each adc result on the LCD screen */
	static uint16 adcResYCoord[ARRAYSIZE];
	uint16 j, m, stop=0;
	vuint16 diff=0;	
	
  j = 0;
  /* go through each adc result,
     find the corresponding adcResBaseYCoord,
     then find the lcd y coord.. */
  for(m=0; m<ARRAYSIZE; m++)
  {
  	do 
  	{
  		/* may want to use ++ to increment instead of indexing
  		   by m.  This cost more time than just ++. */
  		diff = *(adcResBaseYCoordPtr+j) - *(adcResPtr+m);  			
  		if((diff <= ADCRESRES && diff >= 0) 
  											 || j==LCDADCYCOORDLGTH)
  		{
  			/* 	stop whenever the absolute diff is within ADCRESRES
  					or at the end of adcResBaseYCoord array. */
  			stop=1;
  		}
  		else
    	{
    		j++;	
    	}
  	} while(!stop);
  	/* Each time you have gone through the do while loop,
  	   you should have found one lcd y coord. Or else,
  	   you have come to the end of the search in the
  	   adcResBaseYCoordPtr array. */
  	stop = 0;
  	/* assign the lcd y coord. */
  	*(adcResYCoord+m) = *(lcdAdcBaseYCoordPtr+j);  
  	j=0;
  	/* point back to the
  		 begining of the array */
  	//adcResBaseYCoordPtr+j;
  	/* m does not necessarily correspond to j, you could find
  	   adcResYCoord(m=0) at adcResBaseYCoordPtr(j=4) which corresponds
  	   to lcdAdcBaseYCoordPtr(j=4). Or you could find adcResYCoord(m=0) 
  	   at adcResBaseYCoordPtr(j=0) which corresponds to 
  	   lcdAdcBaseYCoordPtr(j=0).  In this case your adcResPtr(m=0) would
  	   be 32760, the first reference adcResBaseYCoordPtr value. 
  	   Basically, you take your adc result and go down the adcResBaseYCoordPtr
  	   array and find a matching value within the criteria.  Then you take
  	   that index and grab the lcd y coord. that matches that index. You're
  	   trying to find the lcd y coord. that matches your raw adc result. */
  }
  
  return adcResYCoord; /* returns pointer to array */	
}
uint16 *bldAdcResYCoordForYFir(uint16 *adcResBaseYCoordPtr, int32 *yFirPtr,
												uint16 *lcdAdcBaseYCoordPtr)
{
	/* find the y coord. of each YFir result on the LCD screen */
	static uint16 adcResYCoord[ARRAYSIZE];
	uint16 j, m, stop=0;
	vuint16 diff=0;	
	
  j = 0;
  /* go through each adc result,
     find the corresponding adcResBaseYCoord,
     then find the lcd y coord.. */
  for(m=0; m<ARRAYSIZE; m++)
  {
  	do 
  	{
  		/* may want to use ++ to increment instead of indexing
  		   by m.  This cost more time than just ++. */
  		diff = *(adcResBaseYCoordPtr+j) - *(yFirPtr+m);  			
  		if((diff <= ADCRESRES && diff >= 0) 
  											 || j==LCDADCYCOORDLGTH)
  		{
  			/* 	stop whenever the absolute diff is within ADCRESRES
  					or at the end of adcResBaseYCoord array. */
  			stop=1;
  		}
  		else
    	{
    		j++;	
    	}
  	} while(!stop);
  	/* Each time you have gone through the do while loop,
  	   you should have found one lcd y coord. Or else,
  	   you have come to the end of the search in the
  	   adcResBaseYCoordPtr array. */
  	stop = 0;
  	/* assign the lcd y coord. */
  	*(adcResYCoord+m) = *(lcdAdcBaseYCoordPtr+j);  
  	j=0;
  	/* point back to the
  		 begining of the array */
  	//adcResBaseYCoordPtr+j;
  	/* m does not necessarily correspond to j, you could find
  	   adcResYCoord(m=0) at adcResBaseYCoordPtr(j=4) which corresponds
  	   to lcdAdcBaseYCoordPtr(j=4). Or you could find adcResYCoord(m=0) 
  	   at adcResBaseYCoordPtr(j=0) which corresponds to 
  	   lcdAdcBaseYCoordPtr(j=0).  In this case your adcResPtr(m=0) would
  	   be 32760, the first reference adcResBaseYCoordPtr value. 
  	   Basically, you take your adc result and go down the adcResBaseYCoordPtr
  	   array and find a matching value within the criteria.  Then you take
  	   that index and grab the lcd y coord. that matches that index. You're
  	   trying to find the lcd y coord. that matches your raw adc result. */
  }
  
  return adcResYCoord; /* returns pointer to array */	
}
lcdCoord *bldLcdAdcRes(uint16 *adcResYCoordPtr, uint16 *xCoordPtr)
{
	lcdCoord *lcdAdcResPtr;
	uint16 i;
	
	for(i=0; i<ARRAYSIZE; i++)
	{
		lcdAdcResPtr->xCoordArry[i] = *(xCoordPtr+i);
		lcdAdcResPtr->yCoordArry[i] = *(adcResYCoordPtr+i);
	}
		
	return lcdAdcResPtr; /* returns a struct. pointer */	
}
cmndXhyh 
	*bldXhyhAdcCmnd(uint8 cmnd, lcdCoord *adcYCoordPtr, uint16 *xCoordPtr)
{
	/* 	this function is used for line_to_xhyh,
			plot_xyhy and set_xhyh. Just use the 
			command (cmnd) needed. But this command
			is used for adc. only. */
	static cmndXhyh xhyhAdcCmnd[ARRAYSIZE];
	uint8 i;
	
	for(i=0;i<ARRAYSIZE;i++)
	{
		xhyhAdcCmnd[i].cmnd	=	cmnd;
		xhyhAdcCmnd[i].xlsb = *(xCoordPtr+i)&0xFF;/* use *x2Ptr++ ? */
		xhyhAdcCmnd[i].xmsb = (*(xCoordPtr+i)>>8)&0xFF;
		xhyhAdcCmnd[i].ylsb	= adcYCoordPtr->yCoordArry[i]&0xff;
		xhyhAdcCmnd[i].ymsb	= (adcYCoordPtr->yCoordArry[i]>>8)&0xff;
	}
	return xhyhAdcCmnd; /* returns pointer to array of structure */
}
/******************************************************************/