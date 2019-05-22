/* 11/1/09	ezLCD105 functions. */

#include "miscFuncts.c"
#include "adcFuncts.c"
#include "dspFuncts.c"

/* prototypes */
void lcdSendXhyhCmnd(uint16 index, cmndXhyh *xhyhCmndPtr, uint8 datQ);
uint16 *lcdSendXhyhCmnd2(uint16 index, cmndXhyh *xhyhCmndPtr, uint8 datQ);
void lcdSndIndvXhyhCmnd(uint8 index, cmndXhyh *xhyhCmndPtr);
void lcdSendCmnd(uint8 bytes, uint8 *cmndPtr);
void lcdSendXhyhCmndByte(cmndXhyh *xhyhCmndPtr);
uint16 *sndRecvLcdQspiByte(uint8 *data);
uint8 *sndRecvLcdQspiByte2(uint8 data); /* non pointer data */
uint8 sndRecvLcdQspiByte3(uint8 data);
uint16 sndRecvLcdQspiByte4(uint8 *data);
void sndLcdQspiByte(uint8 data);
uint16 *sndXhyhCmnd(uint8 cmnd, uint16 xcoord, uint16 ycoord);
void drawHcoeffBorder(void);
void drawHcoeffBox(void);
void drawHcoeffVertGrid(void);
void drawRawAdcBox(uint8 *setCol);
void drawHcoeffHorzGrid(void);
void touchProtocol(uint8 protocol);
void butDefLong(uint8 butNo, uint16 state, uint16 butUp, 
								uint16 butDwn, uint16 butDis, uint16 xcoord, 
								uint16 ycoord, uint8 width, uint8 height);
uint8 *Cls(void);
void setColRgb(uint8 red, uint8 green, uint8 blue);
void setBgColRgb(uint8 red, uint8 green, uint8 blue);
void setXhyh(uint16 xcoord, uint16 ycoord);
void vLineh(uint16 ycoord);
void hLineh(uint16 xcoord);
void boxhh(uint16 xcoord, uint16 ycoord);
void butState(uint8 butNo, uint8 state);
void lightBright(uint8 brightness);
uint8 *lightOff(void);
uint8 *lightOn(void);
void linToXhyh(uint16 xcoord, uint16 ycoord);
uint8 *plot(void);
uint8 *butAllUp(void);
uint8 *butDelAll(void);
void boxHhFill(uint16 xcoord, uint16 ycoord);
uint8 *fill(void);
void fillBound(uint8 red, uint8 green, uint8 blue);
uint8 *ping(void);
void drwhCoeffsDots(lcdCoord *hCoeffYCoordPtr, uint16 *x2Ptr, int16 *hCoeffPtr);
void clrhCoeffGrph(cmndXhyh *linToXhyhCmndPtrHst, uint8 *bkgClr, 
                   int16 *hCoeffPtr);
cmndXhyh *drwhCoeffGrph(int16 *hCoeffPtr, int16 *y1sPtr, uint16 *y2Ptr,
													uint16 *x2Ptr, uint8 *color);
void printChar(uint8 asciiChar);
void printString(uint8 size, uint8 *strPtr);
void redrwGrdLines(cmndXhyh *linToXhyhCmndPtrHst, int16 *hCoeffPtr);
void drwBordAndGrids(uint8 *setCol);
void drwDsplyTypeBox(void);
void drwGraphColorBox(void);
void drwBlueTchScrnBut(void);
void drwRedTchScrnBut(void);
void drwBlackTchScrnBut(void);
void drwhCoeffsTchScrnBut(void);
void drwSignalsTchScrnBut(void);
void drwFilteredTchScrnBut(void);
void drwYHcoeffsLegnd(void);
void drwXHcoeffsLegnd(void);
void drwAdcYLegnd(void);
void initScrn(void);
cmndXhyh *lcdTouchScreen(int16 *hCoeffPtr, int16 *y1sPtr, uint16 *y2Ptr,
												 uint16 *x2Ptr, uint16 *adcResBaseYCoordPtr, 
												 uint16 *lcdAdcBaseYCoordPtr,
												 uint16 *adcXCoordPtr);
cmndXhyh *monitorScreen(int16 *hCoeffPtr, int16 *y1sPtr, uint16 *y2Ptr,
												 uint16 *x2Ptr, uint16 *adcResBaseYCoordPtr, 
												 uint16 *lcdAdcBaseYCoordPtr,
												 uint16 *adcXCoordPtr);
/*************************************************************/

/* Functions/routines begin */
void lcdSendXhyhCmnd(uint16 index, cmndXhyh *xhyhCmndPtr, uint8 datQ)
{
	vuint8 a, addr=0;
	vuint16 i, j;
	uint8 data[XHYHBYTES];
	/* 	try to implement 2-d array
			using array of pointers */
	/* uint16 *rcvDat[XHYHBYTES]; */
	
	j=0;
	/* Need to send two/three coords. at a time.
		 each coord. has five bytes of information. 
		 Also, need 2-D array to store the rcvDat 
		 comming back from the ezlcd105. */
	for(i=index; i<index+datQ; i++)
	{
		/* increment in xhyhCmnd array */
		/* initialize the data to be sent */
		*(data)	 = (xhyhCmndPtr+i)->cmnd;
		*(data+1)= (xhyhCmndPtr+i)->xmsb;
		*(data+2)= (xhyhCmndPtr+i)->xlsb;
		*(data+3)= (xhyhCmndPtr+i)->ymsb;
		*(data+4)= (xhyhCmndPtr+i)->ylsb;
		
		/* write to rams five bytes of information. */
		for(a=0; a<5; a++)
		{	
			/* increment the transf. and cmnd addresses */
			addr = (i-index)+a+j;
									
			write_to_qspi_ram(MCF_QSPI_QAR_TRANS+addr,(uint16)*(data+a));	
			write_to_qspi_ram(MCF_QSPI_QAR_CMD	+addr,
												MCF_QSPI_QDR_CONT|0xbE00);
			/* the cont flag is for transfering more than one que entry */
		}	
		j+=4; /* we need to increment from 0,4 to 8 */	
	}
	//transfer xx elements of the queue, with csiv
	start_spi_trans(datQ*XHYHBYTES,1); 
	//read_from_qspi_ram(0);//here read all the que recv ram
}
uint16 *lcdSendXhyhCmnd2(uint16 index, cmndXhyh *xhyhCmndPtr, uint8 datQ)
{
	vuint8 a, addr=0;
	vuint16 i, j;
	uint8 data[XHYHBYTES];
	uint16 rcvDat[10];
	
	j=0;
	/* Need to send two/three coords. at a time.
		 each coord. has five bytes of information. */
	for(i=index; i<index+datQ; i++)
	{
		/* increment in xhyhCmnd array */
		/* initialize the data to be sent */
		*(data)	 = (xhyhCmndPtr+i)->cmnd;
		*(data+1)= (xhyhCmndPtr+i)->xmsb;
		*(data+2)= (xhyhCmndPtr+i)->xlsb;
		*(data+3)= (xhyhCmndPtr+i)->ymsb;
		*(data+4)= (xhyhCmndPtr+i)->ylsb;
		
		/* write to rams five bytes of information. */
		for(a=0; a<5; a++)
		{	
			/* increment the transf. and cmnd addresses */
			addr = (i-index)+a+j;
									
			write_to_qspi_ram(MCF_QSPI_QAR_TRANS+addr,(uint16)*(data+a));	
			write_to_qspi_ram(MCF_QSPI_QAR_CMD	+addr,
												MCF_QSPI_QDR_CONT|0xbE00);
			/* the cont flag is for transfering more than one que entry */
		}	
		j+=4; /* we need to increment from 0,4 to 8 */	
	}
	//transfer xx elements of the queue, with csiv
	start_spi_trans(datQ*XHYHBYTES,1);
	for(i=0; i<datQ*XHYHBYTES; i++)
	{	//here read all the que recv ram
		*(rcvDat+i) = read_from_qspi_ram(i);
	}
	return rcvDat;
}

