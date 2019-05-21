/* Ver27<x-w<-v<-u<-<-t<-s<-r<-q<-p<-<-o<-n<-m<-L<-k<-j<-<-i<-h<-g<-f<-e<-d
 *			<-c<-b<-a<-26z<-y<-x<-w<-v<-u<-tc<-r
 * File:	main.c
 * Purpose:	sample program
 * 8/27/09	Able to communicate with terminal with uart isr.
 *			Now try to communicate with LabView.
 *			This version works with labview, but we still get overrun error.
 *			Try to fix the isr routine so it won't have OE error, or
 *			clear the error by writing to UCR0.
 * 8/28/09  Don't bother clearing the OE.  Just create a second array
 *          and sort it accordingly to the index "uart0ArryIndex".
 *			Don't need to create a second array.  Just work with the first 
 * 			array and the location of uart0ArryIndex.
 * 8/31/09	Try to get adc samples.
 * 9/2/09	Adapt adc sampling from ver6.
 * 9/3/09 	Need to check on adc results.
 * 9/4/09	Try to implement pointer and pack hCoeffs. into 4 bytes number.
 * 9/5/09	The repacking of the hCoeffs. are correct only if 
 *			index2 = 4*COEFFSIZE.
 *			Else, the repacking routine will never get called.  When the uart0 
 *			transmission gets cutoff somewhere, index2 won't be 4*COEFFSIZE, 
 *			therefore,
 *			the repacking routine won't get called and index2 won't get 
 *			reset also. If index2 doesn't get reset, it will increment 
 *			beyond 4*COEFFSIZE and will never get reset again.
 * 9/9/09	Got uart0 isr to work and pack correctly.  But may require a 
 *			few transfer before the packing to work correctly.  Still need 
 *			to fix when the first time all bytes are transfer correctly to 
 *			pack correctly.  Right now, the packing only works if 
 *			uart0ArryIndex-1 is not -1.
 * 9/9/09	Packing and uart0 transfer also work with adc samples collection.
 * 9/10/09	Try to use pointers wherever possible.
 * 			Fixed it so if 0xAA is at the very end of the array, the packing 
 *			routine will be called and works.  Need to make hCoeff[] accessible 
 *			to other routines.
 *			*packhCoeff() returns a pointer to the hCoeff[] array.
 * 9/15/09	pack routine can be used for 16 bits numbers.
 * 9/17/09  Need a pointer to point to the returned pointer of packhCoeff.
 *			Same thing for adcArryPtr().
 * 9/21/09	Check for stability with new structure declaration.  The RS232 
 *			transfer packing and coords. are sucessful if we change the windows 
 *			each time. Don't try to send the same window hCoeffs. many time at 
 *			one time.
 * 9/23/09	Fixed the run away do-while loop in bldhCoeffCoords().
 * 9/25/09	Has function that returns a structure to the plotXHY coords.
 * 9/26/09	Has function that returns a pointer to the hCoeff Y coord. only, 
 *			not working.
 * 			Also, has a function that returns a pointer to the plotXHY cmnd.
 *			Build pointers for y1s and y2.
 * 9/28/09	Create #define constants for most of the subroutines and functions.
 *			These are working.
 * 10/3/09	Changing constants to fit EZLCD 105 model.
 *  		Build correct x and y coords. for ezlcd 105 model.
 * 10/6/09	Able to index pointer to structure array; qspi subroutine
 *			lcdSendXhyhCmnd().
 * 10/7/09	Able to load transf. and comnd. ram addresses successfully,
 *			lcdSendXhyhCmnd().
 * 10/8/09	Parse out the individual byte of the xhyh comand structure.
 *			These three hundred bytes will be sent through the qspi
 *			port one byte at a time.
 * 10/24/09 Got the ezlcd105 to draw some lines. But still need lots
 *			of work left.
 * 10/25/09 Edit the lcd functions to take care of the returned miso data.
 * 10/25/09 Add more delay before starting qspi clock.  Add data direction
 * 			flag to memory address.  Move primary qspi pin selection to front
 *			of qspi init routine.
 * 10/28/09 Was able to plot h coefficients on the ezlcd105 display, either by
 *  		plotting dots or line.  But the lcd may not update correctly.
 * 10/29/09 It seems that you need to leave the qspi ss line down for the
 *			entire command, not just every byte.
 *			Able to draw the hCoeffs. dots on the lcd after every clear
 *			screen.  But not very well with line drawings.
 * 10/31/09	Attemp to create seperate files.
 * 11/1/09	Sucessfully separated the files.
 *			Was able to fix the problem that changes the setColWht array.
 *			See "cmndXhyh *bldXhyhCmnd(uint8 cmnd, uint16 xcoord, 
 *			uint16 ycoord)" function in "xhyhCmndFuncts.c" file. 
 * 11/2/09	Attemp to use the "*bldhCoeffYCoord(hCoeffPtr, y1sPtr, y2Ptr)" 
 *			function. This allows us to return a pointer to the structure 
 *			instead of the structure, which takes more memory. 
 * 11/6/09	Created the history array for the hCoeffs. so we can refresh the 
 *			graph everytime we import new hCoeffs..  We just use the white
 *			color to draw the previous hCoeffs..
 *			The lcd still crashes often, increased the wait time after
 *			transfer but still doesn't help much.
 * 11/21/09 Able to plot dots of hCoeffs. consitently.
 *			But not able to draw lines of hCoeffs. consistently yet.
 * 11/23/09 Able to draw hCoeffs. graphs consistently.  But the lcd seems to
 *			crash when we try to draw the rectangle in the beginning.  This 
 *			is opposite of what happened before.
 *			We now draw the boundary box before the forever loop.
 *			Got hCoeffs. graph to update correctly.  If a transmission
 *			from LabVIEW is corrupted, the graph will clear but will not
 *			display the corrupted hCoeffs..  Able to group all the routines
 *			into one function for updating hCoeffs. graph.
 * 11/24/09	Able to draw vertical grid lines before the forever loop.
 *			Also, able to draw horizontal grid and update all grids 
 *			properly after hCoeffs. updates.
 * 11/25/09 Able to draw the buttons and labels.  But still need to
 *			define these buttons for touchscreen.
 * 12/06/09 Got the blue graph color touch screen to work.  Need to find
 *			the right delay for polling of the touch screen.
 * 12/19/09 Need to slow down the qspi when initializing the screen.
 * 			Also, wrote made function lcdTouchScreen() for touch screen
 *			instead of leaving the routine in main.
 * 12/25/09	Got the Signals button to work.  Need to separate the border
 *			and grid color from the graph color.
 *			Try to implement raw adc signals.  Need to lower the neg. 
 *			limit of the hCoeff. y coord. for the lcd.  Higher lower PB freq.
 *			will not see the hcoeffs. at the present setting.
 * 12/26/09	Got the adc signals to combine into lcd coordinates.  But do not
 *			know if the adc results are correct.
 *			Now need to transfer the coordinates to the lcd, but also need
 *			to receive the touch screen response from the lcd.
 * 	1/1/10	Able to change to adc graph from hcoeffs. graph.  But still
 *			cannot change color of graph or go back to hcoeffs. graph.
 *			Also, change the receiving array to uint8 instead of uint16.
 *	1/6/10	Fixed the problem of broken adc coords. packet.  Now every
 *			coord. is sent successfully.  Best to send 10 bytes, not 15.
 *	1/24/10 Able to get the adc to work for ac signals.  Square wave is
 *			the best.  Not so good with sine yet.  Samples at 200 still work.
 *			Need to increase the samples to better show sine wave.
 * 2/13/10 Able to get more than 256 adc samples now.  Fixed the bug in
 *			uint8 to uint16.
 *	2/14/10 Try to implement touch screen with adc color change using 
 *			qspi receive ram que.  Seems to be working but need to improve.
 *  4/11/10 Try to extend the h-coeffs. y coord. label to 0.4.  Before it 
 *			was only 0.2.
 *	5/01/10 Sucessfully re-scaled hcoeffs. to show 0.4 to -0.1.
 *	5/09/10 Added legends for hcoeffs. and adc graphs.  Also, added set 
 *			color history global so the correct color would appear each time.
 * 	5/14/10	It seems that the lcd crashes a lot when starting.  It doesn't seem
 *			to have a problem after we cycle the power.  Tried to slow down the
 *			initScrn() so the crash won't happen.
 *			Also, the square wave seems to match the scope output.  So the
 *			sampling is correct, but only with square wave, not sine wave.
 *			This version seems to be stable when changing color and graphs.
 *	5/16/10 Ready to start dsp work.
 *	8/11/10	Changed the adc result starting point at 32768 (2^15) 
 *			instead of 32760.
 *	2/19/11 Investigating the adc raw data conversion to lcd y coord.  
 *			The spreadsheet use the exact equation given in the data sheet, 
 *			page 30-26.
 *  2/26/11 The adc result starting point has been changed back to 32760 some 
 *			time ago.
 *	3/19/11 Looks like we're one short on the length for lcd y coords.
 *	4/9/11  Got y Fir to work.  The output is a bit scaled down from the 
 *			original x signal (adcRes).  Noticed the hCoeffs output on the
 *          lcd display is not the same as in Labview.  Need to check 
 *          on that.  Is it because
 *          we are only scaling the original hcoeffs to 2^15, not 2^16 anymore?
 *  4/10/11 Try to typecast temp so the function will return unint16 for y Fir.
 *	4/16/11 We created an identical function so we did not have to typecast the
 *          y fir output.  This worked and we were able to print the filtered 
 *          signal on the lcd screen.
 *			Try to print the filtered and or the raw adc on the screen with 
 *			some kind of button. Able to print the filtered signal with a button.
 *	4/17/11 Try to navigate between any button pushed.
 *  6/18/11 The filter seems to work for low pass only.  Now trying to make it
 *          work for bandpass.
 *	7/10/11 Try to increase the coefficients size.  This looks like it would
 *			improve the bandpass result.
 *	7/17/11 Was able to increase the hcoeff. size to 100.  Think we could
 *          increase to more.  But before that we need to change the uart
 *          isr to handle more data.  As of right now, we are importing
 *          the entire hcoeff array, we really only need half of it to
 *          be transfered.  This is because of even symmetry.  Even with odd
 *          symmetry, we could still use only half of the array.
 *          We need to transfer half of the hcoeffs. and another two more bytes
 *          to tell how many bytes were transfered, plus the finished flag byte.
 *          We need to do this because each hcoeff is actually two bytes,
 *          so the amount of information coming across is a lot.  For example,
 *          with 100 hcoeffs. we are actually transfering 200 bytes plus three
 *          more bytes.  So if we are going to increase the amount of
 *          hcoeffs., we need to reduce the traffic.  Not sure if this is
 *          necessary, but reducing half of the load seems like a good thing
 *          to do.
 *  2/5/12	With ver 27r, we can change the color on the "Signals" screen.
 *			However, the graphs are drawn across the screen with horizontal
 *			lines.  We need to work on this with ver 27s.
 *	2/12/12	We now have a good version (ver 27s).  The bandpass filter is 
 *			working as designed.  It mimics the labview filter design.
 *			We can capture the adc signal and filter it.  We can change the
 *			length of the h coefficients.  We can change the color of the
 *			signals at will.  We can switch back and forth between the
 *			raw signal and the filtered signal.  We can also go back
 *			to the h coeff. screen.
 *	3/10/12 Ver 27u can show the h coeffs. expanded accross the whole
 *			h coeff. screen.  But we are experiencing a lot of asm exceptions.
 *	3/12/12 Ver 27u, was able to show up to 140 h coeffs.  All the functions on
 *			the screen are working.  Was able to expand the h coeffs. accross
 *			the screen without asm exceptions.  This is because we use a
 *			default h coeff. resolution of 1 if the size of the coeffs.
 *			happens to be zero.  This could happen if we could not transfer
 *			the coeffs. properly.  Also, reduce the delay time for the
 *          initScrn() function.  See function delays() also. 
 *	3/16/12 Ver 27u, very stable, worked for a long time.  Had to turn it off.
 *			The h coeffs. are spread out on the screen as much as possible.
 *			Was able to transferred 160 h coeffs..
 *	4/18/13	Ver 27 v, touchscreen does not work anymore.  Try to use RS232 
 *			from Labview to control the screen functions.  Make sure 
 *       uartArryIndx is initialized to 0 everytime we send a command 
 *			or hcoeffs.  For a command, also use xAA at the end so we'll know 
 *       that it is a command.
 * 9/5/17    Use a different main flow chart.
 */
