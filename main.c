/**
 * @file main.c
 * @author Josué Pagán (j.pagan@upm.es)
 * @brief LD2 blinks with a frequency of 1/c. Where c is the number of times the button has been pressed using interrupts for the button and polling for the delay.
 * @version 0.1
 * @date 2024-04-01
 *
 */

/* INCLUDES */
#include "port_system.h"
#include "port_led.h"
#include "port_button.h"

int main()
{
    port_system_init();
    port_led_gpio_setup();
    port_led_timer_setup();

    port_button_gpio_setup();
    port_button_exti_config();

    while (1)
    {        
    }
    return 0;
}