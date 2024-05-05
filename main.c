/**
 * @file main.c
 * @author Josué Pagán (j.pagan@upm.es)
 * @brief Main file for the alarm system FSM.
 * @version 0.1
 * @date 2024-04-01
 *
 */

/* INCLUDES */
#include <stdio.h>
#include "port_system.h"
#include "fsm_alarm.h"

int main()
{
    // Local variables
    bool previous_alarm_status = false;

    /* Init board */
    port_system_init();

    // Create an alarm system FSM and get a pointer to it
    fsm_t *p_fsm_home_alarm = fsm_alarm_new(&button_home_alarm, &led_home_alarm, &pir_sensor_home_alarm);

    while (1)
    {
        // Launch the FSM
        fsm_fire(p_fsm_home_alarm);

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