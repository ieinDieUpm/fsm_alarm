/**
 * @file port_button.c
 * @author Josué Pagán (j.pagan@upm.es)
 * @brief Port layer for the button of the STM32F4 Nucleo board.
 * @version 0.1
 * @date 2024-04-01
 *
 */

/* Standard C includes */
#include <stdbool.h>

/* HW dependent includes */
#include "stm32f4xx.h"
#include "port_system.h"
#include "port_button.h"

/* Function definitions ------------------------------------------------------*/
void port_button_exti_config()
{
    /* Enable SYSCFG clock */
    RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;
    
    SYSCFG->EXTICR[3] &= ~(0xFUL << 1 * 4);
    SYSCFG->EXTICR[3] |= (0x02 << 1 * 4);

    /* Rising trigger selection register (EXTI_RTSR) */
    EXTI->RTSR &= ~(EXTI_RTSR_TR0 << BUTTON_PIN);
    EXTI->RTSR |= (EXTI_RTSR_TR0 << BUTTON_PIN);

    /* Falling trigger selection register (EXTI_FTSR) */
    EXTI->FTSR &= ~(EXTI_FTSR_TR0 << BUTTON_PIN);
    EXTI->FTSR |= (EXTI_FTSR_TR0 << BUTTON_PIN);

    /* Interrupt mask register (EXTI_IMR) */
    EXTI->IMR &= ~(EXTI_IMR_MR0 << BUTTON_PIN);
    EXTI->IMR |= (EXTI_IMR_MR0 << BUTTON_PIN);

    NVIC_SetPriority(EXTI15_10_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 1, 0));
    NVIC_EnableIRQ(EXTI15_10_IRQn);
}

void port_button_gpio_setup(void)
{
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN;           // Enable peripheral clock
    BUTTON_GPIO_PORT->MODER &= ~MODER_BUTTON_MASK; // Clean the registers
    BUTTON_GPIO_PORT->PUPDR &= ~PUPDR_BUTTON_MASK;
    BUTTON_GPIO_PORT->MODER |= MODER_BUTTON_AS_INPUT; // Set the corresponding configuration
    BUTTON_GPIO_PORT->PUPDR |= PUPDR_BUTTON_AS_NOPUPD;
}

bool port_button_get_status(void)
{
    return (BUTTON_GPIO_PORT->IDR & IDR_BUTTON_MASK) != 0;
}