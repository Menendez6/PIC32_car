#ifndef _US_H
#define _US_H
// ------------------------------------------------------
/**
* Configura los pines de un sensor de ultrasonidos.
*
* Cualquier pin del PORTA es v�lido. No verifica si
* los pines est�n ya en uso. A lo largo del driver se
* utiliza el pin de eco para identificar el sensor.
*
* @param pin_disparo Trigger (salida digital).
* @param pin_eco Medida (entrada digital).
*/

void configurarPinesUS(unsigned char pin_disparo,unsigned char pin_eco);
// ------------------------------------------------------
/**
* Funci�n de configuraci�n del driver de ultrasonidos.
*
* Debe llamarse despu�s de haber configurado los pines.
* Usa el Timer 3.
*/
void inicializarUS(void);
// ------------------------------------------------------
/**
* Devuelve la �ltima medida de un sensor US.
*
* Si a�n no se ha realizado una medida o no se
* ha detectado un obst�culo, devuelve 511.
*
* @param pin_eco �ndice del pin del PORTA [0, 15].
*
* @return Tiempo de vuelo. Cada unidad equivale a 50 us.
*/
float getMedidaUS(unsigned char pin_eco);
#endif