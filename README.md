# 🏡 Smart Energy Guardian (SEG)

![Badge STM32](https://img.shields.io/badge/STM32-L053R8-blue) ![Badge C](https://img.shields.io/badge/Language-C-orange) ![Badge API](https://img.shields.io/badge/API-Low--Layer%20(LL)-brightgreen) ![Badge BareMetal](https://img.shields.io/badge/Architecture-Bare--Metal-red)

## 📝 Présentation du Projet
Le **Smart Energy Guardian** est un système domotique embarqué intelligent développé sur un microcontrôleur **STM32L053R8**. 
Dans un contexte de transition énergétique, ce projet vise à rendre une habitation capable d'adapter sa consommation électrique en temps réel. Il garantit le confort des occupants tout en prévenant les surcharges du réseau via un système matériel et logiciel de **délestage (Interlock)**.

## ✨ Fonctionnalités Principales

### 1. Gestion Budgétaire et Suivi en Temps Réel
L'utilisateur configure un **budget maximum** (en Watts) via le clavier matriciel. Le STM32 calcule dynamiquement le pourcentage de charge du réseau simulé (potentiomètre) :
* 🔵 **< 60% (Confort) :** La LED RGB est bleue. L'énergie est abondante.
* 🟣 **60% - 79% (Équilibré) :** La LED passe au violet. Alerte préventive.
* **Automatisme (Lumière) :** En l'absence de crise énergétique, le système ouvre les volets au lever du soleil (>80% luminosité) et les ferme à la nuit tombée (<20% luminosité) tout en gérant l'éclairage intérieur.

### 2. Procédure de Délestage Critique (>= 80%)
Véritable cœur sécuritaire du système. Si la consommation atteint la zone critique :
* **Déclenchement d'alerte :** LED RGB Rouge et Buzzer intermittent (cadencé logiciellement pour ne pas bloquer l'IHM).
* **Interlock Matériel :** Le système de délestage ferme d'urgence les volets (180°) pour conserver la chaleur et inhibe totalement le capteur de luminosité.
* **Verrouillage IHM :** Les touches manuelles d'ouverture des volets sont désactivées logiciellement pour empêcher les actions contre-productives de l'utilisateur.

### 3. Machine d'États (Navigation Touche 'D')
L'utilisateur peut outrepasser les règles via une touche dédiée, affichant un pop-up asynchrone sur l'écran LCD :
* `MODE 0 (AUTO)` : Gestion intelligente standard.
* `MODE 1 (FORCÉ)` : Dérogation. L'utilisateur casse le verrouillage du délestage et force l'ouverture des volets. La LED passe en violet pour signaler ce forçage.
* `MODE 2 (VEILLE)` : Isolation totale. Fermeture des volets, extinction complète de la signalisation lumineuse (LED RGB) pour soulager le réseau électrique.

## 🔌 Schéma Matériel et Câblage (Pinout)
Le système repose sur la carte Nucleo-L053R8. Voici le routage des composants principaux *(les broches exactes sont définies dans CubeMX / `main.h`)* :

| Composant | Rôle | Protocole / Interface |
| :--- | :--- | :--- |
| **Potentiomètre** | Simulation de la consommation globale de la maison. | ADC (Analog to Digital) |
| **Capteur Temp.** | Relevé de la température intérieure. | ADC / I2C |
| **LDR (Luminosité)**| Détection Jour/Nuit (Seuils : 20% / 80%). | ADC |
| **Servomoteur** | Simulation des volets roulants (Ouverture 0° / Fermeture 180°). | PWM (TIM22_CH1, 50Hz) |
| **Afficheur LCD** | Interface de communication IHM (16x2). | I2C |
| **Clavier 4x4** | Saisie des budgets et commandes manuelles. | GPIOs Numériques |
| **LED RGB** | Indicateur visuel du pourcentage de consommation. | GPIOs Numériques |
| **Buzzer Actif** | Alarme sonore de dépassement énergétique. | GPIO Numérique |
| **HC-SR04** | Détection d'approche/présence. | Timer (Impulsions temporelles)|

## 🕹️ Mapping du Clavier Matriciel
| Touche | Action assignée |
| :---: | :--- |
| **0 - 9** | Saisie des chiffres du budget énergétique. |
| **A** | Effacer la saisie en cours. |
| **B** | Valider et enregistrer le budget. |
| **C** | *Mute* : Couper ou réactiver l'alarme sonore du buzzer. |
| **D** | Naviguer cycliquement entre les modes (AUTO -> FORCÉ -> VEILLE). |
| **E** | Fermer les volets manuellement (Refusé si délestage actif). |
| **F** | Ouvrir les volets manuellement (Refusé si délestage actif). |

## 💻 Architecture Logicielle
Ce projet a été codé en **C** (Bare-Metal / HAL) via STM32CubeIDE. L'architecture garantit la réactivité temps réel :
* **Approche Non-Bloquante :** Remplacement des fonctions de délai classiques (`HAL_Delay`) par des compteurs d'exécution asynchrones pour la gestion des pop-ups LCD et le rythme du buzzer. Le clavier est scanné en permanence.
* **Séparation des Préoccupations :** La logique de calcul énergétique est strictement séparée de la logique matérielle d'actionnement (Servo/LEDs) pour éviter les conflits d'instructions.
* **Protection Matérielle :** Mémorisation logicielle de l'angle physique du servomoteur (`angle_servo_actuel`). Le registre PWM n'est modifié que si la consigne change physiquement, évitant de surcharger le bus APB2 et d'endommager le moteur.

## 🗂️ Arborescence du Projet
L'architecture logicielle est modulaire, séparant clairement les drivers matériels de la logique applicative (Energy Manager).

```text
📁 Core
├── 📁 Inc/ (Fichiers d'en-tête / Interfaces)
│   ├── adc.h, button.h, buzzer.h, gpio.h, hcsr04.h, keypad.h, lcd.h, led.h, rgb.h, sensors.h, servo.h, tim.h
│   ├── energy_manager.h    # Logique métier principale et machine d'états
│   ├── main.h              # Définitions globales et macros
│   └── stm32l0xx_it.h      # Routines d'interruptions (ISR)
│
└── 📁 Src/ (Code Source / Implémentations)
    ├── adc.c, button.c, buzzer.c, gpio.c, hcsr04.c, keypad.c, lcd.c, led.c, rgb.c, servo.c, tim.c
    ├── energy_manager.c    # Cœur du système (Traitement des données et réactions)
    ├── sensors.c           # Traitement des signaux LDR et Thermistance
    ├── main.c              # Point d'entrée et boucle infinie non-bloquante
    └── stm32l0xx_it.c      # Gestionnaires d'interruptions matérielles