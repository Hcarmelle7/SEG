#include "adc.h"

// --- 1. La fonction de mapping GPIO -> Canal (Excellente, on la garde !) ---
int adc_channel_from_gpio(GPIO_TypeDef *port, uint8_t pin) {
    if (port == GPIOA) {
        if ((pin >= 0) && (pin < 8)) return (int)pin;
    }
    if (port == GPIOB) {
        if ((pin >= 0) && (pin < 2)) return (int)(pin + 8);
    }
    if (port == GPIOC) {
        if ((pin >= 0) && (pin < 6)) return (int)(pin + 10);
    }
    return -1;
}

// --- 2. Initialisation globale de l'ADC (À appeler UNE SEULE FOIS dans le main) ---
void ADC_Init_Core(uint8_t resolution) {
    // 1. Activer l'horloge
    RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;

    // 2. Horloge de l'ADC (PCLK/4 ou PCLK/2)
    ADC1->CFGR2 |= (0b11 << ADC_CFGR2_CKMODE_Pos);

    // 3. Mode SINGLE (On enlève le mode continu) et Résolution
    ADC1->CFGR1 &= ~(ADC_CFGR1_CONT | ADC_CFGR1_RES);
    if (resolution == 10)      ADC1->CFGR1 |= (0b01 << ADC_CFGR1_RES_Pos);
    else if (resolution == 8)  ADC1->CFGR1 |= (0b10 << ADC_CFGR1_RES_Pos);
    else if (resolution == 6)  ADC1->CFGR1 |= (0b11 << ADC_CFGR1_RES_Pos);

    // 4. Calibration (Optimisation pro)
    ADC1->CR |= ADC_CR_ADCAL;
    while((ADC1->ISR & ADC_ISR_EOCAL) == 0); // Attend la fin de calibration
    ADC1->ISR |= ADC_ISR_EOCAL;              // Nettoie le drapeau

    // 5. Allumage final de l'ADC
    ADC1->ISR |= ADC_ISR_ADRDY; // Clear le flag avant d'allumer
    ADC1->CR |= ADC_CR_ADEN;
    while(!(ADC1->ISR & ADC_ISR_ADRDY)); // Attend qu'il soit prêt
}

// --- 3. Configuration d'une broche (À appeler pour CHAQUE capteur) ---
void analog_pin_config(GPIO_TypeDef *port, uint8_t pin) {
    // 1. Activer l'horloge du port GPIO de manière sécurisée
    if (port == GPIOA) RCC->IOPENR |= RCC_IOPENR_GPIOAEN;
    else if (port == GPIOB) RCC->IOPENR |= RCC_IOPENR_GPIOBEN;
    else if (port == GPIOC) RCC->IOPENR |= RCC_IOPENR_GPIOCEN;

    // 2. Mode Analogique (11)
    port->MODER |= (3 << (pin * 2));

    // (Note : Pas besoin de toucher à l'ADC ici, on sélectionnera le canal
    // uniquement au moment où on voudra faire la lecture)
}

// --- 4. Fonction de lecture ciblée (Le Multiplexage) ---
uint32_t ADC_Read(GPIO_TypeDef *port, uint8_t pin) {
    int channel = adc_channel_from_gpio(port, pin);
    if (channel == -1) return 0;

    // 1. Indique à l'ADC quel canal lire
    ADC1->CHSELR = (1 << channel);

    // 2. Lance UNE SEULE conversion
    ADC1->CR |= ADC_CR_ADSTART;

    // 3. Attend la fin de la conversion (EOC)
    while ((ADC1->ISR & ADC_ISR_EOC) == 0);

    // 4. Renvoie le résultat
    return ADC1->DR;
}
