/* 11/1/09	Xhyh command functions. */

//#include "ezLCDFuncts.c"

/************************** prototypes *******************************/
cmndXhyh *bldXhyhCmnd(uint8 cmnd, uint16 xcoord, uint16 ycoord);
uint8 *bldXhyhCmndByteArry(cmndXhyh *xhyhCmndPtr, uint16 noOfCoord);
uint8 *bldAdcXhyhCmndByteArry(cmndXhyh *xhyhCmndPtr, uint16 noOfCoord);
plotXhyh *bldPlotXhyhCmnd(lcdCoord *hCoeffYCoordPtr, uint16 *x2Ptr);
cmndXhyh *bldXhyhCmndArry(uint8 cmnd, uint16 *xCoord, uint16 *yCoord);
/*********************************************************************/

/* functions begin */
cmndXhyh *bldXhyhCmnd(uint8 cmnd, uint16 xcoord, uint16 ycoord)
{
	/* 	this function is used for any xhyh command. 
			It builds the xhyh command structure from the
			given coords. and cmnd.  You still need to use
			bldXhyhCmndByteArray() to break it into
			five bytes.  Then use sndRecvLcdQspiByte() to send
			each byte. */
	cmndXhyh *xhyhCmnd, addrHolder;
	
	/* need an address holder for xhyhCmnd */
	xhyhCmnd = &addrHolder;
	
	xhyhCmnd->cmnd	=	cmnd;
	xhyhCmnd->xlsb  = xcoord&0xFF;
	xhyhCmnd->xmsb  = (xcoord>>8)&0xFF;
	xhyhCmnd->ylsb	= ycoord&0xff;
	xhyhCmnd->ymsb	= (ycoord>>8)&0xff;
	
	/* Need to pass the value to xhyhCmnd */
	*xhyhCmnd = addrHolder;
	
	return xhyhCmnd; /* returns pointer of structure */
}
uint8 *bldXhyhCmndByteArry(cmndXhyh *xhyhCmndPtr, uint16 noOfCoord)
{
	/* Need to make it static whenever returning an array,
		 this is so the information won't get corrupted by
		 other functions. The array is set for COEFFSIZE*XHYHBYTES bytes
		 or the largest necessary.  We may only need one if we
		 are doing one xhyh command at a time. There may be a better
		 way to do this in the future. */
	static uint8 xhyhCmndByteArry[/*COEFFSIZE**/XHYHBYTES];
	vuint8 i, a;
	vuint16 j;
	uint8 data[XHYHBYTES];
			
	for(i=0, j=0; i<noOfCoord, j<(noOfCoord-1)*(XHYHBYTES-1)+1; 
													 i++, j+=XHYHBYTES-1)
	{
		/* For each coord., we need to split it
		   into five bytes to be sent. 
		   This is because each coord. is a struct.
		   of five bytes. */
		*(data)	 = (xhyhCmndPtr+i)->cmnd;
		*(data+1)= (xhyhCmndPtr+i)->xmsb;
		*(data+2)= (xhyhCmndPtr+i)->xlsb;
		*(data+3)= (xhyhCmndPtr+i)->ymsb;
		*(data+4)= (xhyhCmndPtr+i)->ylsb;
		
		for(a=0; a<XHYHBYTES; a++)
		{
			*(xhyhCmndByteArry+(i+j+a)) = *(data+a);	
		}
	}
	
	return xhyhCmndByteArry; /* return pointer to array */
}
/* maybe we can use only one of these versions later */
uint8 *bldAdcXhyhCmndByteArry(cmndXhyh *xhyhCmndPtr, uint16 noOfCoord)
{
	/* Need to make it static whenever returning an array,
		 this is so the information won't get corrupted by
		 other functions. The array is set for COEFFSIZE*XHYHBYTES bytes
		 or the largest necessary.  We may only need one if we
		 are doing one xhyh command at a time. There may be a better
		 way to do this in the future. */
	static uint8 adcXhyhCmndByteArry[/*ARRAYSIZE**/XHYHBYTES];
	vuint8 i, a;
	vuint16 j;
	uint8 data[XHYHBYTES];
			
	for(i=0, j=0; i<noOfCoord, j<(noOfCoord-1)*(XHYHBYTES-1)+1; 
													 i++, j+=XHYHBYTES-1)
	{
		/* For each coord., we need to split it
		   into five bytes to be sent. 
		   This is because each coord. is a struct.
		   of five bytes. */
		*(data)	 = (xhyhCmndPtr+i)->cmnd;
		*(data+1)= (xhyhCmndPtr+i)->xmsb;
		*(data+2)= (xhyhCmndPtr+i)->xlsb;
		*(data+3)= (xhyhCmndPtr+i)->ymsb;
		*(data+4)= (xhyhCmndPtr+i)->ylsb;
		
		for(a=0; a<XHYHBYTES; a++)
		{
			*(adcXhyhCmndByteArry+(i+j+a)) = *(data+a);	
		}
	}
	
	return adcXhyhCmndByteArry; /* return pointer to array */
}

plotXhyh *bldPlotXhyhCmnd(lcdCoord *hCoeffYCoordPtr, uint16 *x2Ptr)
{
	/* this function is used for plot_xhyh. */
	static plotXhyh plotXhyhCmnd[COEFFSIZE];
	uint8 i;
	
	for(i=0;i<COEFFSIZE;i++)
	{
		plotXhyhCmnd[i].cmnd	=	PLOT_XHYH;
		plotXhyhCmnd[i].xlsb 	= *(x2Ptr+i)&0xFF;/* use *x2Ptr++ ? */
		plotXhyhCmnd[i].xmsb 	= (*(x2Ptr+i)>>8)&0xFF;
		plotXhyhCmnd[i].ylsb	= hCoeffYCoordPtr->yCoordArry[i]&0xff;
		plotXhyhCmnd[i].ymsb	= (hCoeffYCoordPtr->yCoordArry[i]>>8)&0xff;
	}
	return plotXhyhCmnd; /* returns pointer */
}
/* This version passes in pointers to arrays 
   and build an array of structures. */
cmndXhyh *bldXhyhCmndArry(uint8 cmnd, uint16 *xCoord, uint16 *yCoord)
{
	/* 	this function is used for any xhyh command. 
			It builds the xhyh command structure from the
			given coords. and cmnd.  You still need to use
			bldXhyhCmndByteArray() to break it into
			five bytes.  Then use sndRecvLcdQspiByte() to send
			each byte. May be able to add an input parameter so we only
			have to build one coord. instead of the entire 
			array of coordinates. */
	static cmndXhyh xhyhCmndArry[ARRAYSIZE];
	uint16 i;
	
	for(i=0; i<ARRAYSIZE; i++)
	{
		(*(xhyhCmndArry+i)).cmnd = cmnd;
		(*(xhyhCmndArry+i)).xlsb = *(xCoord+i)&0xFF;
		(*(xhyhCmndArry+i)).xmsb = (*(xCoord+i)>>8)&0xFF;
		(*(xhyhCmndArry+i)).ylsb = *(yCoord+i)&0xFF;
		(*(xhyhCmndArry+i)).ymsb = (*(yCoord+i)>>8)&0xFF;
	}
	
	return xhyhCmndArry; /* returns pointer to array of structures */
}
