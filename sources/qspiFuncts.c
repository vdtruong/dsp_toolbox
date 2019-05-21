/* 11/1/09	Qspi functions. */

/* prototypes */
void init_spi(unsigned char bitcnt, unsigned int Interrupt);
void write_to_qspi_ram(uint8 address, uint16 data);
uint16 read_from_qspi_ram(uint8 address);
void start_spi_trans(uint8 bytes, uint8 csiv);
void LcdSendCommand(char cCommand);
//__declspec(interrupt) void qspiRecvIntHndlr();
/*********************************************************/

/* functions begin */
void init_spi(unsigned char bitcnt, unsigned int Interrupt)
{
	/* pins QSx assumes primary function of qspi */
	MCF_GPIO_PQSPAR = MCF_GPIO_PQSPAR_PQSPAR3(0x1) |
		MCF_GPIO_PQSPAR_PQSPAR2(0x1) |
		MCF_GPIO_PQSPAR_PQSPAR1(0x1) |
		MCF_GPIO_PQSPAR_PQSPAR0(0x1);
	/* qspi clk delay and delay after transfer */
	MCF_QSPI_QDLYR = 0x3088; 
	// QSPI interrupt
	MCF_QSPI_QIR = Interrupt;
	if(Interrupt != 0)
	{
		/* 	QSPI interrupt is ICR18, 
				so the vector table index is 18+64=0x52. */
		/* set upt int. vector 18 */
		// interrupt level is 4.
		MCF_INTC0_ICR18 = MCF_INTC_ICR_IL(QSPIIL);
		/* unmask bit 18 of imrh, set for int. enabled */
		MCF_INTC0_IMRH &= ~ MCF_INTC_IMRL_INT_MASK18;
		MCF_QSPI_QIR |= 0xf; /* clear any interrupts */
	}
	// QMR[BAUD] = fsys/ (2 × [desired QSPI_CLK baud rate])
	// Using 16 yields a baud rate of 2.5 MHz
	MCF_QSPI_QMR = (0|
		MCF_QSPI_QMR_MSTR|
		//MCF_QSPI_QMR_CPHA| // cpha=0
		MCF_QSPI_QMR_BITS(bitcnt)|
		MCF_QSPI_QMR_BAUD(QSPIBAUD)
		);
	/* data direction set for input */
	/* mosi, miso=input, sck, pcs0 */	
	MCF_GPIO_DDRQS = 0x0d; 
}
void write_to_qspi_ram(uint8 address, uint16 data)
{
	MCF_QSPI_QAR = address;
	MCF_QSPI_QDR = data;
}
uint16 read_from_qspi_ram(uint8 address)
{
	MCF_QSPI_QAR = address;
	return( MCF_QSPI_QDR );
}
void start_spi_trans(uint8 bytes, uint8 csiv)
{
	/* This function configures the QWR register, 
	starts the transfer of the QSPI, and polls the SPIF flag for the
	end status. */
	/* clear the complete flag; qspi finished flag. */
	/* clear by writing a 1 to it. */
	MCF_QSPI_QIR = MCF_QSPI_QIR_SPIF; 
	/*	qspi_cs inactive level, i.e., hi or low 
		when not using qspi. */
	if( csiv == 1 )
		MCF_QSPI_QWR = MCF_QSPI_QWR_ENDQP(bytes-1)|MCF_QSPI_QWR_CSIV;
	else
		/* points to end of que */
		MCF_QSPI_QWR = MCF_QSPI_QWR_ENDQP(bytes-1);
	/* Start Xfer, from beginning to end of que address. */
	MCF_QSPI_QDLYR = MCF_QSPI_QDLYR_SPE;
	while(!(MCF_QSPI_QIR & MCF_QSPI_QIR_SPIF))
	{
	// Spin here waiting for completion of # of bytes.
	};
}
void LcdSendCommand(char cCommand)
{
	write_to_qspi_ram(MCF_QSPI_QAR_TRANS,0xf8);
	write_to_qspi_ram(MCF_QSPI_QAR_TRANS+1,cCommand & 0xf0);
	write_to_qspi_ram(MCF_QSPI_QAR_TRANS+2,cCommand << 4);
	write_to_qspi_ram(MCF_QSPI_QAR_CMD,MCF_QSPI_QDR_CONT|0x0F00);
	write_to_qspi_ram(MCF_QSPI_QAR_CMD+1,MCF_QSPI_QDR_CONT|0x0F00);
	write_to_qspi_ram(MCF_QSPI_QAR_CMD+2,MCF_QSPI_QDR_CONT|0x0F00);
	start_spi_trans(3,0); //transfer 3 element of the queue, no csiv
	//the LCD module need time to execute the command
	//delay(40); 
	/* the cont flag is for transfering more than one que entry */
}
__declspec(interrupt) void qspiRecvIntHndlr()
{
	/* this isr check to see if the
		 lcd was touched and which button. 
		 It goes through all the qspi ram
		 addresses that was used. 
		 After the qspi has finished sending
		 all the data, the spif will be set
		 and this isr will be called. */
	uint8 address;
	uint16 data[10];
	
	/* clear spif flag by writing a 1 to it */
	MCF_QSPI_QIR |= MCF_QSPI_QIR_SPIF;
	/* cycle through all receive ram addresses
		 to read the data. */
	for(address=0; address<10; address++)
	{
		*(data+address) = read_from_qspi_ram(address);
		if ( *(data+address)>>6 &3 )/* if touched */
		{
			switch(*(data+address) & 63)
			{
				case 2:
					setGrphColorPtr = setColBlue;
					break;
				case 3:
					setGrphColorPtr = setColRed;
					break;
				case 4:
					setGrphColorPtr = setColBlk;
					break;
				default:
					setGrphColorPtr = setColBlk;
					break;
			}
		}
	}
}