#include "buzzer.h"

// Variables "Mémoire" : le fichier retient le port et la pin configurés
static GPIO_TypeDef *saved_port;
static uint32_t      saved_pin;

void Buzzer_Init(GPIO_TypeDef *port, uint32_t pin) {
    // 1. Sauvegarde dans les tiroirs pour les fonctions On/Off
    saved_port = port;
    saved_pin  = pin;

    // 2. Activer l'horloge (RCC) du port choisi
    if (port == GPIOA)      RCC->IOPENR |= RCC_IOPENR_GPIOAEN;
    else if (port == GPIOB) RCC->IOPENR |= RCC_IOPENR_GPIOBEN;
    else if (port == GPIOC) RCC->IOPENR |= RCC_IOPENR_GPIOCEN;

    // 3. Configuration du registre MODER (Mode Sortie = 01)
    // Étape A : On nettoie (met à 00) les 2 bits correspondants à notre pin
    port->MODER &= ~(0x3UL << (pin * 2));
    // Étape B : On écrit 01 à cet emplacement
    port->MODER |=  (0x1UL << (pin * 2));

    // 4. Configuration du registre OTYPER (Mode Push-Pull = 0)
    // On nettoie le bit (mise à 0) pour s'assurer qu'il est en Push-Pull
    port->OTYPER &= ~(0x1UL << pin);

    // 5. On éteint le buzzer par défaut
    Buzzer_Off();
}

void Buzzer_On(void) {
    // Le registre BSRR utilise les bits 0 à 15 pour forcer la sortie à 1 (Set)
    saved_port->BSRR = (1UL << saved_pin);
}

void Buzzer_Off(void) {
    // Le registre BSRR utilise les bits 16 à 31 pour forcer la sortie à 0 (Reset)
    saved_port->BSRR = (1UL << (saved_pin + 16));
}

void Buzzer_Toggle(void) {
    // Le registre ODR contient l'état actuel, le XOR (^) inverse le bit
    saved_port->ODR ^= (1UL << saved_pin);
}
