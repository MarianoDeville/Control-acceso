/*						PROTOTIPOS DE FUNCIONES						*/
unsigned char Comparo_Cadenas(const char *c_1,const char *c_2);
/********************************************************************/
/*			COMPARO DOS CADENAS DE CARACTERES						*/
/********************************************************************/
unsigned char Comparo_Cadenas(const char *c_1,const char *c_2)
{
	unsigned char i;
	CLRWDT();
	for(i=0;i<5;i++)
	{
		if(*c_1++!=*c_2++)	// Comparo byte a byte de la cadena.
			return 0;		// Hay diferencia asi que salgo y aviso.
	}
	return 1;				// Las cadenas son iguales.
}
