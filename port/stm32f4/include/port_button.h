/**
 * @file port_button.h
 * @author Josué Pagán (j.pagan@upm.es)
 * @brief Header file for the button port layer.
 * @version 0.1
 * @date 2024-04-01
 *
 */

#ifndef PORT_BUTTON_H
#define PORT_BUTTON_H

#include <stdbool.h>

// HW Nucleo-STM32F446RE:
#define BUTTON_PIN 13          /*!< GPIO pin of the button in the Nucleo board */
#define BUTTON_GPIO_PORT GPIOC /*!< GPIO port of the button in the Nucleo board */

#define MODER_BUTTON_MASK (GPIO_MODER_MODE0 << BUTTON_PIN * 2) /*!< Mask for the button in the MODE Register */
#define PUPDR_BUTTON_MASK (GPIO_PUPDR_PUPD0 << BUTTON_PIN * 2) /*!< Mask for the button in the PUPD Register */

#define MODER_BUTTON_AS_INPUT (GPIO_MODE_IN << BUTTON_PIN * 2)       /*!< Output mode for the button in the MODE Register */
#define PUPDR_BUTTON_AS_NOPUPD (GPIO_PUPDR_NOPULL << BUTTON_PIN * 2) /*!< No push/pull configuration for the button in the MODE Register */

#define IDR_BUTTON_MASK (GPIO_IDR_ID0 << BUTTON_PIN) /*!< Mask for the button in the Input Data Register */

/**
 * @brief Initializes the GPIO of the button.
 *
 */
void port_button_gpio_setup(void);

/**
 * @brief Gets the status of the button.
 *
 * @return true if the button is pressed, false otherwise.
 */
bool port_button_get_status(void);

/**
 * @brief Configures the EXTI for the button.
 *
 */
void port_button_exti_config(void);

#endif /* PORT_BUTTON_H */