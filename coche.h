#ifndef _COCHE_H
#define _COCHE_H
// ------------------------------------------------------
/**
* Inicializa los valores de salida de puertos B y C y temporizador 2
*
*/

void inicializarCoche();
// ------------------------------------------------------
/**
* Función de cambio de estado del coche, en función del char c y la distancia a la que se encuentre
*
* @param char c: caracter que recibe desde la UART
* @param float distancia: distancia medida por el sensor
*/
void modificarCoche(char c, float distancia);
// ------------------------------------------------------
#endif