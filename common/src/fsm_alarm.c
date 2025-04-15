/**
 * @file fsm_alarm.c
 * @author Josué Pagán (j.pagan@upm.es)
 * @brief Finite State Machine for an alarm system.
 * @date 2025-04-01
 *
 */

/* Includes ------------------------------------------------------------------*/
/* Standard C includes */
#include <stdlib.h>

/* HW dependent includes */
#include "port_system.h"
#include "port_button.h"
#include "port_led.h"
#include "port_pir_sensor.h"

/* Project includes */
#include "fsm.h"
#include "fsm_alarm.h"

/**
 * @brief Structure to define the alarm system FSM.
 */
struct fsm_alarm_t
{
    fsm_t f;                  /*!< FSM structure. Important to be the first element of the structure */
    uint32_t button_id;       /*!< ID of the button associated to the FSM */
    uint32_t led_id;          /*!< ID of the LED associated to the FSM */
    uint32_t pir_sensor_id;   /*!< ID of the PIR sensor associated to the FSM */
    bool alarm_status;        /*!< Status of the alarm */
    uint32_t last_time_alarm; /*!< Last time the alarm was activated */
};

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
    if (port_pir_sensor_get_status(p_fsm->pir_sensor_id))
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
    return port_button_get_status(p_fsm->button_id);
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
    port_led_set_value(p_fsm->led_id, true);

    // Activate LED timer for blinking
    port_led_timer_activate(p_fsm->led_id, true);

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
    port_led_timer_activate(p_fsm->led_id, false);

    // Turn off the LED
    port_led_set_value(p_fsm->led_id, false);

    // Update the alarm status and the button flags
    p_fsm->alarm_status = false;
    port_button_set_status(p_fsm->button_id, false); // Reset the button status
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

uint32_t fsm_alarm_get_last_time_alarm(fsm_alarm_t *p_this)
{
    return p_this->last_time_alarm;
}

bool fsm_alarm_get_alarm_status(fsm_alarm_t *p_this)
{
    return p_this->alarm_status;
}

/* Initialize the FSM */

/**
 * @brief Initialize the alarm system FSM
 *
 * @param p_fsm_alarm Pointer to the FSM structure
 * @param button_id ID of the button associated to the FSM
 * @param led_id ID of the LED associated to the FSM
 * @param pir_sensor_id ID of the PIR sensor associated to the FSM
 */
void fsm_alarm_init(fsm_alarm_t *p_fsm_alarm, uint32_t button_id, uint32_t led_id, uint32_t pir_sensor_id)
{
    fsm_init(&p_fsm_alarm->f, fsm_trans_alarm);

    // Assign the peripherals to the FSM
    p_fsm_alarm->button_id = button_id;
    p_fsm_alarm->led_id = led_id;
    p_fsm_alarm->pir_sensor_id = pir_sensor_id;

    // Initialize the last time the alarm was activated
    p_fsm_alarm->last_time_alarm = 0;

    // Initialize the alarm status
    p_fsm_alarm->alarm_status = false;

    // Initialize the peripherals
    port_button_gpio_setup(button_id);
    port_led_gpio_setup(led_id);
    port_pir_sensor_gpio_setup(pir_sensor_id);
}

/* Create FSM */
fsm_alarm_t *fsm_alarm_new(uint32_t button_id, uint32_t led_id, uint32_t pir_sensor_id)
{
    // Do malloc for the whole FSM structure to reserve memory for the rest of the FSM, although I interpret it as fsm_t which is the first field of the structure so that the FSM library can work with it
    fsm_alarm_t *p_fsm_alarm = malloc(sizeof(fsm_alarm_t));

    // Initialize the FSM
    fsm_alarm_init(p_fsm_alarm, button_id, led_id, pir_sensor_id);

    return p_fsm_alarm;
}

void fsm_alarm_fire(fsm_alarm_t *p_fsm)
{
    fsm_fire((fsm_t *)p_fsm); // Call the FSM fire function
}