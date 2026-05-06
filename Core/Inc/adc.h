#ifndef ADC_H_
#define ADC_H_

#include "main.h"

// Prototypes demandés par l'exercice
void ADC_Init_Core(uint8_t resolution);
int adc_channel_from_gpio(GPIO_TypeDef *port, uint8_t pin);
void ADC_Init_Core(uint8_t resolution);
void analog_pin_config(GPIO_TypeDef *port, uint8_t pin);
uint32_t ADC_Read(GPIO_TypeDef *port, uint8_t pin);

#endif
