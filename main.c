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

# define MOTOR_B_DIR 6 // Pines del shield PWM (son todos del puerto C)
# define MOTOR_B_PWM 7
# define MOTOR_A_DIR 9
# define MOTOR_A_PWM 8

static char szaux[256];

/*
 * 
 */

/*Cosas pendientes:
 * Mirar lo de las diagonales / solo girar un lado
 * Leds
 * Sensor
 * Cambio de velocidad
 * Si se desconecta => metemos un timer de 3 segundos en el que si no recibe nada,
 * se para. Simplemente creamos una interrupcion que vaya sumando una variable ticks 
 * y cuando ticks sea igual a un número concreto, lo paramos. Al entrar en la de enviar,
 * poner ticks a 0.
 */

int main(void) {
    
    //Los puertos B sí van
    //ANSELA = 0;
    //TRISA = 0;
    ANSELB = 0;
    TRISB = 0;
    ANSELC = 0;
    TRISC = 0xF000;
    LATC = 0XF00F;
    LATB=0xFFFF;
    //LATA = 0xFFFF;
    //PORTB = 0xFFFF;
    unsigned char pin_disparo = 4; //RC4 -> duda: ¿pk en el ejemplo de moodle lo pone como char?
    unsigned char pin_eco = 5; //RC5
    float medida;
    float distancia, distancia_ant = 0.;
    
    char c;
    
    
    InicializarUART1(9600);
    configurarPinesUS(pin_disparo,pin_eco);
    inicializarUS();
    
    INTCONbits.MVEC = 1;
    asm("   ei");
    
    
    while (1){
        medida = getMedidaUS(pin_eco);
        //Cálculo de la distancia en cm
        distancia = 0.5*34000.*medida*50.E-6;
        
        //sprintf(szaux, "La distancia es: %5.1f cm\n\r", distancia);
        //putsUART(szaux);
        
        if (fabs(distancia-distancia_ant) > 1.) {
            sprintf(szaux, "La distancia es: %5.1f cm\n\r", distancia);
            putsUART(szaux);
            distancia_ant =  distancia;
        }
        if (distancia < 7.){
            asm(" di");
            LATC &= ~0xF; //enciendo todos los leds si la distancia es menor que 7 cm
            asm(" ei");
        }else{
            asm(" di");
            LATC |= 0xF; //los apago si no
            asm(" ei");
        }
        
        c = getcUART();
        if (c!='\0'){
            if(c == 'F'){  //move forward(all motors rotate in forward direction)
                LATC |= (1<<MOTOR_A_PWM);
                LATC |= (1<<MOTOR_B_PWM);
                LATCCLR = 1;
              }
            else if(c == 'B'){      //move reverse (all motors rotate in reverse direction)
                LATC |= (1<<MOTOR_A_PWM);
                LATC |= (1<<MOTOR_B_PWM);
                LATC |= (1<<MOTOR_A_DIR);
                LATC |= (1<<MOTOR_B_DIR);
                LATCCLR = 1;
                //digitalWrite(12,HIGH);
                //digitalWrite(10,HIGH);
              }

            else if(c == 'L'){      //turn right (left side motors rotate in forward direction, right side motors doesn't rotate)
                LATC |= (1<<MOTOR_A_PWM);
                LATC |= (1<<MOTOR_B_PWM);
                LATC |= (1<<MOTOR_B_DIR);
                //digitalWrite(11,HIGH);
              }

            else if(c == 'R'){      //turn left (right side motors rotate in forward direction, left side motors doesn't rotate)
                LATC |= (1<<MOTOR_A_PWM);
                LATC |= (1<<MOTOR_B_PWM);
                LATC |= (1<<MOTOR_A_DIR);
                //digitalWrite(13,HIGH);
              }
            
            else if(c == 'W'){    //turn led on or off)
                LATBSET = 1<<15;
                //digitalWrite(9,HIGH);
              }
            else if(c == 'w'){
                LATBCLR = 1<<15;
                //digitalWrite(9,LOW);
              }
            else if(c == 'U'){ //turn led on or off)
                LATBSET = 1<<8;
                //digitalWrite(9,HIGH);
              }
            else if(c == 'u'){
                LATBCLR = 1<<8;
                //digitalWrite(9,LOW);
              }

            else if(c == 'S'){      //STOP (all motors stop)
                LATC &= ~(1<<MOTOR_A_PWM);
                LATC &= ~(1<<MOTOR_B_PWM);
                LATC &= ~(1<<MOTOR_A_DIR);
                LATC &= ~(1<<MOTOR_B_DIR);
                LATCSET = 1;
                //LATA &= ~(1<<10);
                
                /*digitalWrite(13,LOW);
                digitalWrite(12,LOW);
                digitalWrite(11,LOW);
                digitalWrite(10,LOW);*/
              }
        }/*else{
            LATC &= ~(1<<MOTOR_A_DIR);
            LATC &= ~(1<<MOTOR_B_DIR);
            LATC &= ~(1<<MOTOR_A_PWM);
            LATC &= ~(1<<MOTOR_B_PWM);
        }*/
    }

    return 0;
}

