/**
 * @file fsm_alarm.c
 * @author Josué Pagán (j.pagan@upm.es)
 * @brief Finite State Machine for an alarm system.
 * @date 2024-04-01
 *
 */

/* Includes ------------------------------------------------------------------*/
/* Standard C includes */
#include <stdlib.h>

/* Project includes */
#include "fsm_alarm.h"
#include "port_button.h"
#include "port_led.h"
#include "port_pir_sensor.h"

/* State machine input or transition functions */

/**
 * @brief Check if there is a new presence in the room. If the presence was already detected, it does not count as a new presence.
 *
 * @param p_this Pointer to the FSM structure
 * @return true If there is presence in the room
 * @return false If there is no presence in the room
 */
bool check_presence(fsm_t *p_this)
{
    // Retrieve the FSM structure and get the PIR sensor
    fsm_alarm_t *p_fsm = (fsm_alarm_t *)p_this;

    // Check if the PIR sensor is activated
    if (port_pir_sensor_get_status(p_fsm->p_pir_sensor))
    {
        if (!p_fsm->alarm_status) // If the alarm is not activated, there is a new presence
        {
            return true;
        }
        else // If the alarm is already activated, it is not a new presence
        {
            return false;
        }
    }
    else
    {
        return false;
    }
}

/**
 * @brief Check if the button indicating the alarm deactivation has been pressed
 *
 * @param p_this Pointer to the FSM structure
 * @return true If the button has been released
 * @return false If the button has not been released
 */
bool check_deactivation(fsm_t *p_this)
{
    // Retrieve the FSM structure and get the button
    fsm_alarm_t *p_fsm = (fsm_alarm_t *)p_this;

    // Check if the button has been already released and not pressed
    return port_button_is_pressed(p_fsm->p_button);
}

/* State machine output or action functions */

/**
 * @brief Activate the alarm system
 *
 * @param p_this Pointer to the FSM structure
 */
void do_activate_alarm(fsm_t *p_this)
{
    // Retrieve the FSM structure and get the LED
    fsm_alarm_t *p_fsm = (fsm_alarm_t *)p_this;

    // Turn on the LED
    port_led_on(p_fsm->p_led);

    // Activate LED timer for blinking
    port_led_timer_activate(p_fsm->p_led);

    // Update the last time the alarm was activated
    p_fsm->alarm_status = true; // If the PIR sensor is activated, activate the alarm
    p_fsm->last_time_alarm = port_system_get_millis();
}

/**
 * @brief Deactivate the alarm system
 *
 * @param p_this Pointer to the FSM structure
 */
void do_deactivate_alarm(fsm_t *p_this)
{
    // Retrieve the FSM structure and get the LED
    fsm_alarm_t *p_fsm = (fsm_alarm_t *)p_this;

    // Turn off the LED timer
    port_led_timer_deactivate(p_fsm->p_led);

    // Turn off the LED
    port_led_off(p_fsm->p_led);

    // Update the alarm status and the button flags
    p_fsm->alarm_status = false;
    p_fsm->p_button->flag_released = false;
    p_fsm->p_button->flag_pressed = false;
}

/* Transitions table ---------------------------------------------------------*/
/* Esto tiene que estar aquí porque las funciones de la FSM son internas */
/*
 * Maquina de estados: lista de transiciones
 * { EstadoOrigen, CondicionDeDisparo, EstadoFinal, AccionesSiTransicion }
 */

/**
 * @brief Transitions table for the alarm system
 *
 */
fsm_trans_t fsm_trans_alarm[] = {
    {ALARM_OFF, check_presence, ALARM_ON, do_activate_alarm},
    {ALARM_ON, check_deactivation, ALARM_OFF, do_deactivate_alarm},
    {-1, NULL, -1, NULL},
};

uint32_t fsm_alarm_get_last_time_alarm(fsm_t *p_this)
{
    fsm_alarm_t *p_fsm = (fsm_alarm_t *)p_this;
    return p_fsm->last_time_alarm;
}

bool fsm_alarm_get_alarm_status(fsm_t *p_this)
{
    fsm_alarm_t *p_fsm = (fsm_alarm_t *)p_this;
    return p_fsm->alarm_status;
}

/* Initialize the FSM */

/**
 * @brief Initialize the alarm system FSM
 *
 * @param p_this Pointer to the FSM structure
 * @param p_button Pointer to the button structure
 * @param p_led Pointer to the LED structure
 * @param p_pir Pointer to the PIR sensor structure
 */
void fsm_alarm_init(fsm_t *p_this, port_button_hw_t *p_button, port_led_hw_t *p_led, port_pir_hw_t *p_pir)
{
    fsm_alarm_t *p_fsm = (fsm_alarm_t *)(p_this);
    fsm_init(p_this, fsm_trans_alarm);

    // Assign the peripherals to the FSM
    p_fsm->p_button = p_button;
    p_fsm->p_led = p_led;
    p_fsm->p_pir_sensor = p_pir;

    // Initialize the last time the alarm was activated
    p_fsm->last_time_alarm = 0;

    // Initialize the alarm status
    p_fsm->alarm_status = false;

    // Initialize the peripherals
    port_button_init(p_button);
    port_led_init(p_led);
    port_pir_sensor_init(p_pir);
}

/* Create FSM */
fsm_t *fsm_alarm_new(port_button_hw_t *p_button, port_led_hw_t *p_led, port_pir_hw_t *p_pir)
{
    // Do malloc for the whole FSM structure to reserve memory for the rest of the FSM, although I interpret it as fsm_t which is the first field of the structure so that the FSM library can work with it
    fsm_t *p_fsm = malloc(sizeof(fsm_alarm_t));

    // Initialize the FSM
    fsm_alarm_init(p_fsm, p_button, p_led, p_pir);

    return p_fsm;
}
