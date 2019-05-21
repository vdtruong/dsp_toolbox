/* 10/31/09 Functions used to repack the hCoeffs. from the serial transfer and
   					other hCoeffs. preprocessing.
   2/11/12	Added another termination criteria in the int16 *packhCoeff(void)
            routine to stop the function.  Added 
            || uart0ArryIndex == SETCOEFFSIZE/2.  Without this, the routine
            could run on forever if 0xaa was not found in the uart
            hCoeff transmitted array. */
   					

/* prototypes */   					
int16 *packhCoeff(void);
lcdCoord bldhCoeffYCoord(int16 *hCoeffPtr, int16 *y1s, uint16 *y2);
lcdCoord *bldhCoeffYCoordPtr(int16 *hCoeffPtr, int16 *y1s, uint16 *y2);
uint16 *bldx2(void);
uint16 *formHCoeffXCoords(int16 *hCoeffPtr);
int16 *bldY1s(void);
uint16 *bldY2(void);
cmndXhyh *bldXhyhHcoeffCmnd(uint8 cmnd, lcdCoord *hCoeffYCoordPtr, 
														uint16 *x2Ptr, int16 *hCoeffPtr);
uint8 hCoeffOutOfBound(int16 *hCoeffPtr);
//uint16 hCoeffSize(int16 *hCoeffPtr);
uint16 *bldXCoord(uint16 xCoordLgth);
uint16 *bldXCoordRepl(uint16 xCoordLgth);
/*******************************************/

