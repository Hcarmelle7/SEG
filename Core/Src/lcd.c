#include "lcd.h"

// Fonction interne pour envoyer 4 bits
void LCD_Write4Bits(uint8_t val) {
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, (val & 0x01) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, (val & 0x02) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, (val & 0x04) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, (val & 0x08) ? GPIO_PIN_SET : GPIO_PIN_RESET);

    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_SET);
    HAL_Delay(5); // Augmenté pour test hardware
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
    HAL_Delay(5);
}

// Envoi d'une commande (RS = 0)
void LCD_SendCommand(uint8_t cmd) {
    HAL_GPIO_WritePin(LCD_RS_PORT, LCD_RS_PIN, GPIO_PIN_RESET);
    LCD_Write4Bits(cmd >> 4);
    LCD_Write4Bits(cmd & 0x0F);
}

// Envoi d'une donnée/caractère (RS = 1)
void LCD_Data(uint8_t data) {
    HAL_GPIO_WritePin(LCD_RS_PORT, LCD_RS_PIN, GPIO_PIN_SET);
    LCD_Write4Bits(data >> 4);
    LCD_Write4Bits(data & 0x0F);
}

// Initialisation de l'écran (Séquence standard HD44780)
void LCD_Init(void) {
    HAL_Delay(100); // Attendre que la tension du LCD se stabilise

    // Séquence d'initialisation forcée
    LCD_Write4Bits(0x03);
    HAL_Delay(5);
    LCD_Write4Bits(0x03);
    HAL_Delay(1);
    LCD_Write4Bits(0x03);
    HAL_Delay(1);

    LCD_Write4Bits(0x02); // On passe ENFIN en mode 4 bits
    HAL_Delay(1);

    LCD_SendCommand(0x28); // 2 lignes, 5x8 dots
    HAL_Delay(1);
    LCD_SendCommand(0x0C); // Display ON, Cursor OFF
    HAL_Delay(1);
    LCD_SendCommand(0x06); // Entry mode
    HAL_Delay(1);
    LCD_Clear();
}

// Affiche une chaîne de caractères
void LCD_Print(char *str) {
    while (*str) {
        LCD_Data(*str++);
    }
}

// Efface l'écran
void LCD_Clear(void) {
    LCD_SendCommand(0x01);
    HAL_Delay(2);
}

// Positionne le curseur (Ligne 0 ou 1, Colonne 0 à 15)
void LCD_SetCursor(uint8_t row, uint8_t col) {
    uint8_t address = (row == 0) ? (0x80 + col) : (0xC0 + col);
    LCD_SendCommand(address);
}