void lcdSndIndvXhyhCmnd(uint8 index, cmndXhyh *xhyhCmndPtr)
{	/* 	This function only sends out the selected
			xhyhCmndPtr array index. */
	vuint8 a;
	uint8 data[XHYHBYTES];
	/* 	try to implement 2-d array
			using array of pointers */
	//uint16 *rcvDat[XHYHBYTES];
	
	/* increment in xhyhCmnd array */
	/* initialize the data to be sent */
	*(data)	 = (xhyhCmndPtr+index)->cmnd;
	*(data+1)= (xhyhCmndPtr+index)->xmsb;
	*(data+2)= (xhyhCmndPtr+index)->xlsb;
	*(data+3)= (xhyhCmndPtr+index)->ymsb;
	*(data+4)= (xhyhCmndPtr+index)->ylsb;
		
	/* write to rams five bytes of information. 
	   Not sending yet. */
	for(a=0; a<5; a++)
	{	/* 	write information to 5 transfer rams
	 			and command rams */
		write_to_qspi_ram(MCF_QSPI_QAR_TRANS+a,(uint16)*(data+a));	
		write_to_qspi_ram(MCF_QSPI_QAR_CMD	+a,
											MCF_QSPI_QDR_CONT|0x9E00);
		/* the cont flag is for transfering more than one que entry */
	}	
	/* transfer five elements of the queue, with csiv */
	start_spi_trans(XHYHBYTES,1); 
	read_from_qspi_ram(0);
}
void lcdSendCmnd(uint8 bytes, uint8 *cmndPtr)
{	/* 	this command only raise the c.s. line
			after it finishes transfering all the
			bytes. */
	vuint8 a;
	/* 	try to implement 2-d array
			using array of pointers */
	//uint16 *rcvDat[XHYHBYTES];
		
	/* write to rams 'bytes' of information. */
	for(a=0; a<bytes; a++)
	{										
		write_to_qspi_ram(MCF_QSPI_QAR_TRANS+a,(uint16)*(cmndPtr+a));	
		write_to_qspi_ram(MCF_QSPI_QAR_CMD	+a,
											MCF_QSPI_QDR_CONT|0x9E00);
		/* the cont flag is for transfering more than one que entry */
	}	
	/* transfer 'bytes' elements of the queue, with csiv */
	start_spi_trans(bytes,1); 
	read_from_qspi_ram(0);
}
void lcdSendXhyhCmndByte(cmndXhyh *xhyhCmndPtr)
{
	vuint8 i, j;
	uint8 data[XHYHBYTES];
	//uint16 rcvData[ARRAYSIZE]; /* temply. use array size */
	
	for(i=0; i<COEFFSIZE; i++)
	{
		/* increment in xhyhCmnd array */
		/* initialize the data to be sent */
		/* for each coord. We need to split
		   into five bytes to be sent. */
		*(data)	 = (xhyhCmndPtr+i)->cmnd;
		*(data+1)= (xhyhCmndPtr+i)->xmsb;
		*(data+2)= (xhyhCmndPtr+i)->xlsb;
		*(data+3)= (xhyhCmndPtr+i)->ymsb;
		*(data+4)= (xhyhCmndPtr+i)->ylsb;
		
		/* sending one byte at a time,
		   and listening each time */		
		for(j=0; j<XHYHBYTES; j++)
		{
			write_to_qspi_ram(MCF_QSPI_QAR_TRANS, (uint16)*(data+j));	
			write_to_qspi_ram(MCF_QSPI_QAR_CMD, 0x3E00);
			/* transfer 1 element of the queue, with csiv */									
			start_spi_trans(1,1);
			read_from_qspi_ram(0); 
		}
	}	
}
uint16 *sndRecvLcdQspiByte(uint8 *data)
{
	/* This function is used for a pointer
	   input. */
	uint16 *recvData=0;
	
	/* send one byte at a time */
	/* set up the first transf. and comnd. address */
	write_to_qspi_ram(MCF_QSPI_QAR_TRANS, (uint16)*data);	
	write_to_qspi_ram(MCF_QSPI_QAR_CMD, 0x3E00);
	/* transfer 1 element of the queue, with csiv */									
	start_spi_trans(1,1);
	/* receive a byte after sending a byte,
		 this is according to the lcd specification. */
	*recvData = read_from_qspi_ram(0);

	return recvData; /* returns a pointer */
}
uint8 *sndRecvLcdQspiByte2(uint8 data)
{
	/* This function is used for one 
	   byte of data. */
	uint8 *recvData=0;
	
	/* send one byte at a time */
	/* set up the first transf. and comnd. address */
	write_to_qspi_ram(MCF_QSPI_QAR_TRANS, (uint16)data);
	/* set delay before clock and after transfer. */	
	write_to_qspi_ram(MCF_QSPI_QAR_CMD, 0x3e00);
	/* transfer 1 element of the queue, with csiv */									
	start_spi_trans(1,1);
	/* receive a byte after sending a byte,
		 this is according to specification. */
	*recvData = read_from_qspi_ram(0); 
	/* the LCD module needs time to execute the command */
	
	return recvData; /* returns a pointer */
}
uint8 sndRecvLcdQspiByte3(uint8 data)
{
	/* This function is used for one 
	   byte of data. */
	uint8 recvData=0;
	
	/* send one byte at a time */
	/* set up the first transf. and comnd. address */
	write_to_qspi_ram(MCF_QSPI_QAR_TRANS, (uint16)data);
	/* set delay before clock and after transfer. */	
	write_to_qspi_ram(MCF_QSPI_QAR_CMD, 0x3e00);
	/* transfer 1 element of the queue, with csiv */									
	start_spi_trans(1,1);
	/* receive a byte after sending a byte,
		 this is according to specification. */
	//recvData = (uint8)read_from_qspi_ram(0);
	// return read_from_qspi_ram(0);
	// read_from_qspi_ram(0) is actually 16 bits,
	// need to address this later.
	return (uint8)read_from_qspi_ram(0); /* returns an 8-bit value */
}
uint16 sndRecvLcdQspiByte4(uint8 *data)
{
	/* This function is used for a pointer
	   input but return a value. */
	uint16 recvData=0;
	
	/* send one byte at a time */
	/* set up the first transf. and comnd. address */
	write_to_qspi_ram(MCF_QSPI_QAR_TRANS, (uint16)*data);	
	write_to_qspi_ram(MCF_QSPI_QAR_CMD, 0x3E00);
	/* transfer 1 element of the queue, with csiv */									
	start_spi_trans(1,1);
	/* receive a byte after sending a byte,
		 this is according to the lcd specification. */
	return read_from_qspi_ram(0);
	//recvData = read_from_qspi_ram(0);

	//return recvData; /* returns a value */
}
void sndLcdQspiByte(uint8 data)
{
	/* This function is used for one 
	   byte of data. */
	
	/* set up the first transf. and comnd. address */
	write_to_qspi_ram(MCF_QSPI_QAR_TRANS, (uint16)data);
	/* set delay before clock and after transfer. */	
	write_to_qspi_ram(MCF_QSPI_QAR_CMD, 0x3e00);
	/* transfer 1 element of the queue, with csiv */									
	start_spi_trans(1,1);
}
uint16 *sndXhyhCmnd(uint8 cmnd, uint16 xcoord, uint16 ycoord)
{
	/* this function can do set_xhyh, plot_xhyh 
		 or line_to_xhyh. 
		 this function also raises c.s. for
		 every byte. */
	uint8 *arryPtr;
	vuint8 i;
	static uint16 rcvArry[XHYHBYTES];
	
	/* we need an array pointer because we only want to call these
	   functions only once. We first build the cmnd structure from
	   the command and coordinates.  Then we split the structure
	   into five bytes.  Then finally we can send out one byte
	   at a time. */
	arryPtr = bldXhyhCmndByteArry(bldXhyhCmnd(cmnd, xcoord, ycoord),1);
	
	for(i=0; i<XHYHBYTES; i++)
	{
		sndRecvLcdQspiByte(arryPtr+i);	
	}
	
	return rcvArry;
	/* return a pointer to the receive data array.
	   We need to parse this array for possible commands
	   comming back from the lcd, ie, touchscreen commands. */
}
/*******************************************/
void touchProtocol(uint8 protocol)
{
	sndRecvLcdQspiByte2(0xb2);
	sndRecvLcdQspiByte2(protocol);
}
void printChar(uint8 asciiChar)
{
	sndRecvLcdQspiByte2(0x2c);
	sndRecvLcdQspiByte2(asciiChar);
}
void printString(uint8 size, uint8 *strPtr)
{
	uint8 i;
	
	sndRecvLcdQspiByte2(0x2d);
	for(i=0; i<size; i++)
	{
		sndRecvLcdQspiByte2(*(strPtr+i));
	}
}
void butDefLong(uint8 butNo, uint16 state, uint16 butUp, uint16 butDwn, 
								uint16 butDis, uint16 xcoord, uint16 ycoord,
								uint8 width, uint8 height)
{
	sndRecvLcdQspiByte2(0xb5);
	sndRecvLcdQspiByte2(butNo);
	sndRecvLcdQspiByte2(state);
	sndRecvLcdQspiByte2((butUp>>8)&0xff);/* msbyte first */
	sndRecvLcdQspiByte2(butUp&0xff);
	sndRecvLcdQspiByte2((butDwn>>8)&0xff);
	sndRecvLcdQspiByte2(butDwn&0xff);
	sndRecvLcdQspiByte2((butDis>>8)&0xff);
	sndRecvLcdQspiByte2(butDis&0xff);
	sndRecvLcdQspiByte2((xcoord>>8)&0xff);
	sndRecvLcdQspiByte2(xcoord&0xff);
	sndRecvLcdQspiByte2((ycoord>>8)&0xff);
	sndRecvLcdQspiByte2(ycoord&0xff);
	sndRecvLcdQspiByte2(width);
	sndRecvLcdQspiByte2(height);
}
void drawHcoeffBorder(void)
{
	sndXhyhCmnd(SET_XHYH, 24, 75); /* start at 24,75 */
	sndXhyhCmnd(LINE_TO_XHYH, 24, 325); /* draw to 24,325 */
	sndXhyhCmnd(LINE_TO_XHYH, 264, 325); /* draw to 264,325 */
}
void drawHcoeffBox(void)
{            /* command, coord,     coord */
	sndXhyhCmnd(SET_XHYH, LFTVERTBRD, TOPHORZBRD); 
	sndXhyhCmnd(LINE_TO_XHYH, LFTVERTBRD, BOTHORZBRD); /* draw left border */
	sndXhyhCmnd(LINE_TO_XHYH, RGHTVERTBRD, BOTHORZBRD); /* draw bottom border */
	sndXhyhCmnd(LINE_TO_XHYH, RGHTVERTBRD, TOPHORZBRD); /* draw right border */
	sndXhyhCmnd(LINE_TO_XHYH, LFTVERTBRD, TOPHORZBRD); /* back to the start */
}
void drawRawAdcBox(uint8 *setCol)
{
	lcdSendCmnd(4, setCol);
	sndXhyhCmnd(SET_XHYH, ADCLFTBRD, TOPHORZBRD); /* start at 48,75 */
	sndXhyhCmnd(LINE_TO_XHYH, ADCLFTBRD, 474); /* draw left border */
	sndXhyhCmnd(LINE_TO_XHYH, ADCLFTBRD+MAXXCOORDLGTH, 474); /* draw bottom border */
	sndXhyhCmnd(LINE_TO_XHYH, ADCLFTBRD+MAXXCOORDLGTH, TOPHORZBRD); /* draw right border */
	sndXhyhCmnd(LINE_TO_XHYH, ADCLFTBRD, TOPHORZBRD); /* back to the start */
}
void drawHcoeffVertGrid(void)
{
	vuint8 i;
						/* 4*60 = 240           skip 16 */
	for(i=0; i<XPIXELRES*COEFFSIZE; i+=XGRDRES)
	{											/* i+73       , 75 */
		sndXhyhCmnd(SET_XHYH, i+LFTVERTBRD, TOPHORZBRD);
														/* i+73       , 325 */
		sndXhyhCmnd(LINE_TO_XHYH, i+LFTVERTBRD, BOTHORZBRD);
	}
}
void drawHcoeffHorzGrid(void)
{
	vuint16 i;
						 /* 375 - 75 = 300 */		/* skip 50 */
	for(i=0; i<BOTHORZBRD-TOPHORZBRD; i+=YGRDRES)
	{												/* 73       i + 75 */
		sndXhyhCmnd(SET_XHYH, LFTVERTBRD, i+TOPHORZBRD);
															/* 473       i + 75 */
		sndXhyhCmnd(LINE_TO_XHYH, RGHTVERTBRD, i+TOPHORZBRD);
	}
}
uint8 *Cls(void){
  
  /*  clears screen and fill it with the
      current color */
  return sndRecvLcdQspiByte2(0x21);
}
void setColRgb(uint8 red, uint8 green, uint8 blue)
{
	sndRecvLcdQspiByte2(0x31);
	sndRecvLcdQspiByte2(red);
	sndRecvLcdQspiByte2(green);
	sndRecvLcdQspiByte2(blue);
}
void setBgColRgb(uint8 red, uint8 green, uint8 blue)
{
	sndRecvLcdQspiByte2(0x32);
	sndRecvLcdQspiByte2(red);
	sndRecvLcdQspiByte2(green);
	sndRecvLcdQspiByte2(blue);
}

