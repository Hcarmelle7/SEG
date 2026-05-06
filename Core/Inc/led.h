#ifndef LED_H
#define LED_H

#include "main.h"

// La structure complète avec ses vraies variables
typedef struct {
    GPIO_TypeDef *gpioPort;
    uint8_t pin;
} LED_TypeDef;

void Led_init(LED_TypeDef *led, GPIO_TypeDef * port, uint8_t pn);
void Led_turnOn(LED_TypeDef *led);
void Led_turnOff(LED_TypeDef *led);
void Led_toggle(LED_TypeDef *led);
uint8_t Led_isOn(LED_TypeDef *led);
uint8_t Led_isOff(LED_TypeDef *led);

#endif /* LED_H */
