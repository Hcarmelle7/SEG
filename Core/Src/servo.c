#include "servo.h"

void Servo_Init(void) {
    // 1. Activer l'horloge de GPIOA et du Timer 22 (Attention, TIM22 est sur le bus APB2 !)
    RCC->IOPENR |= RCC_IOPENR_GPIOAEN;
    RCC->APB2ENR |= RCC_APB2ENR_TIM22EN;

    // 2. Configurer PA6 en mode Alternate Function
    GPIOA->MODER &= ~(GPIO_MODER_MODE6_Msk);
    GPIOA->MODER |=  (0x02 << GPIO_MODER_MODE6_Pos);

    // 3. Connecter PA6 au Timer 22 (C'est l'Alternate Function 5 sur le STM32L053)
    GPIOA->AFR[0] &= ~(GPIO_AFRL_AFSEL6_Msk);
    GPIOA->AFR[0] |=  (0x05 << GPIO_AFRL_AFSEL6_Pos); // AF5 = 0x05 = TIM22_CH1

    // 4. Configurer le Timer 22 pour 50Hz (Fréquence de l'horloge = 16MHz)
    TIM22->PSC = 15;          // 1 tick = 1 microseconde
    TIM22->ARR = 19999;       // Période de 20ms (20000 µs)

    // 5. Configurer le Canal 1 en mode PWM
    TIM22->CCMR1 &= ~TIM_CCMR1_CC1S_Msk;
    TIM22->CCMR1 |= (0x06 << TIM_CCMR1_OC1M_Pos) | TIM_CCMR1_OC1PE;

    // 6. Activer la sortie du Canal 1 et démarrer le Timer
    TIM22->CCER |= TIM_CCER_CC1E;
    TIM22->CR1 |= TIM_CR1_CEN;
}

void Servo_SetAngle(uint8_t angle) {
	// Sécurité : on empêche de demander un angle impossible
	    if (angle > 180) angle = 180;

	    // Nouvelle plage physique :
	    // 0°   -> 500 µs
	    // 180° -> 2500 µs
	    // L'écart est de 2000 µs (2500 - 500). Donc pour chaque degré : 2000 / 180

	    uint32_t val = 500 + (angle * 2000 / 180);

	    // On écrit dans le registre de comparaison du Timer 22
	    TIM22->CCR1 = val;
}