void setXhyh(uint16 xcoord, uint16 ycoord)
{
	sndRecvLcdQspiByte2(0x33);
	sndRecvLcdQspiByte2((xcoord>>8)&0xff);
	sndRecvLcdQspiByte2(xcoord&0xff);
	sndRecvLcdQspiByte2((ycoord>>8)&0xff);
	sndRecvLcdQspiByte2(ycoord&0xff);
}
void vLineh(uint16 ycoord)
{
	sndRecvLcdQspiByte2(0xa1);
	sndRecvLcdQspiByte2((ycoord>>8)&0xff);
	sndRecvLcdQspiByte2(ycoord&0xff);
}
void hLineh(uint16 xcoord)
{
	sndRecvLcdQspiByte2(0xa0);
	sndRecvLcdQspiByte2((xcoord>>8)&0xff);
	sndRecvLcdQspiByte2(xcoord&0xff);
}
void boxhh(uint16 xcoord, uint16 ycoord)
{
	sndRecvLcdQspiByte2(0xa4);
	sndRecvLcdQspiByte2((xcoord>>8)&0xff);
	sndRecvLcdQspiByte2(xcoord&0xff);
	sndRecvLcdQspiByte2((ycoord>>8)&0xff);
	sndRecvLcdQspiByte2(ycoord&0xff);
}
/*	this function seems to turn off the
		display. */
