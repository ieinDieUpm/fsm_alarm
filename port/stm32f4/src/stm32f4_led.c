/**
 * @file stm32f4_led.c
 * @author Josué Pagán (j.pagan@upm.es)
 * @brief Port layer for LEDs connected to the STM32F4 Nucleo board.
 * @version 0.1
 * @date 2025-04-01
 *
 */

/* Standard C includes */
#include <stdbool.h>
#include <math.h>

/* HW dependent includes */
#include "port_system.h"
#include "port_led.h"

/* Microcontroller dependent includes */
#include "stm32f4_led.h"

/* Global variables -----------------------------------------------------------*/
TIM_HandleTypeDef handler_tim_led_home_alarm; /*!< Timer handler for the LED home alarm */

/* Function definitions ------------------------------------------------------*/
void port_led_timer_setup(uint32_t led_id)
{
    if (led_id == PORT_LED_HOME_ALARM_ID)
    {

        // Enable TIM2 clock using HAL macro
        __HAL_RCC_TIM2_CLK_ENABLE();

        // Set up the timer handle structure
        handler_tim_led_home_alarm.Instance = STM32F4_LED_HOME_ALARM_TIMER;

        // Set the timer parameters
        // Set the prescaler and auto-reload register
        handler_tim_led_home_alarm.Init.Prescaler = 0;
        handler_tim_led_home_alarm.Init.Period = 0;

        // Enable the auto-reload preload
        handler_tim_led_home_alarm.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;

        // Initialize the timer
        HAL_TIM_Base_Init(&handler_tim_led_home_alarm);

        // Enable update interrupt using HAL macro
        HAL_TIM_Base_Start_IT(&handler_tim_led_home_alarm);

        // Set priority and enable IRQ in NVIC
        HAL_NVIC_SetPriority(STM32F4_LED_HOME_ALARM_TIMER_IRQ, STM32F4_LED_HOME_ALARM_TIMER_IRQ_PRIO, STM32F4_LED_HOME_ALARM_TIMER_IRQ_SUBPRIO);
        HAL_NVIC_EnableIRQ(STM32F4_LED_HOME_ALARM_TIMER_IRQ);
    }
}

void port_led_timer_delay_ms(uint32_t led_id, uint32_t delay_ms)
{
    // Compute ARR and PSC to match the duration in milliseconds. Check if the duration is too long and adapt prescaler and ARR
    double ms = (double)delay_ms;
    double scc = (double)SystemCoreClock;
    double psc = round((((scc / 1000.0) * ms) / (65535.0 + 1.0)) - 1.0);
    double arr = round((((scc / 1000.0) * ms) / (psc + 1.0)) - 1.0);

    // Adjust psc and arr if necessary
    while (arr > 0xFFFF)
    {
        psc += 1.0;
        arr = round(((scc / 1000.0) * ms) / (psc + 1.0) - 1.0);
    }

    if (led_id == PORT_LED_HOME_ALARM_ID)
    {
        // Stop timer with interrupt enabled
        HAL_TIM_Base_Stop_IT(&handler_tim_led_home_alarm);

        // Reset counter value
        __HAL_TIM_SET_COUNTER(&handler_tim_led_home_alarm, 0);

        // Update timer initialization structure with computed PSC and ARR values
        handler_tim_led_home_alarm.Init.Prescaler = (uint32_t)(psc);
        handler_tim_led_home_alarm.Init.Period = (uint32_t)(arr);

        // Initialize the timer with the new PSC and ARR values
        HAL_TIM_Base_Init(&handler_tim_led_home_alarm);

        // The PSC and ARR values are currently in the preload registers. To load them into the active registers we need an update event. We can do this manually as follows (or we could wait for the timer to expire).
        // Generate an update event to load the new PSC and ARR values immediately
        HAL_TIM_GenerateEvent(&handler_tim_led_home_alarm, TIM_EVENTSOURCE_UPDATE); // 6) Update generation: Re-inicializa el contador y actualiza los registros. IMPORTANTE que esté lo último

        // Enable the timer
        HAL_TIM_Base_Start_IT(&handler_tim_led_home_alarm);
    }
}

void port_led_gpio_setup(uint32_t led_id)
{
    // Create a generic LED structure to configure the GPIO
    GPIO_InitTypeDef pir_sensor = {0}; // Initialize to 0 all fields by default

    if (led_id == PORT_LED_HOME_ALARM_ID)
    {
        /* GPIO Ports Clock Enable */
        __HAL_RCC_GPIOA_CLK_ENABLE();

        /* Configure GPIO pin */
        pir_sensor.Pin = STM32F4_LED_HOME_ALARM_GPIO_PIN;
        pir_sensor.Mode = MODE_OUTPUT; // A LED is an output device
        pir_sensor.Pull = GPIO_NOPULL; // No pull-up or pull-down internal resistors

        /* Initialize GPIO pin */
        HAL_GPIO_Init(STM32F4_LED_HOME_ALARM_GPIO_PORT, &pir_sensor);

        /* Set GPIO pin output level to low */
        HAL_GPIO_WritePin(STM32F4_LED_HOME_ALARM_GPIO_PORT, STM32F4_LED_HOME_ALARM_GPIO_PIN, GPIO_PIN_RESET);

        // Enable the LED timer
        port_led_timer_setup(led_id);

        // Configure LED timer delay
        port_led_timer_delay_ms(led_id, PORT_LED_HOME_ALARM_BLINK_PERIOD_MS);

        // Ensure that the LED is off at the beginning
        HAL_GPIO_WritePin(STM32F4_LED_HOME_ALARM_GPIO_PORT, STM32F4_LED_HOME_ALARM_GPIO_PIN, GPIO_PIN_RESET);
    }
}

bool port_led_get_value(uint32_t led_id)
{
    if (led_id == PORT_LED_HOME_ALARM_ID)
    {
        return HAL_GPIO_ReadPin(STM32F4_LED_HOME_ALARM_GPIO_PORT, STM32F4_LED_HOME_ALARM_GPIO_PIN);
    }
    else
    {
        return false; // Invalid LED ID
    }
}

void port_led_set_value(uint32_t led_id, bool value)
{
    if (led_id == PORT_LED_HOME_ALARM_ID)
    {
        HAL_GPIO_WritePin(STM32F4_LED_HOME_ALARM_GPIO_PORT, STM32F4_LED_HOME_ALARM_GPIO_PIN, value);
    }
}

void port_led_toggle(uint32_t led_id)
{
    if (led_id == PORT_LED_HOME_ALARM_ID)
    {
        HAL_GPIO_TogglePin(STM32F4_LED_HOME_ALARM_GPIO_PORT, STM32F4_LED_HOME_ALARM_GPIO_PIN);
    }
}

void port_led_timer_activate(uint32_t led_id, bool activate)
{
    if (led_id == PORT_LED_HOME_ALARM_ID)
    {
        if (activate)
        {
            // Enable the timer
            HAL_TIM_Base_Start_IT(&handler_tim_led_home_alarm);
        }
        else
        {
            // Disable the timer
            HAL_TIM_Base_Stop_IT(&handler_tim_led_home_alarm);
        }
    }
}