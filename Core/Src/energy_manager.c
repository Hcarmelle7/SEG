#include "energy_manager.h"
#include "adc.h"
#include "buzzer.h"
#include "sensors.h" // Pour lire la température !
#include "servo.h"
#include "rgb.h"
#include <stdio.h>
#include <stdlib.h>

// Variables internes au module
static char budget_str[5] = "";
static int budget_index = 0;
static int budget_final = 0;
static uint8_t mute = 0;
static uint8_t mode_actuel = 0;
static uint8_t override_delestage = 0;
static uint32_t compteur_affichage_mode = 0;
static uint32_t val_precedente_adc = 0;
static int consommation_affichee = 0;
static int angle_servo_actuel = -1;
#define NOMBRE_DE_MODES 3

void EnergieManager_Loop(void) {

    // On calcule le pourcentage tout de suite pour gérer les sécurités des boutons
    float pourcentage = 0.0f;
    if (budget_final > 0) {
        pourcentage = ((float)consommation_affichee / (float)budget_final) * 100.0f;
    }

    // ====================================================================
    // PARTIE 1 : CLAVIER ET BUDGET
    // ====================================================================
    char key = Keypad_GetKey(&myKeypad);
    if (key != 0) {
        if (key >= '0' && key <= '9' && budget_index < 4) {
            budget_str[budget_index++] = key;
            budget_str[budget_index] = '\0';
            compteur_affichage_mode = 0;
            LCD_SetCursor(&myLcd, 0, 8);
            LCD_Print(&myLcd, budget_str);
        }
        else if (key == 'B') { // Valider le budget
            budget_final = atoi(budget_str);
            Buzzer_On(); LL_mDelay(150); Buzzer_Off();
            compteur_affichage_mode = 0;
            LCD_SetCursor(&myLcd, 0, 0);
            LCD_Print(&myLcd, "OK! Enregistre\x02  ");
            LL_mDelay(500);
            LCD_Clear(&myLcd);
            LCD_SetCursor(&myLcd, 0, 0);
            LCD_Print(&myLcd, "Budget: ");
            LCD_Print(&myLcd, budget_str);
            LCD_Print(&myLcd, "W");
        }
        else if (key == 'A') { // Modifier le budget
            budget_index = 0;
            budget_str[0] = '\0';
            LCD_SetCursor(&myLcd, 0, 8);
            LCD_Print(&myLcd, "    ");
        }
        else if (key == 'C') { //Arreter l'alarme sonore
            mute = 1;
        }
        else if (key == 'D') { // Naviguer entre les modes
            mode_actuel++;
            if (mode_actuel >= 3) mode_actuel = 0;

            compteur_affichage_mode = 3;
            Buzzer_On(); LL_mDelay(50); Buzzer_Off();

            // ACTION MATÉRIELLE UNIQUE AU CHANGEMENT DE MODE
            if (mode_actuel == 1) {
                // On passe en forcé : on ouvre par défaut, mais E et F restent libres !
                if (angle_servo_actuel != 0) { Servo_SetAngle(0); angle_servo_actuel = 0; }
            } else if (mode_actuel == 2) {
                // On passe en veille : on ferme le volet.
                if (angle_servo_actuel != 180) { Servo_SetAngle(180); angle_servo_actuel = 180; }
            }
        }
        else if (key == 'E') { // Fermer manuellement
            if (pourcentage >= 80.0f && mode_actuel != 1) {
                // DÉLESTAGE ACTIF : Action refusée (Bip long d'erreur)
                Buzzer_On(); LL_mDelay(250); Buzzer_Off();
            } else {
                // Action autorisée
                if (angle_servo_actuel != 180) {
                    Servo_SetAngle(180);
                    angle_servo_actuel = 180;
                }
                Buzzer_On(); LL_mDelay(50); Buzzer_Off();
            }
        }
        else if (key == 'F') { // Ouvrir manuellement
            if (pourcentage >= 80.0f && mode_actuel != 1) {
                // DÉLESTAGE ACTIF : Action refusée (Bip long d'erreur)
                Buzzer_On(); LL_mDelay(250); Buzzer_Off();
            } else {
                // Action autorisée
                if (angle_servo_actuel != 0) {
                    Servo_SetAngle(0);
                    angle_servo_actuel = 0;
                }
                Buzzer_On(); LL_mDelay(50); Buzzer_Off();
            }
        }
    }

    // ====================================================================
    // PARTIE 2 : CALCUL DE LA CONSOMMATION AVEC LE POTENTIOMETRE
    // ====================================================================
    uint32_t val_brute = ADC_Read(GPIOA, 0);
    int difference = abs((int)val_brute - (int)val_precedente_adc);

    if (difference > 50) {
        val_precedente_adc = val_brute;
        int conso_calculee = (val_brute * 5000) / 4095;
        consommation_affichee = (conso_calculee / 50) * 50;
        if (consommation_affichee <= 0) consommation_affichee = 1;
    }

    // ====================================================================
    // PARTIE 3 : LECTURE TEMPÉRATURE, SÉCURITÉ ET AFFICHAGE
    // ====================================================================
    int temp_actuelle = (int)Sensors_GetTemperature();

    if (budget_final > 0) {

        // --- EN CAS DE DÉPASSEMENT (> 80%) ---
        if (pourcentage >= 80.0f) {

            if (mute == 0) Buzzer_On(); else Buzzer_Off();
            RGB_SetColor(1, 0, 0); // Led Rouge

            // VERROUILLAGE SÉCURITÉ
            if (mode_actuel == 0 || mode_actuel == 2) {
                // Le mode AUTO et VEILLE forcent la fermeture en permanence pour sécuriser
                if (angle_servo_actuel != 180) {
                    Servo_SetAngle(180);
                    angle_servo_actuel = 180;
                }
            }
            // (Si on est en Mode 1 (FORCÉ), on ne force rien. On laisse l'utilisateur jouer avec E et F)

            if (mode_actuel == 2) {
                RGB_SetColor(0, 0, 0); // Économie maximale en veille
            }

            // Affichage Critique
            char msg_alerte[20];
            sprintf(msg_alerte, "ECO + %4dW \x01%2d\xDF" "C ", consommation_affichee, temp_actuelle);
            LCD_SetCursor(&myLcd, 1, 0);
            LCD_Print(&myLcd, msg_alerte);

        } else {
            // --- RETOUR A LA NORMALE (< 80%) ---
            Buzzer_Off();
            mute = 0;

            if (mode_actuel == 0) {
				uint32_t lumiere = Sensors_GetLightPercent();

				if (lumiere < 20) { // C'est la NUIT
					// On allume les lumières
					//RGB_SetColor(1, 1, 1);
					Led_turnOn(&led_alerte);
					// On ferme les volets
					if (angle_servo_actuel != 0) { Servo_SetAngle(0); angle_servo_actuel = 0; }
				}
				else if (lumiere > 80) { // C'est le JOUR
					// On éteint les lumières
					Led_turnOff(&led_alerte);
					if (pourcentage < 60.0f) RGB_SetColor(0, 0, 1);
					else RGB_SetColor(1, 1, 0); // Ou jaune équilibré

					// On ouvre les volets au lever du soleil
					if (angle_servo_actuel != 180) { Servo_SetAngle(180); angle_servo_actuel = 180; }
				}
			}
			else if (mode_actuel == 2) { // Mode VEILLE
				RGB_SetColor(0, 0, 0);
				if (angle_servo_actuel != 180) { Servo_SetAngle(180); angle_servo_actuel = 180; }
			}
			else {
				 if (pourcentage < 60.0f) RGB_SetColor(0, 0, 1); else RGB_SetColor(1, 1, 0);
			}

            // Affichage Normal
            char msg_normal[20];
            sprintf(msg_normal, "P:%4dW     \x01%2d\xDF" "C   ", consommation_affichee, temp_actuelle);
            LCD_SetCursor(&myLcd, 1, 0);
            LCD_Print(&myLcd, msg_normal);
        }
    }

    // ====================================================================
    // AFFICHAGE TEMPORAIRE LIGNE 0
    // ====================================================================
    if (compteur_affichage_mode > 0) {
        compteur_affichage_mode--;
        LCD_SetCursor(&myLcd, 0, 0);
        if (mode_actuel == 0)      LCD_Print(&myLcd, "MODE: AUTO      ");
        else if (mode_actuel == 1) LCD_Print(&myLcd, "MODE: FORCE   ");
        else if (mode_actuel == 2) LCD_Print(&myLcd, "MODE: VEILLE    ");
    } else {
        LCD_SetCursor(&myLcd, 0, 0);
        LCD_Print(&myLcd, "Budget: ");
        LCD_Print(&myLcd, budget_str);
    }
}
