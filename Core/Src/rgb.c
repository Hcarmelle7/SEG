#include "rgb.h"

void RGB_Init(void) {
    // 1. Activer l'horloge du Port A
    RCC->IOPENR |= RCC_IOPENR_GPIOAEN;

    // 2. Nettoyer les bits du registre MODER pour PA10, PA11 et PA12 (mise à 00)
    // PA10 utilise les bits 20-21, PA11 les bits 22-23, PA12 les bits 24-25
    GPIOA->MODER &= ~((0x3UL << 20) | (0x3UL << 22) | (0x3UL << 24));

    // 3. Configurer en mode Sortie Générale (Output - 01)
    GPIOA->MODER |=  ((0x1UL << 20) | (0x1UL << 22) | (0x1UL << 24));

    // 4. Configurer en mode Push-Pull (Registre OTYPER mis à 0 pour ces broches)
    GPIOA->OTYPER &= ~((0x1UL << 10) | (0x1UL << 11) | (0x1UL << 12));

    // 5. Initialiser la LED éteinte par défaut (Mise à l'état BAS / Reset)
    // Dans le BSRR, les bits 16 à 31 servent à forcer un état bas (Reset)
    GPIOA->BSRR = (1UL << (10 + 16)) | (1UL << (11 + 16)) | (1UL << (12 + 16));
}

void RGB_SetColor(uint8_t r, uint8_t g, uint8_t b) {
    // Étape A : On éteint toutes les couleurs (Mise à l'état bas via les bits 26, 27, 28 du BSRR)
    GPIOA->BSRR = (1UL << 26) | (1UL << 27) | (1UL << 28);

    // Étape B : On prépare le masque pour allumer (Set) les couleurs demandées
    // Dans le BSRR, les bits 0 à 15 servent à forcer un état haut (Set)
    uint32_t set_mask = 0;

    if (r) set_mask |= (1UL << 12); // Allumer PA12 (Rouge)
    if (g) set_mask |= (1UL << 11); // Allumer PA11 (Vert)
    if (b) set_mask |= (1UL << 10); // Allumer PA10 (Bleu)

    // Étape C : On applique le masque (si au moins une couleur doit être allumée)
    if (set_mask != 0) {
        GPIOA->BSRR = set_mask;
    }
}
