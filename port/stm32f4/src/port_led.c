/**
 * @file port_led.c
 * @author Josué Pagán (j.pagan@upm.es)
 * @brief Port layer for a LED.
 * @date 01-03-2024
 */
/* Standard C includes */
#include <stdio.h>
#include <math.h>

/* HW dependent includes */
#include "stm32f4xx.h"
#include "port_led.h"
#include "port_system.h"

/* Global variables -----------------------------------------------------------*/
port_led_hw_t led_home_alarm = {.p_port = LED_HOME_ALARM_GPIO, .pin = LED_HOME_ALARM_PIN};

bool port_led_get_status(port_led_hw_t *p_led)
{
    return (p_led->p_port->IDR & BIT_POS_TO_MASK(p_led->pin)) != 0;
}

void port_led_on(port_led_hw_t *p_led)
{
    p_led->p_port->ODR |= BIT_POS_TO_MASK(p_led->pin);
}

void port_led_off(port_led_hw_t *p_led)
{
    p_led->p_port->ODR &= ~BIT_POS_TO_MASK(p_led->pin);
}

void port_led_toggle(port_led_hw_t *p_led)
{
    p_led->p_port->ODR ^= BIT_POS_TO_MASK(p_led->pin);
}

void port_led_timer_setup(port_led_hw_t *p_led)
{
    if (p_led->p_port == LED_HOME_ALARM_GPIO && p_led->pin == LED_HOME_ALARM_PIN)
    {
        // Enable the TIM2 peripheral clock
        RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;

        // Disable the timer
        TIM2->CR1 &= ~TIM_CR1_CEN;

        // Autoreload preload enabled
        TIM2->CR1 |= TIM_CR1_ARPE;

        // Reset the values of the timer
        TIM2->CNT = 0;

        // Set the prescaler and the auto-reload register for a 500ms period
        TIM2->PSC = 122;
        TIM2->ARR = 65039;

        // Clean interrupt flags
        TIM2->SR &= ~TIM_SR_UIF;

        // Enable the update interrupt
        TIM2->DIER |= TIM_DIER_UIE;

        // Enable the interrupt in the NVIC
        NVIC_SetPriority(TIM2_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 2, 0)); /* Priority 2, sub-priority 0 */
        NVIC_EnableIRQ(TIM2_IRQn);
    }
}

void port_led_timer_activate(port_led_hw_t *p_led)
{
    // Enable the timer
    if (p_led->p_port == LED_HOME_ALARM_GPIO && p_led->pin == LED_HOME_ALARM_PIN)
    {
        TIM2->CR1 |= TIM_CR1_CEN;

        // The PSC and ARR values are currently in the preload registers. To load them into the active registers we
        // need an update event. We can do this manually as follows (or we could wait for the timer to expire).
        TIM2->EGR |= TIM_EGR_UG; // 6) Update generation: Re-inicializa el contador y actualiza los registros. IMPORTANTE que esté lo último y que se haga siempre.
    }
}

void port_led_timer_deactivate(port_led_hw_t *p_led)
{
    // Disable the timer
    if (p_led->p_port == LED_HOME_ALARM_GPIO && p_led->pin == LED_HOME_ALARM_PIN)
    {
        TIM2->CR1 &= ~TIM_CR1_CEN;
    }
}

void port_led_init(port_led_hw_t *p_led)
{
    port_system_gpio_config(p_led->p_port, p_led->pin, GPIO_MODE_OUT, GPIO_PUPDR_NOPULL);
    port_led_timer_setup(p_led);
}