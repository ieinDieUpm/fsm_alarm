/**
 * @file fsm_alarm.h
 * @author Josué Pagán (j.pagan@upm.es)
 * @brief Header file for the alarm system FSM.
 * @date 2025-04-01
 *
 */

#ifndef FSM_ALARM_H
#define FSM_ALARM_H

/* Includes ------------------------------------------------------------------*/
/* Standard C includes */
#include <stdint.h>
#include <stdbool.h>

/* Other includes */
#include "fsm.h"

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
typedef struct fsm_alarm_t fsm_alarm_t;

/* Function prototypes and explanations ---------------------------------------*/
/**
 * @brief Creates a new alarm system FSM.
 *
 * @param button_id ID of the button associated to the FSM.
 * @param led_id ID of the LED associated to the FSM.
 * @param pir_sensor ID of the PIR sensor associated to the FSM.
 * @return fsm_alarm_t* Pointer to the new alarm system FSM.
 */
fsm_alarm_t *fsm_alarm_new(uint32_t button_id, uint32_t led_id, uint32_t pir_sensor);

/**
 * @brief Gets the last time the alarm was activated with a new presence.
 *
 * @param p_this Pointer to the FSM structure.
 */
uint32_t fsm_alarm_get_last_time_alarm(fsm_alarm_t *p_this);

/**
 * @brief Gets the alarm status.
 *
 * @param p_this Pointer to the FSM structure.
 */
bool fsm_alarm_get_alarm_status(fsm_alarm_t *p_this);

/**
 * @brief Fire the alarm FSM.
 *
 * This function is used to check the transitions and execute the actions of the alarm FSM.
 *
 * @param p_fsm Pointer to the `fsm_alarm_t` struct.
 */
void fsm_alarm_fire(fsm_alarm_t *p_fsm);

#endif /* FSM_ALARM_H */