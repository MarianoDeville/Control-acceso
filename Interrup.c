/********************************************************************************/
/*								INTERRUPCIONES									*/
/********************************************************************************/
void interrupt isr(void)
{
	if(RCIF && RCIE)			// Interrupcion por RS232?
	{
		resp=RCREG;				// Vacío el buffer del módulo RS232.
		if(resp==13&&pos)		// Condición de salida.
		{
			RCIE=0;				// Deshabilito la interrupción por RS232.
			CREN=0;				// Deshabilito la recepción continua.
			rs_str[pos]=0;		// Marco final de cadena.
			Comprimo_Hexa();	//
			return;				// Salgo de la interrupción.
		}
		if(resp>=48)			// Comienzo el llenado.
		{
			rs_str[pos]=resp;	// Lleno la cadena con los caracteres recibidos.
			if(pos<15)			// Queda lugar en la cadena?
				pos++;
			return;
		}
		return;
	}
	return;						// Salgo de las interrupciones.
}
