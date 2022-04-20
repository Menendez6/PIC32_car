/* 
 * File:   UART1.h
 * Author: pablo
 *
 */

#ifndef UART1_H
#define	UART1_H

#ifdef	__cplusplus
extern "C" {
#endif

void InicializarUART1(int baudios);
void putsUART(char s[]);
char getcUART(void);
void InterrupcionUART1(void);


#ifdef	__cplusplus
}
#endif

#endif