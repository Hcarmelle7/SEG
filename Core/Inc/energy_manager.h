#ifndef ENERGY_MANAGER_H
#define ENERGY_MANAGER_H

#include "main.h"
#include "lcd.h"
#include "keypad.h"
#include "hcsr04.h"
#include "led.h"

// On déclare que ces variables existent dans le main.c pour pouvoir les utiliser ici
extern volatile uint32_t uwTick;
extern LED_TypeDef led_alerte;
extern LCD_Obj_Typedef myLcd;
extern Keypad_TypeDef myKeypad;
extern HCSR04_TypeDef sensor;

// La fonction qui contiendra toute ta logique
void EnergieManager_Loop(void);

#endif /* ENERGIE_MANAGER_H */
