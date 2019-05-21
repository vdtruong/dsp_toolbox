/*
 * File:    m52259evb_sysinit.c
 * Purpose: Reset configuration of the M52259EVB
 *
 * Notes:   
 */

#include "support_common.h"
#include "exceptions.h"


#ifdef __cplusplus
#pragma cplusplus off
#endif // __cplusplus
/********************************************************************/


/********************************************************************/

void pll_init(void)
{


/*Required if booting with internal relaxation oscillator & pll off, clkmod[1:0]=00 & xtal=1 */

	MCF_CLOCK_OCLR = 0xC0; //turn on crystal
	MCF_CLOCK_CCLR = 0x00; //switch to crystal 
  MCF_CLOCK_OCHR = 0x00; //turn off relaxation osc

/*---------------------------------------------------------------------------------*/


	/* The PLL pre divider - 48MHz / 6 = 8MHz */
	MCF_CLOCK_CCHR =0x05;
	 
	 
	/* The PLL pre-divider affects this!!! 
	 * Multiply 48Mhz reference crystal /CCHR by 10 to acheive system clock of 80Mhz
	 */

	MCF_CLOCK_SYNCR &= ~(MCF_CLOCK_SYNCR_PLLEN);

  MCF_CLOCK_SYNCR |= MCF_CLOCK_SYNCR_CLKSRC | MCF_CLOCK_SYNCR_PLLMODE;
	
	MCF_CLOCK_SYNCR |= MCF_CLOCK_SYNCR_MFD(3) | MCF_CLOCK_SYNCR_RFD(2);
	
	MCF_CLOCK_SYNCR |= MCF_CLOCK_SYNCR_PLLEN;

	
	while (!(MCF_CLOCK_SYNSR & MCF_CLOCK_SYNSR_LOCK))
	{
	}
}

/********************************************************************/

void gpio_init(void) {

    /* Enable the proper UART pins */
    switch (TERMINAL_PORT)
    {
        case 2:
            MCF_GPIO_PUCPAR = 0
                | MCF_GPIO_PUCPAR_URXD2_URXD2
                | MCF_GPIO_PUCPAR_UTXD2_UTXD2;
            break;
        case 1:
            MCF_GPIO_PUBPAR = 0
                | MCF_GPIO_PUBPAR_URXD1_URXD1
                | MCF_GPIO_PUBPAR_UTXD1_UTXD1;
            break;
        case 0:
        default:
            MCF_GPIO_PUAPAR = 0
                | MCF_GPIO_PUAPAR_URXD0_URXD0
                | MCF_GPIO_PUAPAR_UTXD0_UTXD0;
    }
   
  /* PANPAR: PANPAR0=1, assumes adc primary function */
  /* this code dosn't seem to effect the functioning. */
  MCF_GPIO_PANPAR |= MCF_GPIO_PANPAR_PANPAR0/*(unsigned char)0x1*/
  									 /*MCF_GPIO_PANPAR_PANPAR1*/;

}

void allow_interrupts(void) 
{
    /* 
     * Allow interrupts from ABORT, SW1, and SW2 (IRQ[4,5,7]) 
     */
     
    /* Enable IRQ signals on the port */
    MCF_GPIO_PNQPAR = 0
        | MCF_GPIO_PNQPAR_IRQ1_IRQ1
        | MCF_GPIO_PNQPAR_IRQ3_IRQ3
        | MCF_GPIO_PNQPAR_IRQ5_IRQ5
        | MCF_GPIO_PNQPAR_IRQ7_IRQ7;
    
    /* Set EPORT to look for rising edges */
    MCF_EPORT_EPPAR = 0
        | MCF_EPORT_EPPAR_EPPA4_RISING
        | MCF_EPORT_EPPAR_EPPA5_RISING
        | MCF_EPORT_EPPAR_EPPA7_RISING;
        
    /* Clear any currently triggered events on the EPORT */
    MCF_EPORT_EPIER = 0
        | MCF_EPORT_EPIER_EPIE4
        | MCF_EPORT_EPIER_EPIE5
        | MCF_EPORT_EPIER_EPIE7;
        
    /* Enable interrupts in the interrupt controller */
    MCF_INTC0_IMRL &= ~(0
        | MCF_INTC_IMRL_INT_MASK4 
        | MCF_INTC_IMRL_INT_MASK5 
        | MCF_INTC_IMRL_INT_MASK7 
        | MCF_INTC_IMRL_MASKALL);
}

