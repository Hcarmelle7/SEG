#include "capteur_ultrason.h"
#include "tim.h"

extern TIM_HandleTypeDef htim2; // Assure-toi que TIM2 est activé dans le .ioc

void delay_us(uint16_t us) {
    __HAL_TIM_SET_COUNTER(&htim2, 0);
    HAL_TIM_Base_Start(&htim2);
    while (__HAL_TIM_GET_COUNTER(&htim2) < us);
    HAL_TIM_Base_Stop(&htim2);
}

uint32_t HCSR04_Read(void) {
    uint32_t local_time = 0;

    // 1. Trigger sur PA4
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);
    delay_us(10);
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);

    // 2. Attendre Echo sur PA5 avec TIMEOUT (Max 2ms)
    uint32_t start_wait = HAL_GetTick();
    while (!(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_5))) {
        if (HAL_GetTick() - start_wait > 2) return 0; // Sécurité : on sort si pas d'écho
    }

    // 3. Mesurer la durée
    __HAL_TIM_SET_COUNTER(&htim2, 0);
    HAL_TIM_Base_Start(&htim2);

    while (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_5)) {
        if (__HAL_TIM_GET_COUNTER(&htim2) > 30000) break; // Sécurité : trop loin
    }

    local_time = __HAL_TIM_GET_COUNTER(&htim2);
    HAL_TIM_Base_Stop(&htim2);

    return local_time / 58;
}
