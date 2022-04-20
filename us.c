#include <xc.h>
#include <stdint.h>
#include "us.h"
// ------------------------------------------------------
// --------------------- CONSTANTES ---------------------
// ------------------------------------------------------
#define NUM_PINES_US 16 // N�mero de pines US
#define TIEMPO_ESPERA 200 // En m�ltiplos de 50 us

// ------------------------------------------------------
// ------------ VARIABLES GLOBALES AL M�DULO ------------
// ------------------------------------------------------
static unsigned int medida_us[] =
{511, 511, 511, 511, 511, 511, 511, 511,
511, 511, 511, 511, 511, 511, 511, 511};
static unsigned pin_configurado[] =
{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
static unsigned char pines_disparo[NUM_PINES_US];
static unsigned char pin_us = 0;

// -----------------------------------------------------
// ---------------- FUNCIONES P�BLICAS -----------------
// -----------------------------------------------------
void configurarPinesUS(unsigned char pin_disparo,unsigned char pin_eco) 
{
    if (pin_disparo >= NUM_PINES_US || pin_eco >= NUM_PINES_US)
        return;
    
    TRISCSET = 1<<pin_eco; // entrada
    TRISCCLR = (1<<pin_disparo); //salida
    
    ANSELC &= ~((1<<pin_eco) | (1<<pin_disparo)); //como digitales
    
    LATCCLR = 1<<pin_disparo;
    
    /*TRISA &= ~(1 << pin_disparo); // Disparo: Salida
    TRISA |= 1 << pin_eco; // Eco: Entrada
    PORTA &= ~(1 << pin_disparo); // Disparo: Nivel bajo*/
    
    pines_disparo[pin_eco] = pin_disparo;
    pin_configurado[pin_eco] = 1;
    
    
}
// -----------------------------------------------------

void inicializarUS(void) 
{
    // Seleccionar el primer pin de eco configurado
    unsigned char i = 0;
    for (i = 0; i < NUM_PINES_US; i++) {
        if (pin_configurado[i] == 1) {
            pin_us = i;
            break;
        }
    }
    T3CON = 0x0000; // OFF, preescalado 1:1
    PR3 = 249; // Tiempo = 50 us
    TMR3 = 0; // Inicializar la cuenta
    IFS0bits.T3IF = 0; // Borrar la bandera
    IEC0bits.T3IE = 1; // Habilitar interrupciones
    IPC3bits.T3IP = 3; // Prioridad interrupciones
    T3CONbits.ON = 1; // Encender el Timer 3
}
// -----------------------------------------------------

float getMedidaUS(unsigned char pin) {
    //Si a�n no se ha realizado una medida o no se ha detectado un obst�culo, 
    //devuelve 511.
    if (pin >= NUM_PINES_US)
        return 511;
    return medida_us[pin];
}

// -----------------------------------------------------
// ---------------- FUNCIONES PRIVADAS -----------------
// -----------------------------------------------------

__attribute__((vector(12),interrupt(IPL3SOFT),nomips16))
void InterrupcionTimer3(void)
{
    static unsigned char estado = 0;
    static unsigned int contador_espera = 0;
    static unsigned char eco_actual = 0;
    unsigned char eco_anterior;
    static unsigned int medida = 0;
    
    IFS0bits.T3IF = 0; // Borrar el flag
    
    switch (estado) {
    case 0: // Iniciar el disparo
        LATCSET = 1 << pines_disparo[pin_us];
        estado = 1;
        break;
    case 1: // Finalizar el disparo
        LATCCLR = (1 << pines_disparo[pin_us]);
        eco_actual = 0;
        estado = 2;
        break;
    case 2: // Esperar al inicio de la recepci�n
    eco_anterior = eco_actual;
        eco_actual = (PORTC >> pin_us) & 0x1;
        // Flanco de subida
        if (eco_actual > eco_anterior) {
            medida = 0;
            contador_espera = 0;
            estado = 3;
        }
        // Pasar al siguiente sensor si no llega el flanco.
        // Como ya se ha esperado aqu�, no se reinicia el
        // contador para no volver a esperar en el
        // estado 4.
        else if (++contador_espera > TIEMPO_ESPERA) {
            estado = 4;
        }
        break;
    case 3: // Recibir la medida
        eco_anterior = eco_actual;
        eco_actual = (PORTC >> pin_us) & 0x1;
        // Limitar la medida m�xima
        if (++medida > 511)
            medida = 511;
        // Flanco de bajada
        if (eco_actual < eco_anterior) {
            medida_us[pin_us] = medida;
            contador_espera = 0;
            estado = 4;
        }
        else if (++contador_espera > TIEMPO_ESPERA) {
            estado = 4;
        }
        break;
    case 4: // Esperar antes de la siguiente medida
        if (++contador_espera > TIEMPO_ESPERA) {
            // Buscar el siguiente sensor de ultrasonidos
            // �ATENCI�N! Bloquea el programa si no hay
            // ning�n pin US configurado

            do {
                if (++pin_us >= NUM_PINES_US)
                pin_us = 0;
            } while (pin_configurado[pin_us] == 0);

            contador_espera = 0;
            estado = 0;
        }
        break;
    }
}