void adc_init(void)
{
	/* ### Init_ADC init code */
  /* CTRL1: STOP0=1,START0=0,SYNC0=1,EOSIE0=0,ZCIE=0,LLMTIE=0,HLMTIE=0,CHNCFG=0,SMODE=2 */
  MCF_ADC_CTRL1 = 0x5002U;                            
  /* CTRL2: DIV=0x17->23d, this divisor controls the sampling rate. */
  MCF_ADC_CTRL2 = (unsigned short int)((MCF_ADC_CTRL2 & (unsigned short int)~0x87C0) | 0x17U);     
  /* ADSDIS: DS7=1,DS6=1,DS5=1,DS4=1,DS3=1,DS2=1,DS1=1,DS0=0 */
  MCF_ADC_ADSDIS = 0xfeU/*FeU*/;                            
  /* ADSTAT: CIP0=0,CIP1=0,EOSI1=1,EOSI0=1,ZCI=0,LLMTI=0,HLMTI=0,RDY7=0,RDY6=0,RDY5=0,RDY4=0,RDY3=0,RDY2=0,RDY1=0,RDY0=0 */
  MCF_ADC_ADSTAT = 0x1800U;                            
  /* ADLSTAT: HLS7=1,HLS6=1,HLS5=1,HLS4=1,HLS3=1,HLS2=1,HLS1=1,HLS0=1,LLS7=1,LLS6=1,LLS5=1,LLS4=1,LLS3=1,LLS2=1,LLS1=1,LLS0=1 */
  MCF_ADC_ADLSTAT = 0xFFFFU;                            
  /* ADZCSTAT: ZCS7=1,ZCS6=1,ZCS5=1,ZCS4=1,ZCS3=1,ZCS2=1,ZCS1=1,ZCS0=1 */
  MCF_ADC_ADZCSTAT = 0xFFU;                            
  /* ADLLMT0: LLMT=0 */
  MCF_ADC_ADLLMT0 = 0x00U;                            
  /* ADLLMT1: LLMT=0 */
  MCF_ADC_ADLLMT1 = 0x00U;                            
  /* ADLLMT2: LLMT=0 */
  MCF_ADC_ADLLMT2 = 0x00U;                            
  /* ADLLMT3: LLMT=0 */
  MCF_ADC_ADLLMT3 = 0x00U;                            
  /* ADLLMT4: LLMT=0 */
  MCF_ADC_ADLLMT4 = 0x00U;                            
  /* ADLLMT5: LLMT=0 */
  MCF_ADC_ADLLMT5 = 0x00U;                            
  /* ADLLMT6: LLMT=0 */
  MCF_ADC_ADLLMT6 = 0x00U;                            
  /* ADLLMT7: LLMT=0 */
  MCF_ADC_ADLLMT7 = 0x00U;                            
  /* ADHLMT0: HLMT=0x0FFF */
  MCF_ADC_ADHLMT0 = 0x7FF8U;                            
  /* ADHLMT1: HLMT=0x0FFF */
  MCF_ADC_ADHLMT1 = 0x7FF8U;                            
  /* ADHLMT2: HLMT=0x0FFF */
  MCF_ADC_ADHLMT2 = 0x7FF8U;                            
  /* ADHLMT3: HLMT=0x0FFF */
  MCF_ADC_ADHLMT3 = 0x7FF8U;                            
  /* ADHLMT4: HLMT=0x0FFF */
  MCF_ADC_ADHLMT4 = 0x7FF8U;                            
  /* ADHLMT5: HLMT=0x0FFF */
  MCF_ADC_ADHLMT5 = 0x7FF8U;                            
  /* ADHLMT6: HLMT=0x0FFF */
  MCF_ADC_ADHLMT6 = 0x7FF8U;                            
  /* ADHLMT7: HLMT=0x0FFF */
  MCF_ADC_ADHLMT7 = 0x7FF8U;                            
  /* ADOFS0: OFFSET=0 */
  MCF_ADC_ADOFS0 = 0x00U;                            
  /* ADOFS1: OFFSET=0 */
  MCF_ADC_ADOFS1 = 0x00U;                            
  /* ADOFS2: OFFSET=0 */
  MCF_ADC_ADOFS2 = 0x00U;                            
  /* ADOFS3: OFFSET=0 */
  MCF_ADC_ADOFS3 = 0x00U;                            
  /* ADOFS4: OFFSET=0 */
  MCF_ADC_ADOFS4 = 0x00U;                            
  /* ADOFS5: OFFSET=0 */
  MCF_ADC_ADOFS5 = 0x00U;                            
  /* ADOFS6: OFFSET=0 */
  MCF_ADC_ADOFS6 = 0x00U;                            
  /* ADOFS7: OFFSET=0 */
  MCF_ADC_ADOFS7 = 0x00U;                            
  /* ADZCC: ZCE7=0,ZCE6=0,ZCE5=0,ZCE4=0,ZCE3=0,ZCE2=0,ZCE1=0,ZCE0=0 */
  MCF_ADC_ADZCC = 0x00U;                            
  /* ADLST1: SAMPLE3=3,SAMPLE2=2,SAMPLE1=1,SAMPLE0=0 */
  MCF_ADC_ADLST1 = 0x3210U;                            
  /* ADLST2: SAMPLE7=7,SAMPLE6=6,SAMPLE5=5,SAMPLE4=4 */
  MCF_ADC_ADLST2 = 0x7654U;                            
  /* CAL: SEL_VREFH=0,SEL_VREFL=0 */
  MCF_ADC_CAL = 0x00U;                            
  /* POWER: ASB=0,PSTS2=0,PSTS1=0,PSTS0=0,PUDELAY=0x0D,APD=0,PD2=1,PD1=1,PD0=1 */
  MCF_ADC_POWER = 0xD7U;                            
  /* CTRL1: STOP0=0,START0=0 */
  MCF_ADC_CTRL1 &= (unsigned short int)~0x6000;  
  MCF_ADC_POWER &= ~(MCF_ADC_POWER_PD0 | MCF_ADC_POWER_PD1 | MCF_ADC_POWER_PD2);
	// power-up converter A, converter B and voltage reference circuit 

	while (MCF_ADC_POWER & (MCF_ADC_POWER_PSTS0 | MCF_ADC_POWER_PSTS1 | MCF_ADC_POWER_PSTS2)) 
	{}; // stay here as long as converter A, B and voltage reference circuit are power-down              
}
/********************************************************************/
/* 
 * PLL min/max specifications
 */
