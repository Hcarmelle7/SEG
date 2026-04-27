#include "keypad.h"

char Keypad_GetKey(void) {
    char keys[4][4] = {{'1','2','3','F'},
                       {'4','5','6','E'},
                       {'7','8','9','D'},
                       {'A','0','B','C'}};

    for(int i=0; i<4; i++) {
        // 1. On met toutes les lignes à l'état HAUT (Repos)
        HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, GPIO_PIN_SET);

        // 2. On active seulement la ligne actuelle à l'état BAS (Actif)
        uint16_t row_pins[] = {GPIO_PIN_0, GPIO_PIN_1, GPIO_PIN_2, GPIO_PIN_3};
        HAL_GPIO_WritePin(GPIOC, row_pins[i], GPIO_PIN_RESET);

        // Petit délai pour laisser les capacités parasites se décharger
        HAL_Delay(5);

        // 3. On vérifie chaque colonne
        if (HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_4) == GPIO_PIN_RESET) { while(HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_4) == GPIO_PIN_RESET); return keys[i][0]; }
        if (HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_5) == GPIO_PIN_RESET) { while(HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_5) == GPIO_PIN_RESET); return keys[i][1]; }
        if (HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_6) == GPIO_PIN_RESET) { while(HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_6) == GPIO_PIN_RESET); return keys[i][2]; }
        if (HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_7) == GPIO_PIN_RESET) { while(HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_7) == GPIO_PIN_RESET); return keys[i][3]; }
    }
    return 0;
}
