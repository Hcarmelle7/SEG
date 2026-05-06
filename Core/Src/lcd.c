#include "lcd.h"

static void LCD_EnableClock(GPIO_TypeDef *port) {
    uint8_t nb_port = ((uint32_t)port - IOPPERIPH_BASE) / 0x400;
    RCC->IOPENR |= (1 << nb_port);
}

static void LCD_ConfigOut(GPIO_TypeDef *port, uint8_t pin) {
    port->MODER &= ~(0b11 << (2 * pin));
    port->MODER |= (0b01 << (2 * pin));
}

static void LCD_Write4Bits(LCD_Obj_Typedef *lcd, uint8_t val) {
    lcd->portData->ODR &= ~(0xF << lcd->startPin);
    lcd->portData->ODR |= ((val & 0x0F) << lcd->startPin);

    lcd->portEN->ODR |= (1 << lcd->pinEN);
    for(volatile int i=0; i<8000; i++);
    lcd->portEN->ODR &= ~(1 << lcd->pinEN);
    for(volatile int i=0; i<8000; i++);
}

void LCD_SendCommand(LCD_Obj_Typedef *lcd, uint8_t cmd) {
    lcd->portRS->ODR &= ~(1 << lcd->pinRS);
    LCD_Write4Bits(lcd, cmd >> 4);
    LCD_Write4Bits(lcd, cmd & 0x0F);
}

void LCD_Data(LCD_Obj_Typedef *lcd, uint8_t data) {
    lcd->portRS->ODR |= (1 << lcd->pinRS);
    LCD_Write4Bits(lcd, data >> 4);
    LCD_Write4Bits(lcd, data & 0x0F);
}

void LCD_Init(LCD_Obj_Typedef *lcd) {
	for(volatile int i=0; i<500000; i++);
    LCD_EnableClock(lcd->portData);
    LCD_EnableClock(lcd->portRS);
    LCD_EnableClock(lcd->portEN);
    for(int i=0; i<4; i++) LCD_ConfigOut(lcd->portData, lcd->startPin + i);
    LCD_ConfigOut(lcd->portRS, lcd->pinRS);
    LCD_ConfigOut(lcd->portEN, lcd->pinEN);

    //for(volatile int i=0; i<50000; i++);
    LCD_Write4Bits(lcd, 0x03);
        for(volatile int i=0; i<50000; i++); // > 4.1ms

        LCD_Write4Bits(lcd, 0x03);
        for(volatile int i=0; i<10000; i++); // > 100us

        LCD_Write4Bits(lcd, 0x03);
        for(volatile int i=0; i<10000; i++);

        // On passe enfin en mode 4 bits
        LCD_Write4Bits(lcd, 0x02);
        for(volatile int i=0; i<10000; i++);

    LCD_SendCommand(lcd, 0x28);
    LCD_SendCommand(lcd, 0x0C);
    LCD_Clear(lcd);
}

void LCD_Print(LCD_Obj_Typedef *lcd, char *str) {
    while (*str) LCD_Data(lcd, *str++);
}

void LCD_Clear(LCD_Obj_Typedef *lcd) {
    LCD_SendCommand(lcd, 0x01);
    for(volatile int i=0; i<40000; i++);
}

void LCD_SetCursor(LCD_Obj_Typedef *lcd, uint8_t row, uint8_t col) {
    uint8_t address = (row == 0) ? (0x80 + col) : (0xC0 + col);
    LCD_SendCommand(lcd, address);
}

void LCD_CreateChar(LCD_Obj_Typedef *lcd, uint8_t location, uint8_t charmap[]) {
    // Les écrans LCD ne peuvent stocker que 8 caractères personnalisés (de 0 à 7)
    location &= 0x07;

    // 1. Commande pour pointer vers la mémoire de dessin (CGRAM)
    LCD_SendCommand(lcd, 0x40 | (location << 3));

    // 2. On envoie les 8 lignes de pixels de notre icône
    for (int i = 0; i < 8; i++) {
        LCD_Data(lcd, charmap[i]);
    }

    // 3. IMPORTANT : On force l'écran à revenir en mode texte normal
    LCD_SendCommand(lcd, 0x80);
}