#define MAX_FVCO    80000   /* KHz */
#define MAX_FSYS    80000   /* KHz */
#define MAX_FREF    10000   /* KHz */
#define MIN_FREF    2000    /* KHz */
#define MAX_MFD     18      /* Multiplier (not encoded) */
#define MIN_MFD     4       /* Multiplier (not encoded) */
#define MAX_RFD     128     /* Divider (not encoded) */
#define MIN_RFD     1       /* Divider (not encoded) */

/*
 * Low Power Divider specifications
 */
#define MIN_LPD     (1 << 0)    /* Divider (not encoded) */
#define MAX_LPD     (1 << 15)   /* Divider (not encoded) */

/* 
 * Operating Parameters
 * Pass to clock_pll() the 'flags' argument
 */
#define PLL_DISABLE   (0x0001)
#define PLL_CLKSRC    (0x0004)
#define PLL_FWKUP     (0x0020)
#define PLL_DISCLK    (0x0040)
#define PLL_LOCEN     (0x0080)
#define PLL_LOCRE     (0x0800)
#define PLL_LOLRE     (0x8000)
#define PLL_ALLFLAGS  (0x88E4)

/********************************************************************/
/*
 * Functions provided by this driver
 */
int
clock_pll (int, int, int);

int
clock_lpd (int);

/********************************************************************/


/********************************************************************/
/*
 * Initialize the Low Power Divider circuit
 * 
 * Parameters:
 *  div     Desired system frequency divider
 *
 * Return Value:
 *  The divider value applied
 *  -1 if divider was out of valid bounds
 */
int
clock_lpd (int div)
{
    int i, j;

    /* Check bounds of divider */
    if ((div < MIN_LPD) || (div > MAX_LPD))
        return (1 << (MCF_CLOCK_LPDR & 0x0F));

    /* Round divider down to nearest power of two */
    for (i = 0, j = div; j != 1; j >>= 1, i++) 
    {};
    
    /* Apply the divider to the system clock */
    MCF_CLOCK_LPDR = (uint8)MCF_CLOCK_LPDR_LPD(i);

    return (1 << i);
}
/********************************************************************/


/********************************************************************/
/* Initialize the PLL
 * 
 * Parameters:
 *  fref    PLL reference clock frequency in KHz
 *  fsys    Desired PLL output frequency in KHz
 *  flags   Operating parameters
 *
 * Return Value:
 *  The resulting output system frequency
 */