/* functions begin */
int16 *packhCoeff(void)
{
	/* This function returns a pointer of int16. */
	/* this function repacks the 2 8-bit
		 numbers into one 16-bit number. */
	vuint16 a = 0;
	uint8 done = 0, c;
	uint16 d = 0;
	static int16 hCoeff[SETCOEFFSIZE/2];

	/* reset the pack flag */	
	pack = 0;
  		
  	do {
  		/* Takes two bytes of the uart0Arry to form one h coefficient. */
    	for (c=0; c<BYTESIZE; c++){
    		/* if we find the constants 0xaa, we are done with the
    	   uart0Arry. 
    	   It will stop when it sees the 0xaa flag. That's why it is 
    	   important that the h coefficients array does not have 0xaa in
    	   the middle.  This may be solved later by some other means. 
	   		 The 1st element of the hCoeff array is the array size. The size
         is also made up of 2 bytes. 
         Or we are have ran through the set size of
    	   the hCoeff array, which is SETCOEFFSIZE/2. */
    		if(*(uart0Arry+uart0ArryIndex)==0xaa || 
    	   			uart0ArryIndex == SETCOEFFSIZE/2) /* need to subtract 1? */
    	   {
    	     	uart0ArryIndex = 0;
    	     	done = 1;
    	   }
    	   else
    	   {
    	   	*(hCoeff+a) = (*(hCoeff+a)<<8)|(*(uart0Arry+uart0ArryIndex));
	         uart0ArryIndex++;	
    	   } 
    	}
    	a++;
  	} while (!done);
  
  	/* Clear uart0Arry after we pack hCoeff. */
   for (d=0; d<SETCOEFFSIZE; d++){
    	*(uart0Arry + d) = 0;
   }
    
  	/* returns pointer to the h coefficient array. Remember the
     1st element is the size of the h coeff. array.*/	
  	return hCoeff; 
}
/*uint16 hCoeffSize(int16 *hCoeffPtr)
{
	
}*/
uint8 hCoeffOutOfBound(int16 *hCoeffPtr)
{
	/* This function returns a uint8 value. */
	/* h coeff. data starts at index 1 */
	/* h coeff. size is delclared at index 0 */
	uint16 m=1; 
	uint8 stop = 0;
	uint8 outOfBnd = 0;
	uint16 halfSize, stopB4Indx;
	
	halfSize = (*hCoeffPtr)/2; /* size of half the array */
	stopB4Indx = halfSize + 1; /* stop before this index */
	/* If you have 400 coeffs., you would search from m=1
	   to m = 200, your stopB4Indx would be 201. */
	/* search for any hCoeffs. that are out
		of bound between -4915 and 4915. */
	do
	{
		if( *(hCoeffPtr+m) > 4915 || *(hCoeffPtr+m) < -4915 )
		{
			outOfBnd = 1;
			stop = 1;
		}
		else
		{
			m++;
		}
	} while(!stop && m<stopB4Indx);
	/* *hCoeffPtr is the first element,
     it tells the h coeff. packet size. We don't
     need to search the entire size, we only import
     half of the size because the array is even 
     symmetric. */
	return outOfBnd;	
}
lcdCoord bldhCoeffYCoord(int16 *hCoeffPtr, int16 *y1s, uint16 *y2)
{
	/* This functions returns a lcdCoord value. */
	/* This routine is for the h coeff. array, not
	   the adc result array. 
	   We match the h coeff. result to the y coord.
	   on the Lcd screen. 10-8-11 */

	lcdCoord hCoeffYCoord;
	uint8 stop;
	volatile int16 diff;
	uint16 j, m, halfSze = 0, stopB4Indx;
	
	halfSze = (*hCoeffPtr)/2; /* size of half the array */
	stopB4Indx = halfSze + 1; /* stop before this index */
	
	stop = 0;
	diff = 0;
	
	/* find the y coord. of each hCoeff on the LCD screen */
	/* j is the y1s and y2 index in the spreadsheet. y2 is the physical
	   y coordinate on the lcd screen. Right now we set it to
	   start at 75 and ends at 375. Because they share the
	   same index, we can match the coeffs. to the y coord.
	   on the lcd screen. y1s is the h coeffs value after we
	   multiply its floating value with 2^15.  We do this so
	   we can send it through the rs232 comm. port without using
	   floating number.  10/8/11 */
  	j = 0;
  	/* hcoeff starts at index 1 */
  	/* (*hCoeffPtr) is the first element,
     it tells us how many coeffs. there are. 
     We only need to go half way of the array
     since we have an even symmetric array. */
  	for(m=1; m<stopB4Indx; m++)
  	{
  		do 
  		{
  			diff = *(hCoeffPtr+m) - *(y1s+j);  			
  			if((diff <= (int16)DIFFABSVAL && diff >= (int16)-DIFFABSVAL) 
  																	|| j==Y1SLENGTH)
  			{
  				/* stop whenever the absolute diff is within DIFFABSVAL
  					or at the end of y1s array. */
  				stop=1;
  			}
  			else
    		{
    			j++;	
    		}
  		} while(!stop);
  		stop = 0;
  		/* This array starts at index 0. */
  		hCoeffYCoord.yCoordArry[m-1] = *(y2+j);  
  		j=0;
  	}
  	/* returns structure */
  	/* this is only half of the h coeff. array, we have
     even symmetry array.  When we display the h coeff graph,
     we need to repeat this again in reverse. Esentially mirroring. */ 
  	return hCoeffYCoord; 	
}
/* routine does not work, may come back to work on it later */
lcdCoord *bldhCoeffYCoordPtr(int16 *hCoeffPtr, int16 *y1s, uint16 *y2)
{
	/* This function returns a pointer of lcdCoord. */
	/* Structure pointer and its address place holder.
		We need this so the pointer can have 
		an address to hold on to, if not, it
		will change other variable values. */
	lcdCoord *hCoeffYCoordPtr, addrHolder; 
	uint8 j, m, stop;
	volatile int16 diff;
	
	hCoeffYCoordPtr = &addrHolder;
	stop = 0;
	diff = 0;
	
	/* find the x,y coords. of each hCoeff on the LCD screen */
  	j = 0;
  	for(m=0; m<COEFFSIZE; m++)
  	{
  		do 
  		{
  			diff = *(hCoeffPtr+m) - *(y1s+j); 
  			/* diff needs to be abs. 66 and j limited to Y1SLENGTH */ 			
  			if((diff <= (int16)DIFFABSVAL && diff >= 
  								(int16)-DIFFABSVAL) || j==Y1SLENGTH)
  			{
  				stop=1;
  			}
  			else
    		{
    			j++;	
    		}   	
  		} while(!stop);
  		stop = 0;
  		hCoeffYCoordPtr->yCoordArry[m] = *(y2+j)/*y2[j]*/;  
  		j=0;
  	}
  	*hCoeffYCoordPtr = addrHolder;
  	return hCoeffYCoordPtr;	
}
uint16 *bldx2(void)
{	
	/* This is for the h coeffs. x coordinates. 
	   It builds the entire x coordinates from 
     the starting pixel to the end of the h coeff. 
     graph. */
	static uint16 x2[SETCOEFFSIZE+1];
	uint16	i;
	
	/* Exact xcoord pixel on the LCD screen.
	   The first coordinate is at STARTXPIXEL. */
	*x2	 = STARTXPIXEL; 
	
	/* This is an array of 401 points.  The first one
	   is at index 0, the last one is at 400. */ 
	for(i=1; i<SETCOEFFSIZE+1; i++)
 	{
 		/* exact x coord pixel on the LCD screen.
 		   The next one is the last one add to XPIXELRES. */
 		*(x2+i) = *(x2 + i-1) + XPIXELRES;  
 	}	
 	
 	return x2;
}
uint16 *formHCoeffXCoords( int16 *hCoeffPtr)
{
	/* This function returns a pointer of uint16. */
	/* This fuction calculates the h coeff. x coords. based
	   on how many coefficients we have.  Each coefficient
	   will be spaced evenly to fit the SETCOEFFXCOORD points
	   on the screen.  The graph will also be centered in the
	   h coeff. screen. */
	static uint16 hCoeffXCoords[SETCOEFFXCOORD+1];
	uint16	i;
	uint8 res, rem, halfOfRem;
	
	
	if (*hCoeffPtr == 0)
	{
		/* if the size is zero, set res to 1 so
	   we won't be dividing by zero. */
		res = 1;
	}
	else
	{
		res = SETCOEFFXCOORD/(*hCoeffPtr);
	}
	
	rem = SETCOEFFXCOORD - (res * (*hCoeffPtr));
	halfOfRem = rem / 2;
	
	for(i=0; i<*hCoeffPtr; i++)
	{
		/* Each x coord. is separated by the resolution.  This value
	 	   is shifted over by halfOfRem and the STARTXPIXEL.  This is
	 	   so the graph can be in the center. */
	 	*(hCoeffXCoords + i) = i*res + halfOfRem + STARTXPIXEL;
	}	
 	
 	return hCoeffXCoords;
	
}
int16 *bldY1s(void)
{
	/* This routine is for the h coeffs, not the adc array. */
	/* This routine starts at y = 0.150
		and counts down to -0.150. 
      Y1s is for the h coefficients. */

	static int16 y1s[Y1SLENGTH];
	static int16 i;
	static int16 rem;
	
	*y1s = Y1STARTSCL; /* 0.15 scaled to 2^15 */
	
	for(i=1; i<Y1SLENGTH; i++)
 	{
 		rem = i-((i>>1)<<1);
 		*(y1s+i)= (rem>0)? 	*(y1s + i-1) - (int16)Y1SRESODD : 
 												*(y1s + i-1) - (int16)Y1SRESEVEN;
 					  						/* scaled hCoeff by 2^15;
 					  						   subtract 66 if i is odd, 65 if
 					  						   even. */
 	}	
 	
 	return y1s;
}
uint16 *bldY2(void)
{
	/* This routine is for the h coeffs. array, not
           the adc array. */

	uint16 i;
	static uint16 y2[Y1SLENGTH];

	/* pixel correspondence of y1s */
	*y2  = STARTYPIXEL;    
	
	for(i=1; i<Y1SLENGTH; i++)
 	{
 		/* exact pixel on the LCD screen */
 		*(y2+i)	 = *(y2 + i-1)  + (uint16)YPIXELRES; 
 	}
 	
 	return y2;
}
cmndXhyh 
	*bldXhyhHcoeffCmnd(uint8 cmnd, lcdCoord *hCoeffYCoordPtr, uint16 *x2Ptr,
	                   int16 *hCoeffPtr)
{
	/* This function returns a pointer of cmndXhyh. */
	/* This function builds the xh and yh coeff. command to
	   be sent to the lcd screen. Each coord. will have a 
	   5 bytes command. */
	/* This function is used for line_to_xhyh,
		 plot_xyhy and set_xhyh. Just use the 
		 command (cmnd) needed. But this command
		 is used for hCoeffs. only. */
	/* We'll use SETCOEFFSIZE for now, but we may not send
	   this much.  It depends on the user.  But the limit
	   is SETCOEFFSIZE for now.  */		
	
	static cmndXhyh xhyhHcoeffCmnd[SETCOEFFSIZE];
	uint16 i, j=1, symIndx;
	
	symIndx = ((*hCoeffPtr)/2)-1;
	
	/* The size of the coefficient array is from the
	   1st element of the packet sent over (after we have
	   repacked into 16 bits word).  Therefore,
     we need to start at index 1.  This is where we
	   need to build the entire h coeffs. array from
     half of the array that was sent from labview. */ 
	
	for(i=0;i < *hCoeffPtr;i++)
	{
		if(i > symIndx ){
			/* The second half of the h coeffs. array. 
				 We traverse (mirror) the h coeffs. array toward
				 the beginning to build the rest of the
				 xhyh h coeff command. */
			xhyhHcoeffCmnd[i].cmnd	=	cmnd;
			xhyhHcoeffCmnd[i].xlsb 	= *(x2Ptr+i)&0xFF;/* use *x2Ptr++ ? */
			xhyhHcoeffCmnd[i].xmsb 	= (*(x2Ptr+i)>>8)&0xFF;/* masking */
			xhyhHcoeffCmnd[i].ylsb	= hCoeffYCoordPtr->yCoordArry[i-j]&0xff;
			xhyhHcoeffCmnd[i].ymsb	= (hCoeffYCoordPtr->yCoordArry[i-j]>>8)&0xff;
			j+=2;
		}
		else { 
			/* The first half of the h coeffs. array. */
			xhyhHcoeffCmnd[i].cmnd	=	cmnd;
			xhyhHcoeffCmnd[i].xlsb 	= *(x2Ptr+i)&0xFF;/* use *x2Ptr++ ? */
			xhyhHcoeffCmnd[i].xmsb 	= (*(x2Ptr+i)>>8)&0xFF;/* masking */
			xhyhHcoeffCmnd[i].ylsb	= hCoeffYCoordPtr->yCoordArry[i]&0xff;
			xhyhHcoeffCmnd[i].ymsb	= (hCoeffYCoordPtr->yCoordArry[i]>>8)&0xff;
	  }
	}
	/* Now we return the entire h coeff. array, the even symmetric
     array. */
	/* returns pointer to array of structure */
	return xhyhHcoeffCmnd; 
}
/* build a generic x coord. axis for the adc results screen. */
uint16 *bldXCoord(uint16 xCoordLgth)
{
	/* This is for the adc array x coordinates. */

	static uint16 xCoord[ARRAYSIZE];
	uint16	i;
	
	/* exact xcoord pixel on LCD screen */
	*xCoord	 = ADCLFTBRD; /* starts at pixel 48 */ 
	 
	for(i=1; i<xCoordLgth+1; i++)
 	{
 		/* exact x coords. pixels on LCD screen */
 		*(xCoord+i) = *(xCoord + i-1) + ADCXPIXELRES;  
 	}	
 	
 	return xCoord;
}
/* build a generic x coord. axis for the adc results screen. */
uint16 *bldXCoordRepl(uint16 xCoordLgth)
{
	/* This is for the adc array x coordinates. */

	static uint16 xCoordRepl[ARRAYSIZE];
	uint16	i;
	
	/* exact xcoord pixel on LCD screen */
	*xCoordRepl	 = ADCLFTBRD; /* starts at pixel 48 */ 
	 
	for(i=1; i<xCoordLgth+1; i++)
 	{
 		/* exact x coords. pixels on LCD screen */
 		*(xCoordRepl+i) = *(xCoordRepl + i-1) + ADCXPIXELRES;  
 	}	
 	
 	return xCoordRepl;
}