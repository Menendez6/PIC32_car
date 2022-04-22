/* 
 * File:   main.c
 * Author: pablo
 *
 * Created on 13 de marzo de 2022, 12:44
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <xc.h>
#include <math.h>
#include "Pic32Ini.h"
#include "UART1.h"
#include "us.h"
#include "coche.h"




/*
 * 
 */

/*Cosas pendientes:
 * Mirar lo de las diagonales / solo girar un lado => No muy importante
 * Buzzer pillar una resistencia para que suene
 * Cambio de velocidad
 */

int main(void) {
    
    
    unsigned char pin_disparo = 4; //RC4 -> duda: ¿pk en el ejemplo de moodle lo pone como char?
    unsigned char pin_eco = 5; //RC5
    float medida;
    float distancia = 0.;
    
    char c;
    
    
    inicializarCoche();
    InicializarUART1(9600);
    configurarPinesUS(pin_disparo,pin_eco);
    inicializarUS();
    
    INTCONbits.MVEC = 1;
    asm("   ei");
    
    
    while (1){
        medida = getMedidaUS(pin_eco);
        //Cálculo de la distancia en cm
        distancia = 0.5*34000.*medida*50.E-6;
        
        if (distancia < 25.){
            asm(" di");
            LATC &= ~0xF;//enciendo todos los leds si la distancia es menor que 25 cm
            asm(" ei");
        }else{
            asm(" di");
            LATC |= 0xF; //los apago si no
            asm(" ei");
        }
        
        c = getcUART();
        modificarCoche(c,distancia);
    }

    return 0;
}
