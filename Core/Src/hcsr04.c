/*#include "capteur_ultrason.h"
#include "stm32l0xx_ll_utils.h"

// En LL on utilise directement le pointeur vers le périphérique
extern TIM_TypeDef *htim2;
extern volatile uint32_t uwTick;

void delay_us(uint16_t us) {
    LL_TIM_SetCounter(TIM2, 0);
    LL_TIM_EnableCounter(TIM2);
    while (LL_TIM_GetCounter(TIM2) < us);
    LL_TIM_DisableCounter(TIM2);
}

uint32_t HCSR04_Read(void) {
    uint32_t local_time = 0;
    uint32_t timeout;

    // 1. Trigger
    LL_GPIO_SetOutputPin(GPIOA, LL_GPIO_PIN_4);
    delay_us(10);
    LL_GPIO_ResetOutputPin(GPIOA, LL_GPIO_PIN_4);

    // 2. Attendre Echo HIGH avec protection (max 5000 itérations)
    timeout = 5000;
    while (!LL_GPIO_IsInputPinSet(GPIOA, LL_GPIO_PIN_5) && timeout > 0) {
        timeout--;
    }
    if (timeout == 0) return 0; // Sortie immédiate si pas de réponse

    // 3. Mesurer la durée avec protection
    LL_TIM_SetCounter(TIM2, 0);
    LL_TIM_EnableCounter(TIM2);

    timeout = 50000; // Protection pour environ 30ms
    while (LL_GPIO_IsInputPinSet(GPIOA, LL_GPIO_PIN_5) && timeout > 0) {
        timeout--;
        if (LL_TIM_GetCounter(TIM2) > 30000) break;
    }

    local_time = LL_TIM_GetCounter(TIM2);
    LL_TIM_DisableCounter(TIM2);

    return local_time / 58;
}*/

#include "hcsr04.h"
#include "stm32l0xx_ll_utils.h"

void HCSR04_init(HCSR04_TypeDef *sensor,
                 GPIO_TypeDef *trigPort, uint8_t trigPin,
                 GPIO_TypeDef *echoPort, uint8_t echoPin) {

    sensor->trigPort = trigPort;
    sensor->trigPin  = trigPin;
    sensor->echoPort = echoPort;
    sensor->echoPin  = echoPin;

    // Activer horloges GPIOA
    uint8_t nb_port_trig = ((uint32_t)trigPort - IOPPERIPH_BASE) / 0x400;
    uint8_t nb_port_echo = ((uint32_t)echoPort - IOPPERIPH_BASE) / 0x400;
    RCC->IOPENR |= (1 << nb_port_trig);
    RCC->IOPENR |= (1 << nb_port_echo);

    // TRIG en sortie
    trigPort->MODER &= ~(0b11 << (2 * trigPin));
    trigPort->MODER |=  (0b01 << (2 * trigPin));

    // ECHO en entrée
    echoPort->MODER &= ~(0b11 << (2 * echoPin));
}

uint32_t HCSR04_getDistance(HCSR04_TypeDef *sensor) {

    // Envoyer impulsion TRIG de 10µs
    sensor->trigPort->ODR &= ~(1 << sensor->trigPin);
    LL_mDelay(2);
    sensor->trigPort->ODR |= (1 << sensor->trigPin);
    // Attendre 10µs
    uint32_t i;
    for (i = 0; i < 160; i++) __NOP();
    sensor->trigPort->ODR &= ~(1 << sensor->trigPin);

    // Attendre front montant ECHO
    uint32_t timeout = 100000;
    while (!(sensor->echoPort->IDR & (1 << sensor->echoPin))) {
        if (--timeout == 0) return 999;
    }

    // Compter durée ECHO
    uint32_t count = 0;
    while (sensor->echoPort->IDR & (1 << sensor->echoPin)) {
        count++;
        for (i = 0; i < 8; i++) __NOP();
    }

    // Convertir en cm (calibration empirique)
    uint32_t distance = count / 10;
    return distance;
}
