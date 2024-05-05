/**
 * @file fsm_alarm.h
 * @author Josué Pagán (j.pagan@upm.es)
 * @brief Header file for the alarm system FSM.
 * @date 2024-04-01
 *
 */

#ifndef FSM_ALARM_H
#define FSM_ALARM_H

/* Includes ------------------------------------------------------------------*/
/* Standard C includes */
#include <stdint.h>

/* Other includes */
#include <fsm.h>
#include "port_button.h"
#include "port_led.h"
#include "port_pir_sensor.h"

/* Defines and enums ----------------------------------------------------------*/
/* Enums */
/**
 * @brief Enumerates the states of the alarm system FSM.
 *
 */
enum FSM_ALARM_STATES
{
    ALARM_ON = 0, /*!< Alarm is on */
    ALARM_OFF     /*!< Alarm is off */
};

/* Typedefs ------------------------------------------------------------------*/
typedef struct
{
    fsm_t f;                     /*!< FSM structure. Important to be the first element of the structure */
    port_button_hw_t *p_button;  /*!< Pointer to the button structure */
    port_led_hw_t *p_led;        /*!< Pointer to the LED structure */
    port_pir_hw_t *p_pir_sensor; /*!< Pointer to the PIR sensor structure */
    bool alarm_status;           /*!< Status of the alarm */
    uint32_t last_time_alarm;    /*!< Last time the alarm was activated */
} fsm_alarm_t;

/* Function prototypes and explanations ---------------------------------------*/
/**
 * @brief Creates a new alarm system FSM.
 * 
 * @param p_button Pointer to the button of the alarm system.
 * @param p_led Pointer to the LED of the alarm system.
 * @param p_pir Pointer to the PIR sensor of the alarm system.
 * @return fsm_alarm_t* Pointer to the new alarm system FSM. 
 */
fsm_t *fsm_alarm_new(port_button_hw_t *p_button, port_led_hw_t *p_led, port_pir_hw_t *p_pir);

/**
 * @brief Gets the last time the alarm was activated with a new presence.
 *
 * @param p_this Pointer to the FSM structure.
 */
uint32_t fsm_alarm_get_last_time_alarm(fsm_t *p_this);

/**
 * @brief Gets the alarm status.
 *
 * @param p_this Pointer to the FSM structure.
 */
bool fsm_alarm_get_alarm_status(fsm_t *p_this);

#endif /* FSM_ALARM_H */