#include "support_common.h" /* include peripheral declarations and more */
   
#if (CONSOLE_IO_SUPPORT || ENABLE_UART_SUPPORT) 
/* Standard IO is only possible if Console or UART support is enabled. */
#include <stdio.h>
#endif

#include "define.h"
#include "globals.h"
#include "structures.h"
#include "ezLCDConsts.h"

#include "uartFunctions.c"
#include "hCoeffsFuncts.c"
#include "xhyhCmndFuncts.c"
#include "qspiFuncts.c"
#include "ezLCDFuncts.c"
/***********************/

int main(void)
{
	int counter = 0;
	vuint8 index=0;
	uint16 *x2Ptr; /* This is for the h coeffs. */
	int16 *hCoeffPtr = 0; 
	int16 *y1sPtr; /* This is for the h coeffs. */
	uint16 *y2Ptr; /* This is for the h coeffs. */
  	cmndXhyh *linToXhyhCmndPtrHst;   	
	uint8 *qspiDataPtr=0;
	uint8 drwBoxFlg=0;
	vuint8 rcvData = 0;
	vuint8 touch = 0;
	uint16 *adcResBaseYCoordPtr, *lcdAdcBaseYCoordPtr;
	uint16 *adcXCoordPtr, *adcXCoordPtrRepl;

	setColorHstry = setColRed; /* points to array */
	
	/* These functions are for the adc graphs. */
	adcResBaseYCoordPtr = bldAdcResBaseYCoord();
	lcdAdcBaseYCoordPtr = bldLcdAdcBaseYCoord();
	/*adcXCoordPtr = bldXCoord(ARRAYSIZE);*/
	adcXCoordPtrRepl = bldXCoordRepl(ARRAYSIZE);
	
	/* These functions are for the h coeffs. graph. */
	hCoeffPtr = packhCoeff();
	x2Ptr = bldx2();
	/*x2Ptr = formHCoeffXCoords(hCoeffPtr);*/
	y1sPtr = bldY1s();
	y2Ptr = bldY2();
 
 	/* legacy issue, do not remove the next two lines */
	printf("Hello World in C from MCF52259 derivative on M52259DEMO board\n\r");
	fflush(stdout);
	/***************************************************/
	
	uart0IntSetUp();
	init_spi(QSPIBITCNT, QSPIINTFLG); 	
	
	/* Global interrupt enable */
	asm (move.w #0x2000,SR); // unmask interrupts in Core status register
	
	initScrn();	
    	
	for(;;) {	   
	  
    /* when we finished transferring the hCoeffs. */
  	if(pack)
  	{ 
  		/* reset the flag */
  		pack = 0;
  		 		
  		/* reforming the 16 bits hCoeff. values */
  		/* returns pointer to hCoeffs */
  		/* may want to renew hCoeffPtr again in here */  	
    	packhCoeff();
    	delay(5000);
    	
    	if(hCoeffOutOfBound(hCoeffPtr))
    	{
    		/* If the hCoeffs. are out of bound
    			after transfering, erase the last graph and 
    			redraw the grid lines. */
    		redrwGrdLines(linToXhyhCmndPtrHst, hCoeffPtr);  			
    	}
    	else
    	{
    		/* Form the h coeff. x coords. array after we
    	   have finished transferring the coeffs. from
    	   LabVIEW. */
    		x2Ptr = formHCoeffXCoords(hCoeffPtr);
    		/* if hCoeffs. are not out of bound after transfering,
    			clear graph, build the LCD y coords., redraw
    			the grid lines, draw the new graph and save 
    			the new graph to the graph history	pointer. 
    				
    			When you first run the program,  where does 
    			linToXhyhCmndPtrHst comes from? */
    		redrwGrdLines(linToXhyhCmndPtrHst, hCoeffPtr);
    		
	    	/* draw graph and save to graph history pointer */
	    	linToXhyhCmndPtrHst = 
	    		drwhCoeffGrph(hCoeffPtr, y1sPtr, y2Ptr, x2Ptr, setColorHstry);
	      /* linToXhyhCmndPtrHst points to the complete h coeff 
	         command array. */	
    	}    	
  	}	/* if(pack) */
  	
  	/* poll the lcd for touchscreen response */
  	delays();
  	
  	/* may want to use an array to pointers
      to search for any touch screen button
      instead. */
    /* this function sends out command 0x00 to
	   the lcd to receive a touch screen response. */
    monitorScreen(hCoeffPtr, y1sPtr, y2Ptr, x2Ptr, adcResBaseYCoordPtr, 
    				lcdAdcBaseYCoordPtr, adcXCoordPtrRepl);
    //sndLcdQspiByte(0x00);
        
	} /* forever loop */
}/* main */