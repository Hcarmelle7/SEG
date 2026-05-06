#ifndef BUZZER_H
#define BUZZER_H

#include "main.h" // Nécessaire pour inclure les définitions des registres (stm32l0xx.h)

void Buzzer_Init(GPIO_TypeDef *port, uint32_t pin);
void Buzzer_On(void);
void Buzzer_Off(void);
void Buzzer_Toggle(void);

#endif
