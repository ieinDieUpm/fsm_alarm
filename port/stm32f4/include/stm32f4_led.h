/**
 * @file stm32f4_led.h
 * @author Josué Pagán (j.pagan@upm.es)
 * @brief Header file for the LED port layer connected to the STM32F4 Nucleo board.
 * @version 0.1
 * @date 2025-04-01
 *
 */

#ifndef STM32F4_LED_H_
#define STM32F4_LED_H_

/* Includes ------------------------------------------------------------------*/
/* HW dependent includes */
#include "stm32f4xx.h"
#include "stm32f4xx_hal.h"

/* Defines and macros --------------------------------------------------------*/
// HW Nucleo-STM32F446RE:
#define STM32F4_LED_HOME_ALARM_GPIO_PORT GPIOA     /*!< GPIO port of the LED2 in the Nucleo board */
#define STM32F4_LED_HOME_ALARM_GPIO_PIN GPIO_PIN_5 /*!< GPIO pin of the LED2 in the Nucleo board */
#define STM32F4_LED_HOME_ALARM_TIMER TIM2          /*!< Timer to control the blinking of the LED2 in the Nucleo board */
#define STM32F4_LED_HOME_ALARM_TIMER_IRQ TIM2_IRQn /*!< Timer IRQn of the LED2 in the Nucleo board */
#define STM32F4_LED_HOME_ALARM_TIMER_IRQ_PRIO 2    /*!< IRQ priority of the LED2 in the Nucleo board */
#define STM32F4_LED_HOME_ALARM_TIMER_IRQ_SUBPRIO 0 /*!< IRQ subpriority of the LED2 in the Nucleo board */

#endif /* STM32F4_LED_H */
