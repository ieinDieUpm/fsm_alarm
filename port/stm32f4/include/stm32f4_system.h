/**
 * @file stm32f4_system.h
 * @author Josué Pagán (j.pagan@upm.es)
 * @brief Header for stm32f4_system.c file of the STM32F4 Nucleo board.
 * @version 0.1
 * @date 2025-04-01
 *
 */

#ifndef STM32F4_SYSTEM_H_
#define STM32F4_SYSTEM_H_

#include "stm32f4xx.h"
#include "stm32f4xx_hal.h"

/**
 * @brief Initialize a GPIO port and its pins.
 *
 * @param p_port Pointer to the GPIO port to be initialized.
 * @param p_gpio  Pointer to the GPIO initialization structure.
 */
void stm32f4_system_gpio(GPIO_TypeDef *p_port, GPIO_InitTypeDef *p_gpio);

void stm32f4_system_gpio_exti_config(IRQn_Type exti_irqn, uint8_t priority, uint8_t subpriority);

#endif /* STM32F4_SYSTEM_H_ */