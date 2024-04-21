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

/* Defines and enums ----------------------------------------------------------*/
#define BUTTON_EXTI_PR_MASK (EXTI_PR_PR0 << BUTTON_PIN) /*!< Mask for the button in the Pending Register */

/* Global variables -----------------------------------------------------------*/
volatile bool button_pressed = false; /*!< Flag to indicate if the button is pressed */
volatile uint32_t c = 0;              /*!< Counter for the button press */

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
  static bool isr_falling = false;

  if (EXTI->PR & BUTTON_EXTI_PR_MASK)
  {
    if (!port_button_get_status()) // If the button is pressed
    {
      isr_falling = true;
    }
    else // If the button is released
    {
      if (isr_falling) // If the button was pressed before
      {
        isr_falling = false;   // Reset the flag
        button_pressed = true; // Set the flag
        c++;                   // Increment the counter
        port_led_timer_delay_ms(500 / c);
      }
    }

    EXTI->PR |= BUTTON_EXTI_PR_MASK; // Para limpiar el flag que se encuentre a ‘1’ hay que escribir un ‘1’ en dicho bit. Escribir ‘0’ no afecta al estado del bit
  }
}

void TIM2_IRQHandler(void)
{
  TIM2->SR &= ~TIM_SR_UIF; // Clear the update interrupt flag

  if (c > 0)
  {
    port_led_toggle();
    button_pressed = false; // Reset the flag
  }
}