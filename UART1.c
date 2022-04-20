/* 
 * File:   main.c
 * Author: pablo
 *
 * Created on 13 de marzo de 2022, 12:05
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <xc.h>

#define TAM_COLA 100
static char cola_rx[TAM_COLA];
static int icab_rx = 0;
static int icol_rx = 0;

static char cola_tx[TAM_COLA];
static int icab_tx = 0;
static int icol_tx = 0;


void InicializarUART1(int baudios){ 
    
    // Activamos las interrupciones del receptor.
    IFS1bits.U1RXIF = 0; // Borro flag por si acaso
    IEC1bits.U1RXIE = 1; // Habilito interrupciones
    IFS1bits.U1TXIF = 0; // Borro flag del transmisor por si acaso
    IPC8bits.U1IP = 3; // Prioridad 3
    IPC8bits.U1IS = 1; // Subprioridad 1
    // Conectamos U1RX y U1TX a los pines RB13 y RB7 del micro
    
    ANSELB &= ~((1<<13)|(1<<7)); // Pines digitales
    TRISB |= 1<<13; // Y RB13 como entrada
    LATB |= 1<<7; // A 1 si el transmisor está inhabilitado.
    SYSKEY=0xAA996655; // Se desbloquean los registros
    SYSKEY=0x556699AA; // de configuración.
    U1RXR = 3; // U1RX conectado a RB13.
    RPB7R = 1; // U1TX conectado a RB7.
    SYSKEY=0x1CA11CA1; // Se vuelven a bloquear.
    
    U1STAbits.URXISEL = 0; // Interrupción cuando llegue 1 char.
    U1STAbits.UTXISEL = 2; // Interrupción cuando FIFO vacía.
    U1STAbits.URXEN = 1; // Se habilita el receptor.
    U1STAbits.UTXEN = 1; // Y el transmisor
    
    if (baudios > 38400){
        U1BRG = (int) round((1250000 / baudios)-1); //Devuelve el valor con decimales, 
                                     //lo necesitamos en entero, aproximado al más cercano
        U1MODE = 0x8008;
    } else{
        U1BRG = (int) round((312500 / baudios)-1);
        
        U1MODE = 0x8000; // Se arranca la UART
    } 
    
}

void putsUART(char s[]){
    int cont = 0;
    
    while(s[cont]!= '\0'){
        if( (icab_tx+1 == icol_tx) || (icab_tx+1 == TAM_COLA && icol_tx == 0)){
            // La cola está llena. Se aborta el envío de
            // los caracteres que restan
            break;
        }else{
            cola_tx[icab_tx] = s[cont]; // Copia el carácter en la cola
            cont++; // Apunto al siguiente carácter de la cadena
            icab_tx++;
            if(icab_tx == TAM_COLA){
                icab_tx = 0;
            }
        }
    }
    IEC1bits.U1TXIE = 1;
    
}

char getcUART(void){
    char c;
    
    if (icol_rx != icab_rx){
        c = cola_rx[icol_rx];
        icol_rx ++;
        if (icol_rx == TAM_COLA){
            icol_rx = 0;
        }
    }else{
        c = '\0';
    }
    
    return c;
}

__attribute__((vector(32), interrupt(IPL3SOFT),nomips16))
void InterrupcionUART1(void){
    if(IFS1bits.U1RXIF == 1){ // Ha interrumpido el receptor
        if( (icab_rx+1 == icol_rx) || (icab_rx+1 == TAM_COLA && icol_rx == 0)){
            // La cola está llena
        }else{
            cola_rx[icab_rx] = U1RXREG; // Lee carácter de la UART
            icab_rx++;
            if(icab_rx == TAM_COLA){
                icab_rx = 0;
            }
        }
        IFS1bits.U1RXIF = 0; // Y para terminar se borra el flag
    }
    if(IFS1bits.U1TXIF == 1){ // Ha interrumpido el transmisor
        // Se extrae un carácter de la cola y se envía
        if(icol_tx != icab_tx){ // Hay datos nuevos
            U1TXREG = cola_tx[icol_tx];
            icol_tx++;
            if(icol_tx == TAM_COLA){
                icol_tx = 0;
            }
        }else{ // Se ha vaciado la cola.
            IEC1bits.U1TXIE = 0; // Para evitar bucle sin fin
        }
        
        IFS1bits.U1TXIF = 0; // Y para terminar se borra el flag
    }
}

