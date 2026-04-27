#ifndef CAPTEUR_ULTRASON_H_
#define CAPTEUR_ULTRASON_H_

#include "main.h" // C'est CA qui donne accès à HAL et aux types TIM

// Prototypes
uint32_t HCSR04_Read(void);
void delay_us(uint16_t us);

#endif
