//
// Created by Tim Arnold on 27.12.23.
//

#ifndef FOOSBALLMOTORTEST_SYSTEM_H
#define FOOSBALLMOTORTEST_SYSTEM_H

#include <hardware/gpio.h>

#include "communication.h"
#include "../constants.h"

namespace systemPCB
{
    void initPCB()
    {
        gpio_init(LED_RED_PIN);
        gpio_init(LED_YELLOW_PIN);
        gpio_init(LED_BLUE_PIN);

        gpio_set_dir(LED_RED_PIN, GPIO_OUT);
        gpio_set_dir(LED_YELLOW_PIN, GPIO_OUT);
        gpio_set_dir(LED_BLUE_PIN, GPIO_OUT);
    }

    absolute_time_t last_time = get_absolute_time();
    bool yellow_led_on = true;

    void updateLEDSignals()
    {

        if(absolute_time_diff_us(last_time, get_absolute_time()) > 500000)
        {
            yellow_led_on = !yellow_led_on;
            last_time = get_absolute_time();
        }

        gpio_put(LED_BLUE_PIN, linear_movement::is_in_safe_zone_top || linear_movement::is_in_safe_zone_bottom);

        if (communication::is_connected())
        {
            gpio_put(LED_YELLOW_PIN, true);
        }
        else
        {
            if(communication::is_mounted())
            {
                gpio_put(LED_YELLOW_PIN, yellow_led_on);
            }
            else
            {
                gpio_put(LED_YELLOW_PIN, false);
            }
        }
    }

    void ledTest()
    {
        gpio_put(LED_BLUE_PIN, true);
        gpio_put(LED_YELLOW_PIN, true);
        gpio_put(LED_RED_PIN, true);
        sleep_ms(1000);
        gpio_put(LED_BLUE_PIN, false);
        gpio_put(LED_YELLOW_PIN, false);
        gpio_put(LED_RED_PIN, false);
    }

    void debug_output()
    {
        if (!error_checker::fatal_error)
        {
            // printf("Position: %f, %f, %d\n", linear_movement::current_position,
            //         linear_movement::should_position, linear_movement::encoder::current_rotation, pid_settings::pid_speed.lasterror); //600 per rotation 1:4 -> 15mm
        }
    }
}
#endif //FOOSBALLMOTORTEST_SYSTEM_H
