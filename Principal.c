/********************************************************************************/
/*			Manejo de una cerradura electrica con tarjetas RFID.				*/
/*				Revisión:				1.00									*/
/*				PIC:					PIC16F628 - PIC16F628A - PIC16F648A		*/
/*				Compilador:				MPLAB IDE 8 - HI-TECH 9.60				*/
/*				Checksum:				0x0105	(PROTEGIDO)						*/
/*										0xca23	(ABIERTO)						*/
/*				Fecha de creación:		26/10/2009								*/
/*				Fecha de modificación:	04/11/2010								*/
/*				Autor:					Mariano Ariel Deville					*/
/********************************************************************************/
#define		PIC_CLK		20000000	// 20Mhz.									*/
#define		ENTRADA		1			//											*/
#define		SALIDA		0			//											*/
#define		TA			2			// Tiempo de accionamiento del rele en seg.	*/
/*------------------------------------------------------------------------------*/
/*				Defino los nombres de los pines de E/S							*/
#define		LED_V		RA1			// Manejo el led verde.						*/
#define		LED_R		RA0			// Manejo el led rojo.						*/
#define		RELE		RA2			// Manejo el rele de la cerradura.			*/
/********************************************************************************/
/*						VARIABLES GLOBALES										*/
unsigned char rs_str[16],pos,resp;	//											*/
bit bandera;
/********************************************************************************/
/*						PROTOTIPO DE FUNCIONES									*/
void Proceso_Tarjeta(void);
void Habilito_RS232(void);
void Comprimo_Hexa(void);
/********************************************************************************/
#include	"htc.h"					// Necesario para el compilador.			*/
#include	"Utilidades.c"			// Funcion que compara cadenas.				*/
#include	"delay.c"				// Rutinas de demoras.						*/
#include	"RS232.c"				// Comunicación por puerto serie.			*/
#include	"eeprom.c"				// Manejo memoria eeprom.					*/
#include	"Interrup.c"			// Manejo las interrupciones.				*/
/********************************************************************************/
__CONFIG(PROTECT & LVPDIS & BOREN & PWRTEN & WDTEN & MCLREN & HS);	//		*/
/********************************************************************************/
void main(void)
{
/********************************************************************************/
/*				Configuración de los puertos									*/
/*------------------------------------------------------------------------------*/
	PORTA=0;					// Reseteo PORTA.								*/
	PORTB=0;					// Reseteo PORTB.								*/
/*------------------------------------------------------------------------------*/
	TRISA0=SALIDA;				// Indicador luminoso rojo.						*/
	TRISA1=SALIDA;				// Indicador luminoso verde.					*/
	TRISA2=SALIDA;				//												*/
	TRISA3=SALIDA;				// RELE cerradura.								*/
	TRISA4=SALIDA;				//												*/
/*------------------------------------------------------------------------------*/
	TRISB0=SALIDA;				//												*/
	TRISB1=ENTRADA;				// RX - RS232.									*/
	TRISB2=SALIDA;				// TX - RS232.									*/
	TRISB3=SALIDA;				//												*/
	TRISB4=SALIDA;				//												*/
	TRISB5=SALIDA;				//												*/
	TRISB6=ENTRADA;				// ICSP.										*/
	TRISB7=ENTRADA;				// ICSP.										*/
/********************************************************************************/
/*			TIMER 1 - NO UTILIZADO												*/
/*------------------------------------------------------------------------------*/
	T1OSCEN=0;					// Oscilador deshabilitado.						*/
	TMR1IF=0;					// Bajo la bandera de la interrupción.			*/
	TMR1ON=0;					// Apago el TMR1.								*/
/********************************************************************************/
/*			Configuración de las interrupciones									*/
/*------------------------------------------------------------------------------*/
	RBIE=0;						//												*/
	INTE=0;						// Interrupcion RB0/INT deshabilitada.			*/
	T0IE=0;						//												*/
	TMR1IE=0;					//												*/
	TMR2IE=0;					// Interrupcion desborde TMR2 deshabilitada.	*/
	CCP1IE=0;					//												*/
	TXIE=0;						//												*/
	CMIE=0;						// Comparator Interrupt disable.				*/
	RCIE=0;						// Interrupcion recepcion USART deshabilitada.	*/
	EEIE=0;						//												*/
	RBPU=1;						// RB pull-ups estan deshabilitadas.			*/
	GIE=1;						// Utilizo interrupciones.						*/
	PEIE=1;						// Interrupcion externa habilitada.				*/
/********************************************************************************/
	Serial_Setup(9600);			// Setea el puerto serie.
	resp=EE_Read(0);			// Leo el primer caracter de la memria.
	if(resp!=35)				// No se cargo ninguna tarjeta aun?
		Borrar_Tarjetas();		// Me aseguro que quede limpia la memoria.
	Habilito_RS232();			// Abro el puerto RS232 en espera de la lectura de una tarjeta.
	for(;;)
	{
		if(!RCIE)				// Llego algo por el puerto RS232?
		{
			Proceso_Tarjeta();	// Proceso el número de tarjeta recibido.
			PORTA=0;			// Reseteo PORTA, apagando todos los indicadores.
			Habilito_RS232();	// Habilito la recepción por RS232.
		}
		CLRWDT();
	}
}
/********************************************************************************/
/*	Despues de una lectura excitosa proceso la información recibida.			*/
/*------------------------------------------------------------------------------*/
void Proceso_Tarjeta(void)
{
	unsigned int cont;
	resp=Buscar_Base();				// Busco si la tarjeta leida esta en memoria.
	if(!resp)						// Hay tarjetas cargadas?
	{
		PORTA=0B00000011;			// Indeco con los dos leds el grabado de la tarjeta.
		Guardar_Tarjeta();			// Guargo el numero de la tarjeta en la EEPROM del PIC.
		DelayMs(250);
		return;						// Salgo de la función.
	}
	if(resp==1)						// La tarjeta leida es la primera guardada en memoria?
	{
		LED_V=1;					// Led verde indica agregado.
		DelayMs(250);				// Demora para evitar problemas de lectura en el RS232.
		cont=0;
		Habilito_RS232();			// Comienzo una nueva lectura.
		while(RCIE)					// Leo una nueva tarjeta.
		{
			DelayMs(10);
			if(cont++>500)			// Time out?
				return;				// Salgo sin hacer nada.
		}
		resp=Buscar_Base();			// Busco si la tarjeta leida esta en memoria.
		if(resp==3)					// La tarjeta no esta en la base, la agrego.
		{
			Guardar_Tarjeta();		// Guardo en memoria la nueva tarjeta.
			return;
		}
		if(resp==1)					// Volvi a leer la tarjeta maestra, paso a modo borrado.
		{
			DelayMs(250);
			cont=0;
			Habilito_RS232();
			while(RCIE)				// Leo una nueva tarjeta.
			{
				PORTA=0B00000000;	// Led rojo titilando indica borrado.
				DelayMs(20);
				PORTA=0B00000001;	// Led rojo titilando indica borrado.
				DelayMs(20);
				if(cont++>100)		// Time out?
					return;			// Salgo sin hacer nada.
			}
			resp=Buscar_Base();		// Busco si la tarjeta leida esta en memoria.
			if(resp==1)				// Es la tarjeta maestra?
			{
				PORTA=0B00000001;	// Led rojo indica el borrado de toda la memoria.
				Borrar_Tarjetas();	// Elimino todo el contenido de la memoria.
				DelayS(1);			// Confirmo visualmente el borrado.
				return;
			}
		}
		return;						// Salgo de la función.
	}
	if(resp==2)						// Se encontro la tarjeta y está habilitada.
	{
		PORTA=0B00000110;			// Prendo el led verde y el relé.
		DelayS(TA);					// Tiempo de accionamiento.
		return;						// Salgo de la función.
	}
	if(resp==3)						// La tarjeta no esta en la base.
	{
		LED_R=1;					// Enciendo el led indicador.
		DelayS(1);
		return;
	}
	return;
}
/********************************************************************************/
/*	Habilito la interrupción por RS232 y reseteo las variables relacionadas.	*/
/*------------------------------------------------------------------------------*/
void Habilito_RS232(void)
{
	rs_str[0]=0;
	pos=0;						// Reseteo la posición de la cadena.
	CREN=1;						// Habilito la recepcion continua en RS232.
	RCIE=1;						// Habilito la interrupcion del puerto serie.
}
/********************************************************************************/
/*	Habilito la interrupción por RS232 y reseteo las variables relacionadas.	*/
/*------------------------------------------------------------------------------*/
void Comprimo_Hexa(void)
{
	bandera=0;
	for(pos=0;pos<10;pos++)
	{
		rs_str[pos]-=48;
		if(rs_str[pos]>49)
			rs_str[pos]-=40;
		if(rs_str[pos]>16)
			rs_str[pos]-=7;
		if(!bandera)
			rs_str[pos/2]=rs_str[pos]<<4;	// Cargo un hexa en la parte alta del byte.
		else
			rs_str[pos/2]+=rs_str[pos];		// Cargo un hexa en la parte baja del byte.
		bandera=!bandera;					// Me indica en que parte del byte debo cargar el hexa.
	}
}
