#ifndef SENSORS_H
#define SENSORS_H

#include "main.h"
#include "hcsr04.h"
#include "led.h"

// Variables externes depuis le main
extern volatile uint32_t uwTick;
extern HCSR04_TypeDef sensor;
extern LED_TypeDef led_alerte;
extern LED_TypeDef led_confort;

// La fonction principale des capteurs
void Sensors_Loop(void);
float Sensors_GetTemperature(void);
uint32_t Sensors_GetLightPercent(void);

#endif /* SENSORS_H */
