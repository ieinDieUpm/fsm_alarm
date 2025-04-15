/**
 * @file main.c
 * @author Josué Pagán (j.pagan@upm.es)
 * @brief Main file for the alarm system FSM.
 * @version 0.1
 * @date 2025-04-01
 *
 */

/* INCLUDES */
/* Standard C includes */
#include <stdio.h>

/* HW dependent includes */
#include "port_system.h"
#include "port_button.h"
#include "port_led.h"
#include "port_pir_sensor.h"

/* Project includes */
#include "fsm_alarm.h"

/* MAIN FUNCTION */

/**
 * @brief Main function
 * 
 * @return int 
 */
int main()
{
    // Local variables
    bool previous_alarm_status = false;

    /* Init board */
    port_system_init();

    // Create an alarm system FSM and get a pointer to it
    fsm_alarm_t *p_fsm_home_alarm = fsm_alarm_new(
        PORT_BUTTON_HOME_ALARM_ID,
        PORT_LED_HOME_ALARM_ID,
        PORT_PIR_SENSOR_HOME_ALARM_ID);

    while (1)
    {
        // Launch the FSM
        fsm_alarm_fire(p_fsm_home_alarm);

        bool current_alarm_status = fsm_alarm_get_alarm_status(p_fsm_home_alarm);
        if (current_alarm_status != previous_alarm_status)
        {
            uint32_t last_time_alarm = fsm_alarm_get_last_time_alarm(p_fsm_home_alarm);
            if (current_alarm_status)
            {
                printf("ALARM!!! Alarm activated at %ld\n", last_time_alarm);
            }
            else
            {
                printf("Alarm deactivated\n");
            }
            previous_alarm_status = current_alarm_status;
        }
    }
    return 0;
}