int
clock_pll (int fref, int fsys, int flags)
{
    int syncr, mfd_max, mfd_min, rfd_max;
    int i, temp, fout, mfd, rfd, done;
    
    /* Check for the disable flag */
    if (flags & PLL_DISABLE)
    {
        MCF_CLOCK_SYNCR &= ~MCF_CLOCK_SYNCR_PLLEN;
        return fref;
    }
    
    /* Check bounds of reference clock */
    if((fref >= MAX_FREF) || (fref <= MIN_FREF))
        return fref;
        
    if (fsys == 0)
    {
        /* Return current PLL output */
        if ((MCF_CLOCK_SYNCR & MCF_CLOCK_SYNCR_CLKSRC) &&
            (MCF_CLOCK_SYNCR & MCF_CLOCK_SYNCR_PLLEN))
        {
            mfd = (MCF_CLOCK_SYNCR & 0x7000) >> 12;
            mfd = MIN_MFD + (mfd * 2);
            rfd = (MCF_CLOCK_SYNCR & 0x0700) >> 8;
            rfd = 1 << rfd;
            return (fref * mfd / rfd);
        }
        else
            return fref;            
    }
    
    /* Check bounds of requested system clock */
    if (fsys > MAX_FSYS)
        fsys = MAX_FSYS;

    /* Determine maximum possible multiplier (must be even) */
    mfd_max = MAX_FVCO / fref;
    mfd_max &= ~1;
    if (mfd_max > MAX_MFD)
        mfd_max = MAX_MFD;
    
    /* Determine maximum possible output based on max multiplier */
    fout = fref * mfd_max;

    /* Determine target output based on fsys and max possible */ 
    if (fout > fsys)
        fout = fsys;
    
    /* Determine the minimum multiplier */
    for (mfd_min = mfd_max; 
         mfd_min > MIN_MFD && fout < (fref * mfd_min);)
         mfd_min-=2;
         
    /* Set preliminary divider maximum */         
    rfd_max = MAX_RFD;

    /*
     * Loop across the valid MFD and RFD settings starting with
     * the max MFD and min RFD and find the closest match less-than
     * or equal-to the desired output frequency
     */
    done = FALSE;
    for (mfd = mfd_max; mfd >= mfd_min; mfd-=2)
    {
        for (rfd = MIN_RFD; rfd < rfd_max; rfd <<= 1)
        {
            temp = fref * mfd / rfd;
            
            if (fout == temp)
            {
                /* exact match */
                done = TRUE;
                break;
            }
            if (fout > temp)
            {
                /* new upper bound for the rfd */
                rfd_max = rfd;
                /* is this the nearest match? */
                if ((rfd != 1) && ((fout - temp) < (fref / (rfd>>1))))
                    done = TRUE;
                break;
            }
        }
        if (done)
            break;
     }
    if (!done)
    {
        /* 
         * Fell out of loop before finding an exact match or getting 
         * as close as possible. Adjust mfd to nearest match.
         */
        mfd += 2;
    }
    
    fout = fref * mfd / rfd;

    /* Encode MFD and RFD settings */
    mfd = (mfd - MIN_MFD) / 2;
    for (i = 0; rfd > MIN_RFD; rfd >>= 1, i++) {};
    rfd = i;

    /*
     * Temp fix for PLL bug
     */
    (*(vuint8 *)(&__IPSBAR[0x120006])) |= 0x04;

    /* 
     * Initialize the PLL to generate the new system clock frequency 
     * A higher divider is used first with the desired MFD.  Once 
     * locked, the desired RFD is applied
     */
    syncr = MCF_CLOCK_SYNCR & ~(0x7700);
    temp = (MCF_CLOCK_SYNCR & 0x7000) >> 12;
    MCF_CLOCK_SYNCR = (uint16)(syncr 
        | MCF_CLOCK_SYNCR_RFD(rfd + 1)
        | MCF_CLOCK_SYNCR_MFD(temp)
        | MCF_CLOCK_SYNCR_PLLEN);
    MCF_CLOCK_SYNCR = (uint16)(syncr 
        | MCF_CLOCK_SYNCR_RFD(rfd + 1)
        | MCF_CLOCK_SYNCR_MFD(mfd)
        | MCF_CLOCK_SYNCR_PLLEN);
        
   	/* Wait for the PLL to lock */	
	while (!(MCF_CLOCK_SYNSR & MCF_CLOCK_SYNSR_LOCK)) {};

    /* Finish off the initialization */
    MCF_CLOCK_SYNCR = (uint16)(syncr
        | MCF_CLOCK_SYNCR_RFD(rfd)
        | MCF_CLOCK_SYNCR_MFD(mfd)
        | MCF_CLOCK_SYNCR_CLKSRC
        | MCF_CLOCK_SYNCR_PLLEN);

    return fout;
}


/********************************************************************/
/* Initialize                                                       */
/********************************************************************/

void __initialize_hardware(void)
{

	asm 
	{
	    /* Initialize IPSBAR */
	    move.l  #__IPSBAR,d0
	    andi.l  #0xC0000000,d0 // need to mask
	    add.l   #0x1,d0
	    move.l  d0,0x40000000

	    

	    /* Initialize FLASHBAR */
	    move.l  #__FLASHBAR,d0
	    andi.l  #0xFFF80000,d0 // need to mask
	    add.l   #0x61,d0
	    movec   d0,FLASHBAR

	}

	pll_init();
	gpio_init();
	adc_init();
	allow_interrupts();
	
	initialize_exceptions();
}
