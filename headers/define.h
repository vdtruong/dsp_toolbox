/* 10/31/09	#defines for all functions and routines */

#define QSPIIL				   4		/* qspi int. level */
#define QSPIBITCNT			8		/* qspi transf/recv bits */
#define QSPIINTFLG			0		/* set qspi for interrupt or not */
#define QSPIBAUD				30	   /* set qspi b.r. */
#define UART0IL				6		/* UART0 Interrupt level */
#define UART0PL				6		/* UART0 priority level */
#define COEFFENDFLG			0xAA  /* hCoeff array end flag */
#define DIFFABSVAL		   33    /* absolute value for hCoeff scale 66 */
#define STARTXPIXEL 	   	73	   /* starting x coord pixel */
#define XPIXELRES			   1     /* x coord pixel resolution */
#define Y1STARTSCL			4915  /* starting y1s scale (h coeff) */
#define Y1SRESEVEN			33	   /* y1s resolution for even index (hcoeff) */
#define Y1SRESODD				33	   /* y1s resolution for odd index (hcoeff) */
#define STARTYPIXEL			75	   /* starting y coord pixel */
#define YPIXELRES				1		/* y coord pixel resolution (y2) */
#define COEFFSIZE				100   /* h coeffs. from labview */
#define SETCOEFFSIZE	   	400	/* We set the coefficient array to a
										      preset size.  Right now we will				   
												preset it to 400 coefficients.  Actually,		   
												we will only send half of the coefficients		   
												array.  But each coefficient is two bytes,		   
											   therefore, we will actually use 400			 
												bytes of the array to receive it. We may		   
												not send up to 400 coefficients.  It could		   
												be 400 or fewer, but even amount. */		   
#define SETCOEFFXCOORD	   400   /* We like to set the hCoeff horizontal box
                                    to 400.  If we have less, we'll skip
                                    certain amount of pixel. For example, if
                                    we only transfer 100 coefficients, we'll
                                    skip every other 4. */
#define ARRAYSIZE 		   512	/* adc samples */
#define BYTESIZE 		   	2  	/* byte size of h coefficient.  Each h coeff. is 
												16 bits, therefore, each coefficient will 			 
												need two bytes to be transferred.  After 			 
												receiving it, we will repack these two bytes			  
												to form one h coefficient. */ 			 
#define Y1SLENGTH			   301 	/* length of y coords. (h coeff) */
#define PLOT_XHYH				0x3e  /* plot xy coord for ezlcd-105 */
#define LINE_TO_XHYH 	   0x3f  /* draw a line to position */
#define SET_XHYH			   0x33  /* pick a point on the lcd screen */
#define BOXHH				   0xa4
#define BOXHH_FILL		   0xa5
#define XHYHDATAQ 			2 	   /* number of commands and data to que for 
											   xhyh cmnds. 2*5=10 addresses, allowable
												is 16 addresses. */
#define ADCDATAQ 			   2 		   /* number of commands and data to que for 
												adc xhyh cmnds. 2*5=10 addresses, allowable
												is 16 addresses. */
#define XHYHBYTES				5		/* number of bytes in a xhyh command */
#define EZBUTTON			   1
#define CUBUTTON			   2
#define CALIBXY			   64
#define LIGHTBRIGHT			150	/* set the brightness of the backlight.
												scale is 0-255. */
#define	XGRDRES				16 	/* pixels between each grid line on
												the x-axis. */
											/* left vertical border pixel */				 
#define	LFTVERTBRD		STARTXPIXEL
											/* right vertical border pixel */
#define	RGHTVERTBRD		XPIXELRES*SETCOEFFXCOORD+LFTVERTBRD /* 1*400 + 73 = 473 */		
#define	TOPHORZBRD		   75		/* top horizontal border pixel */
#define	BOTHORZBRD		   375	/* bottom horizontal border pixel */
#define	YGRDRES			   50 	/* pixels between each grid line on
								   				the y-axis. */
#define LCDADCYCOORDLGTH   381	/* length of lcd adc y coords. base, 
												456 - 75 = 330 */
#define ADCRES3_3V			32760 /* highest value of adc res. is 3.3 V, 
                                    32760 = 4095 * 8. This is because
                                    the result has been left shifted by 3.
                                    2^3 = 8. Refer to the datasheet for
                                    more information. */
#define ADCRESRES				99		/* adc result resolution of 99 (0.01V)
												on the lcd screen, not the actual						
												adc result in the micro-controller.				 	   
												The actual resolution of the 				 	   
												microcontroller adc is 3.3/4096 = .0008.				 	   
												but to carry out this resolution, the				 	   
												length of the lcd screen will be too				 	   
												long for what we have.  So we'll settle				 	   
												for 0.01V which is between 104 and 96 raw				 	   
												data.  But we'll average it for 99, this				 	   
												is so we don't have to deal with floating				 	   
												math.  */				 	   
#define MAXXCOORDLGTH		512	/* maximum x coord. length */
#define ADCXPIXELRES			1     /* adc result x coord pixel resolution */

#define STRTYTXTCOORD	   30		/* Start the 0.4 text. */
#define YCOORD040			   75		/* Y Location of 0.4 text.*/
#define YCOORD040			   75		/* Y Location of 0.3 text.*/
#define ADCLFTBRD			   48    /* adc box left border starting point */
