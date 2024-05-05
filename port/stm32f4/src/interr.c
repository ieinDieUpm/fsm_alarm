/**
 * @file interr.c
 * @brief Interrupt service routines for the STM32F4 platform.
 * @author Josué Pagán (j.pagan@upm.es)
 * @date 2024-04-01
 */
// Include headers of different port elements:
#include "stm32f4xx.h"
#include "port_system.h"
#include "port_button.h"
#include "port_led.h"
#include "port_pir_sensor.h"

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
 * First, this function identifies the line/ pin which has raised the interruption. Then, perform the desired action. Before leaving it cleans the interrupt pending register.
 *
 */
void EXTI15_10_IRQHandler(void)
{
  // Button
  if (EXTI->PR & BIT_POS_TO_MASK(button_home_alarm.pin))
  {
    if (port_button_read_gpio(&button_home_alarm)) // If the button is released (not pressed)
    {
      if (button_home_alarm.flag_pressed) // If the button was pressed before
      {
        button_home_alarm.flag_released = true; // Set the flag
        button_home_alarm.flag_pressed = false; // Reset the flag
      }
    }
    else // If the button is pressed
    {
      button_home_alarm.flag_released = false; // Reset the flag
      button_home_alarm.flag_pressed = true;   // Set the flag
    }

    EXTI->PR |= BIT_POS_TO_MASK(button_home_alarm.pin); // Para limpiar el flag que se encuentre a ‘1’ hay que escribir un ‘1’ en dicho bit. Escribir ‘0’ no afecta al estado del bit
  }

  // PIR sensor
  if (EXTI->PR & BIT_POS_TO_MASK(pir_sensor_home_alarm.pin))
  {
    if (port_pir_sensor_read_gpio(&pir_sensor_home_alarm))
    {
      pir_sensor_home_alarm.sensor_status = true;
    }
    else
    {
      pir_sensor_home_alarm.sensor_status = false;
    }

    EXTI->PR |= BIT_POS_TO_MASK(pir_sensor_home_alarm.pin);
  }
}

/**
 * @brief Interrupt service routine for the TIM2 timer.
 *
 * @note This ISR is called when the TIM2 timer generates an interrupt.
 * The program flow jumps to this ISR and toggles the LED.
 *
 */
void TIM2_IRQHandler(void)
{
  port_led_toggle(&led_home_alarm);
  
  TIM2->SR &= ~TIM_SR_UIF; // Clear the update interrupt flag
}