/*#ifndef CAPTEUR_ULTRASON_H_
#define CAPTEUR_ULTRASON_H_

#include "main.h" // C'est CA qui donne accès à HAL et aux types TIM

// Prototypes
uint32_t HCSR04_Read(void);
void delay_us(uint16_t us);

#endif*/

#ifndef HCSR04_H
#define HCSR04_H

#include "stm32l053xx.h"

typedef struct {
    GPIO_TypeDef *trigPort;
    uint8_t trigPin;
    GPIO_TypeDef *echoPort;
    uint8_t echoPin;
} HCSR04_TypeDef;

void HCSR04_init(HCSR04_TypeDef *sensor,
                 GPIO_TypeDef *trigPort, uint8_t trigPin,
                 GPIO_TypeDef *echoPort, uint8_t echoPin);

uint32_t HCSR04_getDistance(HCSR04_TypeDef *sensor);

#endif
