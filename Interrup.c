/********************************************************************************/
/*								INTERRUPCIONES									*/
/********************************************************************************/
void interrupt isr(void)
{
	if(RCIF && RCIE)			// Interrupcion por RS232?
	{
		resp=RCREG;				// Vac�o el buffer del m�dulo RS232.
		if(resp==13&&pos)		// Condici�n de salida.
		{
			RCIE=0;				// Deshabilito la interrupci�n por RS232.
			CREN=0;				// Deshabilito la recepci�n continua.
			rs_str[pos]=0;		// Marco final de cadena.
			Comprimo_Hexa();	//
			return;				// Salgo de la interrupci�n.
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
