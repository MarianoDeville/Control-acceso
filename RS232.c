/********************************************************************************/
/*				COMUNICACION RS232 - RUTINAS DE ENVIO DE DATOS					*/
/********************************************************************************/
/*						PROTOTIPO DE FUNCIONES									*/
/*------------------------------------------------------------------------------*/
void Serial_Setup(unsigned int velocidad);
/********************************************************************************/
/*						SETEO EL PUERTO SERIE									*/
/*------------------------------------------------------------------------------*/
void Serial_Setup(unsigned int velocidad)
{
	float calculo;
	CLRWDT();
	calculo=PIC_CLK/velocidad;
	BRGH=0;					// Data rate for sending low speed.
	SPBRG=(unsigned char)(calculo/64)-1;
	SYNC=0;					// Asynchronous.
	SPEN=1;					// Enable serial port pins.
	CREN=0;					// Disable continuous reception.
	SREN=0;					// No effect.
	TXIE=0;					// Disable tx interrupts.
	RCIE=0;					// Disable rx interrupts.
	TX9=0;					// 8-bit transmission.
	RX9=0;					// 8-bit reception.
	TXEN=0;					// Disable transmitter.
	return;
}

