#ifndef INC_LCD_H_
#define INC_LCD_H_

#include "stm32l0xx_hal.h"

// Définitions essentielles pour le compilateur
#define LCD_RS_PORT  GPIOB
#define LCD_RS_PIN   GPIO_PIN_0
#define LCD_EN_PORT  GPIOB
#define LCD_EN_PIN   GPIO_PIN_1
#define LCD_DATA_PORT GPIOB

// Prototypes des fonctions
void LCD_Init(void);
void LCD_SendCommand(uint8_t cmd);
void LCD_Data(uint8_t data);
void LCD_Print(char *str);
void LCD_Write4Bits(uint8_t val);
void LCD_SetCursor(uint8_t row, uint8_t col);
void LCD_Clear(void);

#endif
