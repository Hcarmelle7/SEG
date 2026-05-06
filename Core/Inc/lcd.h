#ifndef LCD_H_
#define LCD_H_

#include "main.h"

typedef struct {
    GPIO_TypeDef *portData;
    uint8_t startPin;
    GPIO_TypeDef *portRS;
    uint8_t pinRS;
    GPIO_TypeDef *portEN;
    uint8_t pinEN;
} LCD_Obj_Typedef; // On utilise ce nom pour éviter le conflit ST

void LCD_Init(LCD_Obj_Typedef *lcd);
void LCD_Print(LCD_Obj_Typedef *lcd, char *str);
void LCD_Clear(LCD_Obj_Typedef *lcd);
void LCD_SetCursor(LCD_Obj_Typedef *lcd, uint8_t row, uint8_t col);
void LCD_SendCommand(LCD_Obj_Typedef *lcd, uint8_t cmd);
void LCD_Data(LCD_Obj_Typedef *lcd, uint8_t data);
void LCD_CreateChar(LCD_Obj_Typedef *lcd, uint8_t location, uint8_t charmap[]);

#endif // Fin du fichier LCD_H_
