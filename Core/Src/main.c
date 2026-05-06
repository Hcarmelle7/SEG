/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "tim.h"
#include "gpio.h"
#include "main.h"
#include "led.h"
#include "lcd.h"      // <--- IMPORTANT
#include "keypad.h"   // <--- IMPORTANT
#include "hcsr04.h"
#include <stdio.h>  // Pour sprintf
#include <stdlib.h>
#include "stm32l0xx_ll_utils.h"
#include "servo.h"
#include "buzzer.h"
#include "energy_manager.h" // Module d'énergie
#include "sensors.h"
#include "rgb.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
volatile uint32_t uwTick;
LED_TypeDef led_alerte;
LED_TypeDef led_confort;
LCD_Obj_Typedef myLcd;
Keypad_TypeDef  myKeypad;
HCSR04_TypeDef sensor;



char budget_str[5] = "";
int budget_index = 0;
uint32_t adc_val = 0;
float consommation_watts = 0.0;
int budget_final = 0;
uint32_t last_tick = 0;
uint32_t last_presence_tick = 0;
uint32_t current_dist = 0;
uint32_t val_precedente = 0;
// Tableau représentant les pixels de l'icône
uint8_t icone_thermo[8] = {
  0b00100, //   * (Haut du thermomètre)
  0b01010, //  * * (Vide)
  0b01010, //  * * (Vide)
  0b01110, //  *** (Niveau du liquide)
  0b01110, //  *** (Liquide)
  0b11111, // ***** (Bulbe plein)
  0b11111, // ***** (Bulbe plein)
  0b01110  //  *** (Bas du bulbe)
};

uint8_t icone_sourire[8] = {
  0b01110, //  *** (Haut du cercle)
  0b10001, // * * (Espace)
  0b11011, // ** ** (Bords du cercle + Yeux)
  0b10001, // * * (Joues)
  0b11011, // ** ** (Bords + Coins du sourire)
  0b10101, // * * * (Bord + Bas de la bouche + Bord)
  0b10001, // * * (Espace)
  0b01110  //  *** (Bas du cercle)
};

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */


/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_SYSCFG);
  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_PWR);

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  //My_ADC_Init();
  MX_TIM2_Init();
  /* USER CODE BEGIN 2 */
  // Config Clavier : PC0-PC3 (Lignes), PC4-PC7 (Colonnes)
  	RGB_Init();
  	Led_init(&led_alerte, GPIOA, 8);
  	Led_init(&led_confort, GPIOA, 9);
  	myKeypad.port = GPIOC;
  	myKeypad.startRowPin = 0;
  	myKeypad.startColPin = 4;
  	Keypad_Init(&myKeypad);

  	// Config LCD
  	myLcd.portData = GPIOB;
  	myLcd.startPin = 4;
  	myLcd.portRS = GPIOB; myLcd.pinRS = 0;
  	myLcd.portEN = GPIOB; myLcd.pinEN = 1;
  	LCD_Init(&myLcd);
  	LCD_CreateChar(&myLcd, 1, icone_thermo); // On l'enregistre dans l'emplacement mémoire n°1
  	LCD_CreateChar(&myLcd, 2, icone_sourire);
  	HCSR04_init(&sensor, GPIOA, 4, GPIOA, 5);
  	Servo_Init();
  	Buzzer_Init(GPIOB, 9);
  	LCD_Print(&myLcd, "Budget: ");


  /* USER CODE END 2 */
  	// PA2 correspond au port GPIOA, pin 2.
  	ADC_Init_Core(12);                   // On allume l'ADC en 12 bits
  	analog_pin_config(GPIOA, 1);         // Prépare PA1 pour la Lumière (LDR)
  	analog_pin_config(GPIOA, 2);
  	analog_pin_config(GPIOA, 0);

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */
	EnergieManager_Loop();

	// 2. Gestion des capteurs et actionneurs (Ultrason, Servo, LED)
	  Sensors_Loop();


/*
	  uint32_t lumiere = Sensors_GetLightPercent();

	  if (lumiere < 20) { // Nuit noire (< 20%)
	      RGB_SetColor(1, 1, 1); // Lumière ON
	      Servo_SetAngle(180);   // Volets fermés
	  }
	  else if (lumiere > 80) { // Grand jour (> 80%)
	      RGB_SetColor(0, 0, 0); // Lumière OFF
	      Servo_SetAngle(0);     // Volets ouverts
	  }
*/
	      // 3. Petite pause pour que l'écran ne clignote pas trop vite
	      //LL_mDelay(200);

	    // CAS C : ZONE INTERMÉDIAIRE (Entre 20% et 80%)
	    // S'il y a un nuage passager, le système ne fait rien.
	    // Cela évite l'usure du moteur et le clignotement des lumières.


	LL_mDelay(500);



    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  LL_FLASH_SetLatency(LL_FLASH_LATENCY_0);
  while(LL_FLASH_GetLatency()!= LL_FLASH_LATENCY_0)
  {
  }
  LL_PWR_SetRegulVoltageScaling(LL_PWR_REGU_VOLTAGE_SCALE1);
  while (LL_PWR_IsActiveFlag_VOS() != 0)
  {
  }
  LL_RCC_HSI_Enable();

   /* Wait till HSI is ready */
  while(LL_RCC_HSI_IsReady() != 1)
  {

  }
  LL_RCC_HSI_SetCalibTrimming(16);
  LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
  LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_1);
  LL_RCC_SetAPB2Prescaler(LL_RCC_APB2_DIV_1);
  LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_HSI);

   /* Wait till System clock is ready */
  while(LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_HSI)
  {

  }

  LL_Init1msTick(16000000);

  LL_SetSystemCoreClock(16000000);
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
