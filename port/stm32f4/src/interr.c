/**
 * @file interr.c
 * @brief Interrupt service routines for the STM32F4 platform.
 * @author Josué Pagán (j.pagan@upm.es)
 * @date 2025-03-01
 */
// Include HW dependencies:
#include "port_system.h"

// Include headers of different port elements:
#include "port_button.h"
#include "stm32f4_button.h"
#include "port_led.h"
#include "stm32f4_led.h"
#include "port_pir_sensor.h"
#include "stm32f4_pir_sensor.h"

/* Global variables -----------------------------------------------------------*/
extern TIM_HandleTypeDef handler_tim_led_home_alarm; /*!< Timer handler for the LED */

//------------------------------------------------------
// INTERRUPT SERVICE ROUTINES
//------------------------------------------------------
/**
 * @brief Interrupt service routine for the System tick timer (SysTick).
 *
 * @note This ISR is called when the SysTick timer generates an interrupt.
 * The program flow jumps to this ISR and increments the tick counter by one millisecond.
 *
 * @warning **The variable `msTicks` must be declared volatile!** Just because it is modified by a call of an ISR, in order to avoid [*race conditions*](https://en.wikipedia.org/wiki/Race_condition)
. **Added to the definition** after *static*.
 *
 */
void SysTick_Handler(void)
{
  port_system_set_millis(port_system_get_millis() + 1);
}

/**
 * @brief  This function handles Px10-Px15 global interrupts.
 *
 * First, this function identifies the line/ pin which has raised the interruption. Then, it calls the HAL_xxx_IRQHandler() function to handle the interruption and clear the pending bit.
 *
 */
void EXTI15_10_IRQHandler(void)
{
  if (__HAL_GPIO_EXTI_GET_IT(STM32F4_BUTTON_HOME_ALARM_GPIO_PIN))
  {
    HAL_GPIO_EXTI_IRQHandler(STM32F4_BUTTON_HOME_ALARM_GPIO_PIN);
  }
  else if (__HAL_GPIO_EXTI_GET_IT(STM32F4_PIR_SENSOR_HOME_ALARM_GPIO_PIN))
  {
    HAL_GPIO_EXTI_IRQHandler(STM32F4_PIR_SENSOR_HOME_ALARM_GPIO_PIN);
  }
  else
  {
    // Handle other EXTI lines if needed
  }
}

/**
 * @brief  This function handles the EXTI line detection.
 *
 * This function is called when the EXTI line is detected. It checks the status of the button and increments the counter if the button is pressed.
 *
 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  static bool isr_falling = false;

  // Button
  if (GPIO_Pin == STM32F4_BUTTON_HOME_ALARM_GPIO_PIN)
  {
    if (!port_button_read_value(PORT_BUTTON_HOME_ALARM_ID)) // If the button is pressed
    {
      isr_falling = true;
    }
    else // If the button is released
    {
      if (isr_falling) // If the button was pressed before
      {
        port_button_set_status(PORT_BUTTON_HOME_ALARM_ID, true); // Set the button as pressed
      }
    }
  }

  // PIR sensor
  if (GPIO_Pin == STM32F4_PIR_SENSOR_HOME_ALARM_GPIO_PIN)
  {
    if (port_pir_sensor_read_value(PORT_PIR_SENSOR_HOME_ALARM_ID)) // If the PIR sensor is activated
    {
      port_pir_sensor_set_status(PORT_PIR_SENSOR_HOME_ALARM_ID, true); // Set the PIR sensor as activated
    }
    else // If the PIR sensor is deactivated
    {
      port_pir_sensor_set_status(PORT_PIR_SENSOR_HOME_ALARM_ID, false); // Set the PIR sensor as deactivated
    }
  }
}

/**
 * @brief Interrupt service routine for the Timer 2 (TIM2).
 *
 * @note This ISR is called when the Timer 2 generates an interrupt.
 * The program flow jumps to this ISR then this calls the generic HAL_TIM_IRQHandler() function to clear the update flag.
 *
 */
void TIM2_IRQHandler(void)
{
  HAL_TIM_IRQHandler(&handler_tim_led_home_alarm); // This will clear the update flag internally.
}

/**
 * @brief  Timer period elapsed callback in non blocking mode
 * @param  handler_tim: TIM handle
 *
 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *handler_tim)
{
  if (handler_tim->Instance == STM32F4_LED_HOME_ALARM_TIMER) // If the timer is the LED timer
  {
    port_led_toggle(PORT_LED_HOME_ALARM_ID); // Toggle the LED
  }
}