/*void boxHhFill(uint16 xcoord, uint16 ycoord)
{
	sndRecvLcdQspiByte2(0xa5);
	sndRecvLcdQspiByte2((xcoord>>8)&0xff);
	sndRecvLcdQspiByte2(xcoord&0xff);
	sndRecvLcdQspiByte2((ycoord>>8)&0xff);
	sndRecvLcdQspiByte2(ycoord&0xff);
}*/
void boxHhFill(uint16 xcoord, uint16 ycoord)
{
	// this function is not very stable,
	// tends to turn off the lcd.
	// start at 25,76 
	sndXhyhCmnd(SET_XHYH, 25, 76); 
	// fill box with current color 
	sndXhyhCmnd(BOXHH_FILL, xcoord, ycoord); 
}
void butState(uint8 butNo, uint8 state)
{
	sndRecvLcdQspiByte2(0xb1);
	sndRecvLcdQspiByte2(butNo);
	sndRecvLcdQspiByte2(state);
}
void lightBright(uint8 brightness)
{
	sndRecvLcdQspiByte2(0x80);
	sndRecvLcdQspiByte2(brightness);
}
uint8 *lightOff(void)
{
	return sndRecvLcdQspiByte2(0x23);
}
uint8 *lightOn(void)
{
	return sndRecvLcdQspiByte2(0x22);
}
void linToXhyh(uint16 xcoord, uint16 ycoord)
{
	sndRecvLcdQspiByte2(0x3f);
	sndRecvLcdQspiByte2((xcoord>>8)&0xff);
	sndRecvLcdQspiByte2(xcoord&0xff);
	sndRecvLcdQspiByte2((ycoord>>8)&0xff);
	sndRecvLcdQspiByte2(ycoord&0xff);
}
uint8 *plot(void)
{
	return sndRecvLcdQspiByte2(0x26);
}
void PlotXhyh(uint16 xcoord, uint16 ycoord)
{
	sndRecvLcdQspiByte2(0x3e);
	sndRecvLcdQspiByte2((xcoord>>8)&0xff);
	sndRecvLcdQspiByte2(xcoord&0xff);
	sndRecvLcdQspiByte2((ycoord>>8)&0xff);
	sndRecvLcdQspiByte2(ycoord&0xff);
}
uint8 *butAllUp(void)
{
	return sndRecvLcdQspiByte2(0xb3);
}
uint8 *butDelAll(void)
{
	return sndRecvLcdQspiByte2(0xb4);
}
uint8 *fill(void)
{
	return sndRecvLcdQspiByte2(0x9b);
}
void fillBound(uint8 red, uint8 green, uint8 blue)
{
	sndRecvLcdQspiByte2(0x9c);
	sndRecvLcdQspiByte2(red);
	sndRecvLcdQspiByte2(green);
	sndRecvLcdQspiByte2(blue);
}
uint8 *ping(void)
{
	/* returns a pointer */
	return sndRecvLcdQspiByte2(0x83);
}
void drwhCoeffsDots(lcdCoord *hCoeffYCoordPtr, uint16 *x2Ptr, int16 *hCoeffPtr)
{
	uint8 index;
	cmndXhyh *plotXhyhCmndPtr, *plotXhyhCmndPtrHst;
	
	/*set pen color to white */
  lcdSendCmnd(sizeof(setColWht), setColWht);
  /*redraw last graph with background color */	
  for (index=0; index<COEFFSIZE; index+=XHYHDATAQ)
  {
  	lcdSendXhyhCmnd(index, plotXhyhCmndPtrHst, XHYHDATAQ);
  }
    	
  /* set pen color to black */
  lcdSendCmnd(sizeof(setColBlk), setColBlk);	
  /* draw new hCoeffs. graph */
  plotXhyhCmndPtr = 
  		bldXhyhHcoeffCmnd(PLOT_XHYH, hCoeffYCoordPtr, x2Ptr, hCoeffPtr);
  for (index=0; index<COEFFSIZE; index+=XHYHDATAQ)
  {
  	lcdSendXhyhCmnd(index, plotXhyhCmndPtr, XHYHDATAQ);
  }
  /* copy this for the next refresh */
  plotXhyhCmndPtrHst = plotXhyhCmndPtr;
}
void clrhCoeffGrph(cmndXhyh *linToXhyhCmndPtrHst, uint8 *bkgClr, int16 *hCoeffPtr)
{
	uint8 index;
	
	lcdSendCmnd(4, bkgClr);
	/* redraw last graph with background color */
	/* the first element of hCoeffPtr is the h coeff. size */
	/* Looks like we are sending 2 coeffs. at a time.  Make sure
     this make sense for whatever total amount you are sending. */	
	for (index=0; index<(*hCoeffPtr); index+=XHYHDATAQ)
	{
		/* Make sure you are sectioning out the h coeff. array
       at the right amount.  Or else you cannot send all
		   of the coeffs. to the LCD.  Looks like right now it is
       every two.  Can this fit 400 coeffs. or other amount? 
       linToXhyhCmndPtrHst can be up to 400 commands, one
       for each coeff. */
		lcdSendXhyhCmnd(index, linToXhyhCmndPtrHst, XHYHDATAQ);
	}	
}
cmndXhyh *drwhCoeffGrph(int16 *hCoeffPtr, int16 *y1sPtr, uint16 *y2Ptr,
													uint16 *x2Ptr, uint8 *color)
{
	uint16 index;
	cmndXhyh *linToXhyhCmndPtr, *setXhyhhCoeffPtr;
	lcdCoord *hCoeffYCoordPtr;
	
	/* We need to form the x coordinate within this
	   routine.  We can call it. */
	
	/* Forming the lcd pixel y coord from the hcoeffs. */
	/* First we need to build the h coeff. y coords. */
  	hCoeffYCoordPtr = &bldhCoeffYCoord(hCoeffPtr, y1sPtr, y2Ptr);
	
	/* Now we need to form the h coeffs. coords. to their
     x and y commands to be sent to the Lcd.  The command
     we use is SET_XHYH.  This step will incorporate the x and
     y coordinates.  It is a two steps process to come up with
     an h coeff. array to be sent to the Lcd. */
	/* This will set the starting coordinate of the hCoeffs..
     It does not draw the graph yet. We only need one coord. for this. */
	setXhyhhCoeffPtr = bldXhyhHcoeffCmnd(SET_XHYH, hCoeffYCoordPtr, x2Ptr, 
	                                hCoeffPtr);
	/* Set the first coordinate of the h coeffs. array */
	lcdSndIndvXhyhCmnd(0, setXhyhhCoeffPtr);
	
	/* set drawing color */
	lcdSendCmnd(4, color);
	/* Graphs hCoeffs. */
	/* We need to set up the h coeffs. coords. to the LINE_TO_XHYH
     command. */
	linToXhyhCmndPtr = 
		bldXhyhHcoeffCmnd(LINE_TO_XHYH, hCoeffYCoordPtr, x2Ptr, hCoeffPtr);
	/* Now we send this commands set to the Lcd.
	   We are sending XHYHDATAQ coords. at a time. Remember
	   that each coord. has 5 bytes to it.  So we are sending
	   XHYHDATAQ*5 bytes at a time.  We could send up to 400
	   coords..  This is 2000 bytes total. */
	for (index=0; index<(*hCoeffPtr); index+=XHYHDATAQ)
	{
		lcdSendXhyhCmnd(index, linToXhyhCmndPtr, XHYHDATAQ);
	}
	/* linToXhyhCmndPtr has the complete h coeff array. */
	return linToXhyhCmndPtr;
}
void redrwGrdLines(cmndXhyh *linToXhyhCmndPtrHst, int16 *hCoeffPtr)
{
	clrhCoeffGrph(linToXhyhCmndPtrHst, setColWht, hCoeffPtr);
  	/* set drawing grid color */
	lcdSendCmnd(4, setColBlk);
  	delay(500);
  	/*drawHcoeffVertGrid();*/
  	//delay(1000);
  	drawHcoeffHorzGrid();
}
void drwBordAndGrids(uint8 *setCol)
{
	lcdSendCmnd(4, setCol);
  	drawHcoeffBox();
  	delay(5000);
  	/*drawHcoeffVertGrid();*/
  	delay(1000);
  	drawHcoeffHorzGrid();	
}
void drwDsplyTypeBox(void)
{
	setXhyh(700, 75);
  	boxhh(780, 125);
  	delay(500);
  	setXhyh(710, 85);
  	printString(8, DISPLAY);
  	delay(500);
  	setXhyh(720, 105);
  	printString(5, TYPE);
}
void drwGraphColorBox(void)
{
	setXhyh(600, 75);
  	boxhh(680, 125);
  	delay(500);
  	setXhyh(620, 85);
  	printString(6, GRAPH);
  	delay(500);
  	setXhyh(620, 105);
  	printString(6, COLOR);
}
void drwBlueTchScrnBut(void)
{
	setXhyh(600, 150);
  	boxhh(680, 200);
    delay(500);
    setXhyh(620, 175);
    printString(5, BLUE);
    delay(1000);
    butDefLong(2,1,0xffff,0xffff,0xffff,600,150,80,50);
}
void drwRedTchScrnBut(void)
{
	setXhyh(600, 225);
  	boxhh(680, 275);
    delay(500);
    setXhyh(620, 250);
    printString(4, RED);
    delay(4000);
    butDefLong(3,1,0xffff,0xffff,0xffff,600,225,80,50);
}
void drwBlackTchScrnBut(void)
{
	setXhyh(600, 300);
  	boxhh(680, 350);
    delay(500);
    setXhyh(620, 325);
    printString(6, BLACK);
    delay(4000);
    butDefLong(4,1,0xffff,0xffff,0xffff,600,300,80,50);
}
void drwhCoeffsTchScrnBut(void)
{
	setXhyh(700, 150);
  	boxhh(780, 200);
   delay(500); 
   setXhyh(710, 175); 
   printString(8, HCOEFFS); 
   delay(1000); 
   butDefLong(6,1,0xffff,0xffff,0xffff,700,150,80,50); 
}
void drwSignalsTchScrnBut(void)
{
	setXhyh(700, 225);
  	boxhh(780, 275);
   delay(500);
   setXhyh(710, 250);
   printString(8, SIGNALS);
   delay(4000);
   butDefLong(1,1,0xffff,0xffff,0xffff,700,225,80,50);
}
void drwFilteredTchScrnBut(void)
{
	setXhyh(700, 300);
  	boxhh(780, 350);
   delay(500);
   setXhyh(710, 325);
   printString(9, FILTERED);
   delay(4000);
   butDefLong(5,1,0xffff,0xffff,0xffff,700,300,80,50);
}
void drwYHcoeffsLegnd(void)
{
	setXhyh(30, 75);
  	printString(5, o15);
   delay(500);
   setXhyh(30, 75 + YGRDRES);
   printString(5, o1o);
   delay(500);
   setXhyh(30, 75 + 2*YGRDRES);
   printString(5, oo5);
   delay(500);
   setXhyh(30, 75 + 3*YGRDRES);
   printString(5, ooo);
   delay(500);
   setXhyh(30, 75 + 4*YGRDRES);
   printString(6, _oo5);
   delay(500);
   setXhyh(30, 75 + 5*YGRDRES);
   printString(6, _o1o);
   delay(500);
   setXhyh(30, 75 + 6*YGRDRES);
   printString(6, _o15);
   delay(1000);
}
void drwXHcoeffsLegnd(void)
{	/* the -1 is to line up with the grid line. */
	setXhyh(STARTXPIXEL-1, 330);
  	printString(2, zero);
   delay(500);
   setXhyh(STARTXPIXEL-1+1*3*XGRDRES, 330);
   printString(3, twelve);
   delay(500);
   setXhyh(STARTXPIXEL-1+2*3*XGRDRES, 330);
   printString(3, twentyfour);
   delay(500);
   setXhyh(STARTXPIXEL-1+3*3*XGRDRES, 330);
   printString(3, thirtysix);
   delay(500);
   setXhyh(STARTXPIXEL-1+4*3*XGRDRES, 330);
   printString(3, fortyeight);
   delay(500);
   setXhyh(STARTXPIXEL-1+5*3*XGRDRES, 330);
   printString(3, sixty);
   delay(1000);
}
void drwAdcYLegnd(void)
{	
   setXhyh(ADCLFTBRD-25, 75);
   printString(4, three_three);
   delay(500);
   setXhyh(ADCLFTBRD-25, 111);
   printString(4, three_o);
   delay(500);
   setXhyh(ADCLFTBRD-25, 232);
   printString(4, two_o);
   delay(500);
   setXhyh(ADCLFTBRD-25, 354);
   printString(4, one_o);
   delay(500);
	setXhyh(ADCLFTBRD-25, 473);
   printString(2, zero);
}
void initScrn(void)
{
	//touchProtocol(CUBUTTON);
	//delay(60000);
	//delay(60000);
	//delays();
	lightBright(LIGHTBRIGHT);
	//delay(60000);
	//delay(60000);
	delays();
	/* hCoeffs border */
	drwBordAndGrids(setColBlk);
  /*delay(60000);
  delay(60000);
  delays();*/
  //drwDsplyTypeBox();
  //delay(60000);
  //delay(60000);
  //delays();
  //drwGraphColorBox();
  //delay(60000);
  //delay(60000);
  //delays();
  //drwBlueTchScrnBut();
  //delay(60000);
  //delay(60000);
  //delays();
  //drwRedTchScrnBut();
  /*delay(60000);
  delay(60000);
  delays();*/
  //drwBlackTchScrnBut();
  /*delay(60000);
  delay(60000);
  delays();*/
  //drwhCoeffsTchScrnBut();
  /*delay(60000);
  delay(60000);
  delays();*/
  //drwSignalsTchScrnBut();
  /*delay(60000);
  delay(60000);
  delays();*/
  //drwFilteredTchScrnBut();
  /*delay(60000);
  delay(60000);*/
  delays();
  drwYHcoeffsLegnd();
  /*delay(60000);
  delay(60000);
  delays();*/
  /* This should be variable.
     So now we just won't display it. 7/16/2011 */
  /* drwXHcoeffsLegnd();
  delays(); */
  //touchProtocol(CUBUTTON);
}
void eraseHcoefGrph(int16 *hCoeffPtr, int16 *y1sPtr, uint16 *y2Ptr, uint16 *x2Ptr)
{
   /* Erase hCoeffs. border, grids and graph.
      Then draw the raw adc border and grids */
   uint8 *setGrdColorPtr;
   
   setGrdColorPtr = setColWht;
   setGrphColorPtr = setColWht;
   lcdSendCmnd(4, setGrphColorPtr);
   /* erase the hcoeffs. legends. */
   drwYHcoeffsLegnd();
   drwXHcoeffsLegnd();
   /* draw graph and save to graph history pointer */
	linToXhyhCmndPtrHst = 
	drwhCoeffGrph(hCoeffPtr, y1sPtr, y2Ptr, x2Ptr, setGrdColorPtr);
	/* linToXhyhCmndPtrHst points to the complete h coeff array. */
	drwBordAndGrids(setGrdColorPtr);
}
cmndXhyh *monitorScreen(int16 *hCoeffPtr, int16 *y1sPtr, uint16 *y2Ptr, uint16 *x2Ptr, 
				uint16 *adcResBaseYCoordPtr, uint16 *lcdAdcBaseYCoordPtr,
				uint16 *adcXCoordPtr)
{
	/* We no longer use the touchscreen response.
	   The command will come from labview rs232. 
		However, when the lcd is displaying the
		adc result, every coordinate sent to the lcd
		will generate a response from the touch screen. 
		But we will not work on the information. */
		 
   /* 9/5/17 Use the new flow chart. */
   
	vuint8 rcvData = 0;
	vuint8 already_showing_raw = 0, fltrGrph = 0, signals = 0, sig_and_filtr = 0;
	uint8 *setGrdColorPtr;
	cmndXhyh *linToXhyhCmndPtr;
	cmndXhyh *linToXhyhCmndPtrHst;
	uint16 *adcYCoordPtr, *adcYCoordYFirPtr;
	uint8 *adcSetXhyhPtr;
	uint16 i, index, *rcvDatPtr;
	uint8 stopAdc = 0;
	uint16 *adcPtr;
	int32 *yFirPtr = 0;
	
	setGrdColorPtr = setColBlk;
	
	/* If the command is show signals, see if we already showing signal,
	   if not, show signals. */
	
  	if(cmd == 1) /* if raw signal is pushed */
  	{
  	   if (already_showing_raw) /* if already commanded to show raw signal */ 
  	   {  /* update raw signal to lcd */			   
			/* capture adc samples */
			/* raw adc */
			adcPtr = adcArryPtr();
			adcYCoordPtr = 	
			bldAdcResYCoord(adcResBaseYCoordPtr, adcPtr, lcdAdcBaseYCoordPtr);	 	
			/* set the beginning location of the adc signal */	
			adcSetXhyhPtr = bldAdcXhyhCmndByteArry(bldXhyhCmndArry(SET_XHYH, 	
			   										adcXCoordPtr, adcYCoordPtr), 1); 	
			/* set the starting point of the adc graph */	
			for(i=0; i<XHYHBYTES; i++)   
			{   
				sndRecvLcdQspiByte4(adcSetXhyhPtr++);   
			}   
			lcdSendCmnd(4, setGrphColorPtr);   
			/* Now send the adc results to the lcd   
			  	and collect the responses from the lcd. */   
			linToXhyhCmndPtr = bldXhyhCmndArry(LINE_TO_XHYH, adcXCoordPtr, adcYCoordPtr);
  	   }
  	   else if (!already_showing_raw)   /* button just pushed for signal */
  	   {
  	      /* If not showing raw signal yet, prepare to show it.
  	         Need to know which signal was being shown so we
  	         can erase it. */
  	      if(already_showing_hcoeffs)
  	      {
  	         /* Need to turn off the hcoeffs graph
  	            if we were showing it. */	         
  	         eraseHcoefGrph(hCoeffPtr, y1sPtr, y2Ptr, x2Ptr);
  	      }
  	      else if (already_showing_filtered)
  	      {
  	         /* Need to clear the filtered signal if
  	            we were showing it. */
			   lcdSendCmnd(4, setColWht);
			   for (index=0; index<ARRAYSIZE; index+=ADCDATAQ)
			   {
				   lcdSendXhyhCmnd(index, linToXhyhCmndPtr, ADCDATAQ);
			   }
  	      }
  	      else if (already_showing_raw_and_filtered)
  	      {
  	         /* if already showing raw and filtered signals,
  	            need to erase them */
				/* erase raw signal */
			   lcdSendCmnd(4, setColWht);
			   for (index=0; index<ARRAYSIZE; index+=ADCDATAQ)
			   {
				   lcdSendXhyhCmnd(index, linToXhyhCmndPtr, ADCDATAQ);
			   }
				/* erase filtered signal */
  	      }
  	      /* still part of preparing */
	      uart0ArryIndex = 0;  /* reset to the beginning of array */
			*(uart0Arry) = 0;    /* reset */
			cmd = 0;             /* reset */
			fltrGrph = 0;
			raw_signal = 1; 
			sig_and_filtr = 0; 
			already_showing_raw = 1;
	      setGrdColorPtr = setColBlk;
	      drawRawAdcBox(setGrdColorPtr);
	      drwAdcYLegnd();
	   	setGrphColorPtr = setColorHstry;
	   	/* done with prepration */
	   	
			/* now capture adc signal and showing it */   
    	   startAdcSmpl();/* start sampling */	        
	      /* now take care of adc graph */
	      setGrdColorPtr = setColBlk;
	      drawRawAdcBox(setGrdColorPtr);
	      drwAdcYLegnd();
	   	
	      setGrphColorPtr = setColorHstry;
			   
			/* capture adc samples */
			/* raw adc */
			adcPtr = adcArryPtr();
			adcYCoordPtr = 	
			bldAdcResYCoord(adcResBaseYCoordPtr, adcPtr, lcdAdcBaseYCoordPtr);	 	
			/* set the beginning location of the adc signal */	
			adcSetXhyhPtr = bldAdcXhyhCmndByteArry(bldXhyhCmndArry(SET_XHYH, 	
			   										adcXCoordPtr, adcYCoordPtr), 1); 	
			/* set the starting point of the adc graph */	
			for(i=0; i<XHYHBYTES; i++)   
			{   
				sndRecvLcdQspiByte4(adcSetXhyhPtr++);   
			}   
			lcdSendCmnd(4, setGrphColorPtr);   
			/* now send the adc results to the lcd   
			  	and collect the responses from the lcd. */   
			linToXhyhCmndPtr = bldXhyhCmndArry(LINE_TO_XHYH, adcXCoordPtr, adcYCoordPtr);
  	   }  /* button just pushed for signal */
  	}     /* if raw signal is pushed */
  	else if (cmd != 1)
  	{
  	   /* If raw signal is not pushed, check to see
  	      if we are in that mode already. */
  	   if (already_showing_raw)
  	   {
  	      /* If in raw signal mode already,
  	         update the raw adc and show it */
			/* capture adc samples */
			/* raw adc */
			adcPtr = adcArryPtr();
			adcYCoordPtr = 	
			bldAdcResYCoord(adcResBaseYCoordPtr, adcPtr, lcdAdcBaseYCoordPtr);	 	
			/* set the beginning location of the adc signal */	
			adcSetXhyhPtr = bldAdcXhyhCmndByteArry(bldXhyhCmndArry(SET_XHYH, 	
			   										adcXCoordPtr, adcYCoordPtr), 1); 	
			/* set the starting point of the adc graph */	
			for(i=0; i<XHYHBYTES; i++)   
			{   
				sndRecvLcdQspiByte4(adcSetXhyhPtr++);   
			}   
			lcdSendCmnd(4, setGrphColorPtr);   
			/* now send the adc results to the lcd   
			  	and collect the responses from the lcd. */   
			linToXhyhCmndPtr = bldXhyhCmndArry(LINE_TO_XHYH, adcXCoordPtr, adcYCoordPtr);
  	   }
  	}  /* if raw signal is not pushed, check to see if we are in that mode already */
  	if (cmd == 5)  /* ask if cmd is filtered */
  	{
  	   if (cmd_already_filtered) /* if already showing filtered signal */
  	   {  /* if already showing filtered signal, update the signal */
		   /* capture adc samples */
		   /* raw adc */
		   adcPtr = adcArryPtr();
		   /* filtered signal */
		  	yFirPtr = yFir(*hCoeffPtr, ARRAYSIZE, adcPtr, hCoeffPtr);   
		  	/* set the beginning location of the adc signal */   
		  	adcYCoordYFirPtr = bldAdcResYCoordForYFir(adcResBaseYCoordPtr,    
		  												yFirPtr, lcdAdcBaseYCoordPtr);    
			adcSetXhyhPtr = bldAdcXhyhCmndByteArry(bldXhyhCmndArry(SET_XHYH,   
			   									adcXCoordPtr, adcYCoordYFirPtr), 1);   
			/* set the starting point of the adc graph */   
		  	for(i=0; i<XHYHBYTES; i++)   
		  	{   
		  	   sndRecvLcdQspiByte4(adcSetXhyhPtr++);   
		  	}   
			lcdSendCmnd(4, setGrphColorPtr);   
			/* now send the filtered results to the lcd   
		      and collect the responses from the lcd. */   
			linToXhyhCmndPtr = bldXhyhCmndArry(LINE_TO_XHYH, adcXCoordPtr,    
		  															 adcYCoordYFirPtr);
  	   }
  	   else
  	   {  /* if filter is pushed and not already in that mode */
  	      /* need to prepare the graph before displaying the 
  	         filtered signal. */
  	        
  	   }   
  	         
   if (cmd == 6) /* if command is signal and filter */
  	            {
  	               if (cmd_already_sig_filtered) /* if already showing this */
  	               {
  	                  if (cmd == 0) /* if command is to show hcoeffs */
  	                  {
  	                     if (cmd_already_hcoeffs) /* if already showing hcoeffs */
  	                     {
  	                        /* if display is already showing h coeffs,
  	                           ask about other commands sent */
  	                        if (cmd == 2) /* if command is to show blue color */
  	                        {
  	                           if (cmd_already_blue) /* if color is already blue */
  	                           {
  	                              /* if display is already blue, don't do
  	                                 anything */  	                              
  	                           }  /* if color is already blue */
  	                           else  /* if we need to show blue display */  
  	                           {
  	                              /* if we need to show blue display */
			                        uart0ArryIndex = 0;  /* reset to the beginning of array */
			                        *(uart0Arry) = 0;    /* reset */
			                        cmd = 0;             /* reset */
			                        setGrphColorPtr = setColBlue;
			                        setColorHstry = setColBlue;
			                        lcdSendCmnd(4, setGrphColorPtr);
			                        cmd_already_blue = 1; /* display is now blue */
  	                           }  /* if we need to show blue display */
  	                        }  /* if command is to show blue color */
  	                        else
  	                        {
  	                           /* if command is not to show blue color,
  	                              do nothing */
  	                        }  /* if command is not to show blue color */
  	                     }  /* if already showing hcoeffs */
  	                     else
  	                     {
  	                        /* if display is not showing h coeffs already,
  	                           need to show them */
					            /* erase adc border, grids and graph
						            with white color. */
					            uart0ArryIndex = 0; /* reset to the beginning of array */
					            *(uart0Arry) = 0; /* reset */
					            cmd = 0; /* reset */
					            fltrGrph = 0;
					            signals = 0;
					            signal_and_filtr = 0;
					            setGrdColorPtr = setColWht;
					            drawRawAdcBox(setGrdColorPtr);
					            drwAdcYLegnd();
					            for (index=0; index<ARRAYSIZE; index+=ADCDATAQ)
					            {
						            lcdSendXhyhCmnd(index, linToXhyhCmndPtr, ADCDATAQ);
					            }
					            /* draw hCoeffs. border and grid */
					            setGrdColorPtr = setColBlk;
					            drwBordAndGrids(setGrdColorPtr);
					            drwYHcoeffsLegnd();
					            /* drwXHcoeffsLegnd(); we'll do this later */
					            /* draw graph and save to graph history pointer */
					            setGrphColorPtr = setColorHstry;
					            linToXhyhCmndPtrHst = 
						            drwhCoeffGrph(hCoeffPtr, y1sPtr, y2Ptr, x2Ptr, setGrphColorPtr);
					            stopAdc = 1;
			                  cmd_already_hcoeffs = 1; /* display is now showing h coeffs */
  	                     }  /* need to show h coeffs from command sent */
  	                  }
  	               }
  	            }
  	         }
  	      }
  	   }      
	
	
	/* We are starting with the hcoeffs. screen. */
	switch(cmd)
	{	
		case 1:                    /* signals button is pushed */
		   if (cmd_already_signal) /* if already showing signal */ 
		   {
		      /* don't do anything */
		   }
		   else
		   {
			   uart0ArryIndex = 0;  /* reset to the beginning of array */
			   *(uart0Arry) = 0;    /* reset */
			   cmd = 0;             /* reset */
			   fltrGrph = 0;
			   signals = 1; 
			   sig_and_filtr = 0; 
			   cmd_already_signal = 1;
			   
    	      startAdcSmpl();/* start sampling */
            /* erase hCoeffs. border, grids and graph.
      	      Then draw the raw adc border and grids */
            setGrdColorPtr = setColWht;
            setGrphColorPtr = setColWht;
            lcdSendCmnd(4, setGrphColorPtr);
            /* erase the hcoeffs. legends. */
            drwYHcoeffsLegnd();
            drwXHcoeffsLegnd();
            /* draw graph and save to graph history pointer */
	         linToXhyhCmndPtrHst = 
	    	      drwhCoeffGrph(hCoeffPtr, y1sPtr, y2Ptr, x2Ptr, setGrdColorPtr);
	         /* linToXhyhCmndPtrHst points to the complete h coeff array. */
	         drwBordAndGrids(setGrdColorPtr);
	         /*************************************************************/
	         
	         /* now take care of adc graph */
	         setGrdColorPtr = setColBlk;
	         drawRawAdcBox(setGrdColorPtr);
	         drwAdcYLegnd();
	   	
	         setGrphColorPtr = setColorHstry;
			   
			   /* capture adc samples */
			   /* raw adc */
			   adcPtr = adcArryPtr();
			   adcYCoordPtr = 	
					bldAdcResYCoord(adcResBaseYCoordPtr, adcPtr, lcdAdcBaseYCoordPtr);	 	
				/* set the beginning location of the adc signal */	
				adcSetXhyhPtr = bldAdcXhyhCmndByteArry(bldXhyhCmndArry(SET_XHYH, 	
			   										adcXCoordPtr, adcYCoordPtr), 1); 	
			  	/* set the starting point of the adc graph */	
			  	for(i=0; i<XHYHBYTES; i++)   
			  	{   
			  		sndRecvLcdQspiByte4(adcSetXhyhPtr++);   
			  	}   
					lcdSendCmnd(4, setGrphColorPtr);   
			  	/* now send the adc results to the lcd   
			   	and collect the responses from the lcd. */   
			  	linToXhyhCmndPtr = bldXhyhCmndArry(LINE_TO_XHYH, adcXCoordPtr, adcYCoordPtr);    
		   }
			break;
		case 2:                    /* if blue color is pushed */
		   if (cmd_already_blue)   /* if already showing blue */
			{
			   /* don't do anything */
			}
			else
			{  /* if not showing blue already */
			   uart0ArryIndex = 0;  /* reset to the beginning of array */
			   *(uart0Arry) = 0;    /* reset */
			   cmd = 0;             /* reset */
			   setGrphColorPtr = setColBlue;
			   setColorHstry = setColBlue;
			   lcdSendCmnd(4, setGrphColorPtr);
			   cmd_already_blue = 1;   
			}
			break;
		case 3:                    /* if red color is pushed */
		   if (cmd_already_red)    /* if already showing red */
			{
			   /* don't do anything */
			}
			else
			{
			   uart0ArryIndex = 0;  /* reset to the beginning of array */
			   *(uart0Arry) = 0;    /* reset */
			   cmd = 0;             /* reset */
			   setGrphColorPtr = setColRed;
			   setColorHstry = setColRed;
			   lcdSendCmnd(4, setGrphColorPtr);
			   cmd_already_red = 1;			   
			}
			break;
		case 4:                    /* if black color is pushed */
		   if (cmd_already_black)  /* if already showing black */
			{
			   /* don't do anything */
			}
			else
			{
			   uart0ArryIndex = 0;  /* reset to the beginning of array */
			   *(uart0Arry) = 0;    /* reset */
			   cmd = 0;             /* reset */
			   setGrphColorPtr = setColBlk;
			   setColorHstry = setColBlk;
			   lcdSendCmnd(4, setGrphColorPtr);
			   cmd_already_black = 1;
			}
			break;
		case 5:                    /* Filtered button is pushed */
		   if (cmd_already_fltr)   /* if already showing filtered */
			{
			   /* don't do anything */
			}
			else
			{
			   uart0ArryIndex = 0;  /* reset to the beginning of array */
			   *(uart0Arry) = 0;    /* reset */
			   cmd = 0;             /* reset */
			   fltrGrph = 1;
			   signals = 0;
			   sig_and_filtr = 0;
			   cmd_already_fltr = 1;
			}
			break;
		case 6:                             /* Signal and Filtered button is pushed */
		   if (cmd_already_sig_and_fltr)    /* if already showing filtered */
			{
			   /* don't do anything */
			}
			else
			{
			   uart0ArryIndex = 0;  /* reset to the beginning of array */
			   *(uart0Arry) = 0;    /* reset */
			   cmd = 0;             /* reset */
			   fltrGrph = 0;
			   signals = 0;
			   sig_and_filtr = 1;  
			   cmd_already_sig_and_fltr = 1;
			}
			break;
		default:
			break;		
	}
	
  	if(signals || fltrGrph || sig_and_filtr) /* if button is Signal, Filtered or signal and filtered */
  	{  	
    	startAdcSmpl();/* start sampling */
      /* erase hCoeffs. border, grids and graph.
      	 Then draw the raw adc border and grids */
      setGrdColorPtr = setColWht;
      setGrphColorPtr = setColWht;
      lcdSendCmnd(4, setGrphColorPtr);
      /* erase the hcoeffs. legends. */
      drwYHcoeffsLegnd();
      drwXHcoeffsLegnd();
      /* draw graph and save to graph history pointer */
	   linToXhyhCmndPtrHst = 
	    	drwhCoeffGrph(hCoeffPtr, y1sPtr, y2Ptr, x2Ptr, setGrdColorPtr);
	   /* linToXhyhCmndPtrHst points to the complete h coeff array. */
	   drwBordAndGrids(setGrdColorPtr);
	   /*************************************************************/
	   /* now take care of adc graph */
	   setGrdColorPtr = setColBlk;
	   drawRawAdcBox(setGrdColorPtr);
	   drwAdcYLegnd();
	   	
	   setGrphColorPtr = setColorHstry;
	  
	  	do
	  	{
			/* capture adc samples */
			/* raw adc */
			adcPtr = adcArryPtr();
			
	  		if(fltrGrph)/* if filtered button is pushed */
			{
				/* filtered signal */
			  	yFirPtr = yFir(*hCoeffPtr, ARRAYSIZE, adcPtr, hCoeffPtr);   
			  	/* set the beginning location of the adc signal */   
			  	adcYCoordYFirPtr = bldAdcResYCoordForYFir(adcResBaseYCoordPtr,    
			  													yFirPtr, lcdAdcBaseYCoordPtr);    
				adcSetXhyhPtr = bldAdcXhyhCmndByteArry(bldXhyhCmndArry(SET_XHYH,   
				 																	adcXCoordPtr, adcYCoordYFirPtr), 1);   
				 	/* set the starting point of the adc graph */   
			  	for(i=0; i<XHYHBYTES; i++)   
			  	{   
			  		sndRecvLcdQspiByte4(adcSetXhyhPtr++);   
			  	}   
					lcdSendCmnd(4, setGrphColorPtr);   
					/* now send the filtered results to the lcd   
			   	 and collect the responses from the lcd. */   
					linToXhyhCmndPtr = bldXhyhCmndArry(LINE_TO_XHYH, adcXCoordPtr,    
			  																	 adcYCoordYFirPtr);   
			}
			else
			{
				/* if Signals button is pushed */  		
			  	adcYCoordPtr = 	
					bldAdcResYCoord(adcResBaseYCoordPtr, adcPtr, lcdAdcBaseYCoordPtr);	 	
				/* set the beginning location of the adc signal */	
				adcSetXhyhPtr = bldAdcXhyhCmndByteArry(bldXhyhCmndArry(SET_XHYH, 	
			   										adcXCoordPtr, adcYCoordPtr), 1); 	
			  	/* set the starting point of the adc graph */	
			  	for(i=0; i<XHYHBYTES; i++)   
			  	{   
			  		sndRecvLcdQspiByte4(adcSetXhyhPtr++);   
			  	}   
					lcdSendCmnd(4, setGrphColorPtr);   
			  	/* now send the adc results to the lcd   
			   	 and collect the responses from the lcd. */   
			  	linToXhyhCmndPtr = bldXhyhCmndArry(LINE_TO_XHYH, adcXCoordPtr,    
			  																	 adcYCoordPtr);   	   
			} 
			
			/* starts sending signal to lcd. */
			for (index=0; index<ARRAYSIZE; index+=ADCDATAQ)
			{
				rcvDatPtr = lcdSendXhyhCmnd2(index, linToXhyhCmndPtr, ADCDATAQ);
			}
			
			/* After displaying the signal, we need to clear it so we can draw 
				a new one */
			lcdSendCmnd(4, setColWht);
			for (index=0; index<ARRAYSIZE; index+=ADCDATAQ)
			{
				lcdSendXhyhCmnd(index, linToXhyhCmndPtr, ADCDATAQ);
			}
			
			/* first element, if not used as hcoeffs array.
			   Normally, the first element of the array
			   is the size of the array. */
			switch(cmd) 
			{
				case 1: /* signals button is pushed */
					uart0ArryIndex = 0; /* reset to the beginning of array */
					*(uart0Arry) = 0; /* reset */
					cmd = 0; /* reset */
					signals = 1;
					fltrGrph = 0; 
					break;
					/* other color buttons are pushed. */
				case 2:
					uart0ArryIndex = 0; /* reset to the beginning of array */
					*(uart0Arry) = 0; /* reset */
					cmd = 0; /* reset */
					setGrphColorPtr = setColBlue;
					setColorHstry = setColBlue;
					lcdSendCmnd(4, setGrphColorPtr);
					break;
				case 3:
					uart0ArryIndex = 0; /* reset to the beginning of array */
					*(uart0Arry) = 0; /* reset */
					cmd = 0; /* reset */
					setGrphColorPtr = setColRed;
					setColorHstry = setColRed;
					lcdSendCmnd(4, setGrphColorPtr);
					break;
				case 4:
					uart0ArryIndex = 0; /* reset to the beginning of array */
					*(uart0Arry) = 0; /* reset */
					cmd = 0; /* reset */
					setGrphColorPtr = setColBlk;
					setColorHstry = setColBlk;
					lcdSendCmnd(4, setGrphColorPtr);
					break;
				case 5: /* Filtered button is pushed */
					uart0ArryIndex = 0; /* reset to the beginning of array */
					*(uart0Arry) = 0; /* reset */
					cmd = 0; /* reset */
					fltrGrph = 1;
					signals = 0;
					break;
					/* The hcoeff button is the button that
			 		would end this entire routine and
			 		brings us back to the main routine. 
			 		Do we need to clear signals and fltrGrph? */
				case 6: /* hcoeffs button is pushed. */
					/* erase adc border, grids and graph
						with white color. */
					uart0ArryIndex = 0; /* reset to the beginning of array */
					*(uart0Arry) = 0; /* reset */
					cmd = 0; /* reset */
					fltrGrph = 0;
					signals = 0;
					setGrdColorPtr = setColWht;
					drawRawAdcBox(setGrdColorPtr);
					drwAdcYLegnd();
					for (index=0; index<ARRAYSIZE; index+=ADCDATAQ)
					{
						lcdSendXhyhCmnd(index, linToXhyhCmndPtr, ADCDATAQ);
					}
					/* draw hCoeffs. border and grid */
					setGrdColorPtr = setColBlk;
					drwBordAndGrids(setGrdColorPtr);
					drwYHcoeffsLegnd();
					/* drwXHcoeffsLegnd(); we'll do this later */
					/* draw graph and save to graph history pointer */
					setGrphColorPtr = setColorHstry;
					linToXhyhCmndPtrHst = 
						drwhCoeffGrph(hCoeffPtr, y1sPtr, y2Ptr, x2Ptr, setGrphColorPtr);
					stopAdc = 1;
					break;
				default:
					break;
			}/*switch*/
			
		}while(!stopAdc);
  } /* if filtered or signals button is pushed */
		
  return linToXhyhCmndPtrHst;	
}
/****************************************************/