/****************************************************************************/
/*				MANEJO DE LA MEMÓRIA EEPROM DEL PIC 16F____					*/
/*--------------------------------------------------------------------------*/
/*						PROTOTIPOS DE FUNCIONES								*/
unsigned char EE_Read(unsigned char address);				//				*/
void EE_Write(unsigned char address,unsigned char value);	//				*/
unsigned char buscar_base(void);							//				*/
void Guardar_Tarjeta(void);									//				*/
void Borrar_Tarjetas(void);									//				*/
/*																			*/
#if defined(_16F648A)		// Capacidad de memoria según el modelo de PIC.	*/
	#define LLENO	255		//												*/
#else						//												*/
	#define LLENO	127		//												*/
#endif						//												*/
/****************************************************************************/
/*		DEVUELVO EL BYTE LEIDO EN LA DIRECCIÓN PASADA COMO ARGUMENTO		*/
/*--------------------------------------------------------------------------*/
unsigned char EE_Read(unsigned char address)
{
	CLRWDT();
	EEADR=address;				// Paso la dirección en la que voy a leer.
	RD=1;						// Inicio el ciclo de lectura.
	while(RD)					// Espero a que termine la lectura.
		continue;				// Si demora mucho actua el WDT.
	return EEDATA;				// Devuelvo el valor leido.
}
/****************************************************************************/
/*				ESCRIBO UN BYTE EN LA MEMORIA EEPROM						*/
/*--------------------------------------------------------------------------*/
void EE_Write(unsigned char address,unsigned char value)
{
	CLRWDT();
	EEADR=address;				// Paso la dirección en la que voy a leer.
	EEDATA=value; 				// Paso el byte que deseo escribir.
	EECON1&=0x7F;
 	WREN=1;						// EEPROM modo escritura.
	EECON2=0x55;
	EECON2=0xAA;
	WR=1;						// Escribo en memoria.
 	while(WR)					// Espero que termine de escribir.
		continue;				// Si demora mucho actua el WDT.
	WREN=0;						// Deshabilito EEPROM modo escritura.
	return;
}
/****************************************************************************/
/*			Busco si está almacenada en memoria la tarjeta.					*/
/*--------------------------------------------------------------------------*/
unsigned char Buscar_Base(void)
{
	unsigned char i,cad[9];
	pos=1;
	while(pos<LLENO-4)					// Cantidad de paginas en la memoria.
	{
		bandera=0;
		for(i=0;i<5;i++)
		{
			cad[i]=EE_Read(pos++);		// Leo un caracter de la memoria.
			if(cad[i])
				bandera=1;				// Bandera de que esta ocupado el lugar.
		}
		if(!bandera&&pos<7)
			return 0;					// Salgo de la funcion y aviso vacio.
		if(Comparo_Cadenas(cad,rs_str))	// Coincide el número leido con el remitente?
		{
			if(pos<7)
				return 1;				// La tarjeta esta y es la primera.
			else
				return 2;				// La tarjeta esta y no es la primera.
		}
	}
	return 3;							// No encontre coincidencia, salgo y aviso.
}
/****************************************************************************/
/*			Busco lugar libre y guardo en la EEPROM la tarjeta.				*/
/*--------------------------------------------------------------------------*/
void Guardar_Tarjeta(void)
{
	unsigned char i,pos;
	pos=1;
	bandera=0;
	while(!bandera)
	{
		bandera=1;
		for(i=0;i<5;i++)
		{
			if(EE_Read(pos++))
				bandera=0;			// bandera de que esta ocupado el lugar.
		}
		if(pos>LLENO-1)				// Supero la capacidad de memoria?
			return;					// Salgo
	}
	pos-=5;
	for(i=0;i<5;i++)
	{
		EE_Write(pos+i,rs_str[i]);
	}
	return;							// Salgo de la funcion.
}
/****************************************************************************/
/*			Borrado total de la memoria EEPROM del PIC.						*/
/*--------------------------------------------------------------------------*/
void Borrar_Tarjetas(void)
{
	volatile unsigned int i;
	for(i=1;i<=LLENO;i++)			// 
	{
		EE_Write(i,0);
	}
	EE_Write(0,'#');
	return;
}
