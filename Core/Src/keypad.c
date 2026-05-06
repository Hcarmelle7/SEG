#include "keypad.h"

void Keypad_Init(Keypad_TypeDef *kp) {
    // 1. Activation Horloge
    uint8_t nb_port = ((uint32_t)kp->port - IOPPERIPH_BASE) / 0x400;
    RCC->IOPENR |= (1 << nb_port);

    // 2. Configuration des LIGNES en SORTIE (Mode 01)
    for(int i=0; i<4; i++) {
        uint8_t pin = kp->startRowPin + i;
        kp->port->MODER &= ~(0b11 << (2 * pin));
        kp->port->MODER |= (0b01 << (2 * pin));
        kp->port->ODR |= (1 << pin); // État repos : Haut
    }

    // 3. Configuration des COLONNES en ENTRÉE (Mode 00) avec PULL-UP (Mode 01)
    for(int i=0; i<4; i++) {
        uint8_t pin = kp->startColPin + i;
        kp->port->MODER &= ~(0b11 << (2 * pin));
        kp->port->PUPDR &= ~(0b11 << (2 * pin));
        kp->port->PUPDR |= (0b01 << (2 * pin)); // Activation Pull-up
    }
}

char Keypad_GetKey(Keypad_TypeDef *kp) {
    char keys[4][4] = {{'1','2','3','F'},
                       {'4','5','6','E'},
                       {'7','8','9','D'},
                       {'A','0','B','C'}};

    for(int i=0; i<4; i++) {
        // 1. Mettre toutes les lignes à 1 (repos)
        for(int j=0; j<4; j++) {
            kp->port->ODR |= (1 << (kp->startRowPin + j));
        }

        // 2. Activer la ligne i (mettre à 0)
        kp->port->ODR &= ~(1 << (kp->startRowPin + i));

        // Petit délai pour la stabilisation électrique
        for(volatile int d=0; d<500; d++);

        // 3. Scanner les colonnes
        for(int j=0; j<4; j++) {
            uint8_t colPin = kp->startColPin + j;

            // Si la colonne est à 0, la touche est pressée
            if (!(kp->port->IDR & (1 << colPin))) {
                // Attente du relâchement (Anti-rebond simple)
                while (!(kp->port->IDR & (1 << colPin)));
                return keys[i][j];
            }
        }
    }
    return 0;
}
