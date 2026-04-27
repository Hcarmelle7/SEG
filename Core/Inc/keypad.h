#ifndef INC_KEYPAD_H_
#define INC_KEYPAD_H_

#include "stm32l0xx_hal.h"

// Lignes (Sorties)
#define ROW_PORT GPIOC
#define R1_PIN GPIO_PIN_0
#define R2_PIN GPIO_PIN_1
#define R3_PIN GPIO_PIN_2
#define R4_PIN GPIO_PIN_3

// Colonnes (Entrées avec Pull-up)
#define COL_PORT GPIOC
#define C1_PIN GPIO_PIN_4
#define C2_PIN GPIO_PIN_5
#define C3_PIN GPIO_PIN_6
#define C4_PIN GPIO_PIN_7

char Keypad_GetKey(void);

#endif
