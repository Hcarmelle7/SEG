#include "sensors.h"
#include "servo.h"

static uint32_t last_presence_tick = 0;

//Partie 1 : CAPTEUR DE DISTANCE
void Sensors_Loop(void) {
    uint32_t dist = HCSR04_getDistance(&sensor);

    if (dist > 2 && dist < 10) {
        //Servo_SetAngle(180);
        Led_turnOn(&led_alerte);
        last_presence_tick = uwTick;
    }
    else  {
        //Servo_SetAngle(0);
        Led_turnOff(&led_alerte);
    }




}

float Sensors_GetTemperature(void) {
    static uint32_t moyenne_temp = 0;

    // Lecture de PA1 au lieu de PA0 !
    uint32_t val_brute = ADC_Read(GPIOA, 7);

    if (moyenne_temp == 0) moyenne_temp = val_brute;

    // Lissage pour absorber le bruit électrique du LM35
    moyenne_temp = ((moyenne_temp * 8) + (val_brute * 2)) / 10;

    // Calcul : Temp = (ADC * 3300mV) / (4095 * 10mV) = (ADC * 330) / 4095
    float temperature = (moyenne_temp * 330.0f) / 4095.0f;

    return temperature;
}

// --- 3. LECTURE DE LA LUMINOSITÉ (LDR sur PA2) ---
static float val_filtree = 1000.0f; // On initialise à une valeur moyenne

uint32_t Sensors_GetLightPercent(void) {
    // 1. Lire la valeur brute (moyenne de 16 échantillons pour la stabilité)
    uint32_t somme = 0;
    for(int i = 0; i < 16; i++) {
        somme += ADC_Read(GPIOA, 1);
    }
    uint32_t val_brute = somme / 16;

    // 2. Application du filtre exponentiel (lissage)
    // alpha = 0.1 rend le signal très stable
    float alpha = 0.1f;
    val_filtree = (alpha * (float)val_brute) + ((1.0f - alpha) * val_filtree);

    // 3. Conversion en pourcentage (0 à 100%)
    // Rappel : 100 (jour) -> 100%, 2000 (nuit) -> 0%
    // Formule : ((2000 - val) / (2000 - 100)) * 100
    int32_t min_val = 100;
    int32_t max_val = 2000;

    int32_t range = max_val - min_val;
    int32_t pct = ((max_val - (int32_t)val_filtree) * 100) / range;

    // Sécurité pour rester entre 0 et 100
    if (pct < 0) pct = 0;
    if (pct > 100) pct = 100;

    return (uint32_t)pct;
}
