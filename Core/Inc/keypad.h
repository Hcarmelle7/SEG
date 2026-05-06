#ifndef KEYPAD_H_
#define KEYPAD_H_

#include "main.h"

typedef struct {
    GPIO_TypeDef *port;     // Port commun pour lignes et colonnes (ex: GPIOC)
    uint8_t startRowPin;    // Première pin des lignes (ex: 0 pour PC0-PC3)
    uint8_t startColPin;    // Première pin des colonnes (ex: 4 pour PC4-PC7)
} Keypad_TypeDef;

void Keypad_Init(Keypad_TypeDef *kp);
char Keypad_GetKey(Keypad_TypeDef *kp);

#endif
