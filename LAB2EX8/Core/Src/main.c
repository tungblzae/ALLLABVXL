/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

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

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int hour = 15, minute = 8, second = 50;
int led_buffer[4] = {0, 0, 0, 0}; // Initialize led_buffer

// Define the software timer variables
int timer_counter = 0;
int timer_flag = 0;
int TIMER_CYCLE = 1000; // Timer cycle for 1 second

// Define a second software timer (optional)
int timer2_counter = 0;
int timer2_flag = 0;
int TIMER2_CYCLE = 500; // Timer cycle for 0.5 seconds

void setTimer(int duration) {
    timer_counter = duration / TIMER_CYCLE;
    timer_flag = 0;
}

void setTimer2(int duration) {
    timer2_counter = duration / TIMER2_CYCLE;
    timer2_flag = 0;
}

void timer_run() {
    if (timer_counter > 0) {
        timer_counter--;
        if (timer_counter == 0) {
            timer_flag = 1;
        }
    }
    if (timer2_counter > 0) {
        timer2_counter--;
        if (timer2_counter == 0) {
            timer2_flag = 1;
        }
    }
}

void update7SEG(int index) {
	// Define the GPIO ports and pins for common anode displays (adjust as needed)
	    GPIO_TypeDef* seg_ports[] = {GPIOB, GPIOB, GPIOB, GPIOB, GPIOB, GPIOB, GPIOB};
	    uint16_t seg_pins[] = {GPIO_PIN_0, GPIO_PIN_1, GPIO_PIN_2, GPIO_PIN_3, GPIO_PIN_4, GPIO_PIN_5, GPIO_PIN_6};

	    // Define the LED patterns for each digit (adjust as needed)
	    int led_patterns[10][7] = {
	        {1, 1, 1, 1, 1, 1, 0}, // Digit 0
	        {0, 1, 1, 0, 0, 0, 0}, // Digit 1
	        // Define patterns for digits 2 through 9
	    };

	    // Ensure that the index is within a valid range (0 to 3)
	    if (index < 0 || index > 3) {
	        // Handle an invalid index, e.g., turn off all displays
	        for (int i = 0; i < 7; i++) {
	            HAL_GPIO_WritePin(seg_ports[i], seg_pins[i], GPIO_PIN_RESET);
	        }
	        return;
	    }

	    // Get the LED pattern for the current digit (based on led_buffer values)
	    int digit_value = led_buffer[index];

	    // Loop through the segments and set them based on the LED pattern
	    for (int i = 0; i < 7; i++) {
	        if (led_patterns[digit_value][i] == 1) {
	            // Turn on the segment for the current digit
	            HAL_GPIO_WritePin(seg_ports[i], seg_pins[i], GPIO_PIN_SET);
	        } else {
	            // Turn off the segment for the current digit
	            HAL_GPIO_WritePin(seg_ports[i], seg_pins[i], GPIO_PIN_RESET);
	        }
	    }

	    // Ensure that other displays are turned off
	    for (int i = 0; i < 4; i++) {
	        if (i != index) {
	            for (int j = 0; j < 7; j++) {
	                HAL_GPIO_WritePin(seg_ports[j], seg_pins[j], GPIO_PIN_RESET);
	            }
	        }
	    }
}

int main(void) {
    // STM32 initialization and configuration

    while (1) {
        // Software timer for 1-second intervals
        timer_run();

        // Check if a 1-second interval has passed
        if (timer_flag) {
            // Update the clock values
            second++;

            if (second >= 60) {
                second = 0;
                minute++;
            }
            if (minute >= 60) {
                minute = 0;
                hour++;
            }
            if (hour >= 24) {
                hour = 0;
            }

            // Update the led_buffer based on the current time
            led_buffer[0] = (hour / 10) % 10;
            led_buffer[1] = hour % 10;
            led_buffer[2] = (minute / 10) % 10;
            led_buffer[3] = minute % 10;

            // Reset the timer flag for the next 1-second interval
            timer_flag = 0;
        }

        // Optional: Use a second software timer for additional tasks (e.g., every 0.5 seconds)
        if (timer2_flag) {
            // Perform tasks for the second software timer here

            // Reset the timer2 flag for the next 0.5-second interval
            timer2_flag = 0;
        }

        // Call the update7SEG function to update the seven-segment displays
        for (int i = 0; i < 4; i++) {
            update7SEG(i);
        }


    }
}


/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_0|GPIO_PIN_1, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3
                          |GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7, GPIO_PIN_RESET);

  /*Configure GPIO pins : PC13 PC14 PC15 */
  GPIO_InitStruct.Pin = GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : PD0 PD1 */
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /*Configure GPIO pins : PA5 PA6 PA7 */
  GPIO_InitStruct.Pin = GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : PB0 PB1 PB2 PB3
                           PB4 PB5 PB6 PB7 */
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3
                          |GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure peripheral I/O remapping */
  __HAL_AFIO_REMAP_PD01_ENABLE();

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

#ifdef  USE_FULL_ASSERT
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

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
