
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <xc.h>
#include <math.h>
#include "Pic32Ini.h"

# define MOTOR_B_DIR 6 // Pines del shield PWM (son todos del puerto C)
# define MOTOR_B_PWM 7
# define MOTOR_A_DIR 9
# define MOTOR_A_PWM 8

int conexion, inter = 0;


__attribute__((vector(8),interrupt(IPL2SOFT),nomips16))
void InterrupcionTimer2(void)
{
    IFS0bits.T2IF = 0;
    if (conexion == 0){ // Si se desconecta que se pare
        LATC &= ~(1<<MOTOR_A_PWM);
        LATC &= ~(1<<MOTOR_B_PWM);
        LATC &= ~(1<<MOTOR_A_DIR);
        LATC &= ~(1<<MOTOR_B_DIR);
    }
    
    if (inter == 1){
        LATBINV = (1<<15);
        LATBINV = (1<<8);
    }
    
    
}


void inicializarCoche(){
    ANSELB = 0;
    TRISB = 0;
    ANSELC = 0;
    TRISC = 0xF000;
    LATC = 0XF00F;
    LATB=0x0000;
    //LATA = 0xFFFF;
    //PORTB = 0xFFFF;
    
    T2CON = 0x0060; // OFF, preescalado 1:64
    PR2 = 39062; // Tiempo = 50 us
    TMR2 = 0; // Inicializar la cuenta
    IFS0bits.T2IF = 0; // Borrar la bandera
    IEC0bits.T2IE = 1; // Habilitar interrupciones
    IPC2bits.T2IP = 2; // Prioridad interrupciones
    T2CONbits.ON = 1; // Encender el Timer 3
}

void modificarCoche(char c, float distancia){
    if (c!='\0'){
        //TMR2 = 0
        conexion = 1;
        if(c == 'F'){  //move forward(all motors rotate in forward direction)
            if (distancia > 25.){
                LATC |= (1<<MOTOR_A_PWM);
                LATC |= (1<<MOTOR_B_PWM);
            }else{
                LATC &= ~(1<<MOTOR_A_PWM);
                LATC &= ~(1<<MOTOR_B_PWM);
                LATC &= ~(1<<MOTOR_A_DIR);
                LATC &= ~(1<<MOTOR_B_DIR);
            }
          }
        else if(c == 'B'){      //move reverse (all motors rotate in reverse direction)
            LATC |= (1<<MOTOR_A_PWM);
            LATC |= (1<<MOTOR_B_PWM);
            LATC |= (1<<MOTOR_A_DIR);
            LATC |= (1<<MOTOR_B_DIR);
          }

        else if(c == 'L'){      //turn right (left side motors rotate in forward direction, right side motors doesn't rotate)
            LATC |= (1<<MOTOR_A_PWM);
            LATC |= (1<<MOTOR_B_PWM);
            LATC |= (1<<MOTOR_B_DIR);
          }

        else if(c == 'R'){      //turn left (right side motors rotate in forward direction, left side motors doesn't rotate)
            LATC |= (1<<MOTOR_A_PWM);
            LATC |= (1<<MOTOR_B_PWM);
            LATC |= (1<<MOTOR_A_DIR);
          }

        else if(c == 'W'){    //turn led on or off)
            LATBSET = 1<<15;
          }
        else if(c == 'w'){
            LATBCLR = 1<<15;
          }
        else if(c == 'U'){ //turn led on or off)
            LATBSET = 1<<8;
          }
        else if(c == 'u'){
            LATBCLR = 1<<8;
          }
        else if(c == 'V'){ //turn buzzer on or off)
            LATBSET = 1<<14;
          }
        else if(c == 'v'){
            LATBCLR = 1<<14;
          }
        else if(c == 'X'){ //Luces de emergencia (intermitentes)
            LATBCLR = 1<<15;
            LATBCLR = 1<<8;
            inter = 1;
          }
        else if(c == 'x'){
            LATBCLR = 1<<15;
            LATBCLR = 1<<8;
            inter = 0;
          }
        

        else if(c == 'S'){      //STOP (all motors stop)
            LATC &= ~(1<<MOTOR_A_PWM);
            LATC &= ~(1<<MOTOR_B_PWM);
            LATC &= ~(1<<MOTOR_A_DIR);
            LATC &= ~(1<<MOTOR_B_DIR);
          }
    }else{
        conexion = 0;
    }
}
