/*  dsp routines */
/*	3/26/11 Start work with dsp fir filter. 
/*  4/10/11 Try to typecast y as we collect temp. */

/********* Prototypes **********************************************/		
int32 *yFir(uint16 M, uint16 N, uint16 *adcResPtr, int16 *hCoeffPtr);
/*****************************************************************/

int32 *yFir(uint16 M, uint16 N, uint16 *adcResPtr, int16 *hCoeffPtr)
{
	/* This routine calculates the FIR result for symmetric and even M
	   h coefficients. M is the size of the h coeffs. array.  N is the
	   size of the adc results. This routine only uses half of the h
	   coeffs. array.  We save a lot of calculations. */
	   
	uint16 *xPtr1=0, *xPtr2=0, k=0, L=0, n=0;
	int16 *CoeffPtr=0;
	int32 temp = 0;
	static int32 y[ARRAYSIZE];
	//uint16 *yPtr;
	
	L = M - 1;
	
	for(n=0; n<N; n++)
	{
		temp = 0;
		CoeffPtr = hCoeffPtr; /* points back to the beginning of coeffs. array */
		xPtr1 = adcResPtr + n; /* points xPtr1 at n pos. of adcResPtr */
		xPtr2 = adcResPtr + (n-L); /* points xPtr2 to n-L pos. of adcResPtr */
		for(k=0; k<M/2; k++)
		{
			/* if memory location is less than the beginning
			   of the adc input,then set the value of the pointer
			   to 0. This is because we don't have any adc value
			   before index 0. */
			if(xPtr1 < adcResPtr)
			{
				*xPtr1 = 0;
			}
			if(xPtr2 < adcResPtr)
			{
				*xPtr2 = 0;
			}
			/* need to shift right 16 times to descale the hCoeffs..
			   It was shifted left so we don't have to deal with float
			   numbers. */			
			temp+= ((*xPtr1-- + *xPtr2++) * *CoeffPtr++)>>16;
			/**(y+n) = temp;*/
		}
		*(y+n) = temp;
	}
	
	return y;
}