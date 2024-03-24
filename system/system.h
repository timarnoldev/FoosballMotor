//
// Created by Tim Arnold on 27.12.23.
//

#ifndef FOOSBALLMOTORTEST_SYSTEM_H
#define FOOSBALLMOTORTEST_SYSTEM_H

#include <hardware/gpio.h>
#include "../constants.h"

namespace systemPCB {
    void initPCB() {
        gpio_init(LED_RED_PIN);
        gpio_init(LED_YELLOW_PIN);
        gpio_init(LED_BLUE_PIN);

        gpio_set_dir(LED_RED_PIN, GPIO_OUT);
        gpio_set_dir(LED_YELLOW_PIN, GPIO_OUT);
        gpio_set_dir(LED_BLUE_PIN, GPIO_OUT);
    }

    void updateLEDSignals() {
        gpio_put(LED_BLUE_PIN, linear_movement::is_in_safe_zone_top || linear_movement::is_in_safe_zone_bottom);
        gpio_put(LED_YELLOW_PIN, !communication::is_connected());
    }

    void ledTest() {
        gpio_put(LED_BLUE_PIN, true);
        gpio_put(LED_YELLOW_PIN, true);
        gpio_put(LED_RED_PIN, true);
        sleep_ms(1000);
        gpio_put(LED_BLUE_PIN, false);
        gpio_put(LED_YELLOW_PIN, false);
        gpio_put(LED_RED_PIN, false);
    }

    void debug_output() {
        if (!error_checker::fatal_error) {
            printf("Position: %f, %f, %d\n", linear_movement::current_position,
                   linear_movement::should_position, linear_movement::encoder::current_rotation, pid_settings::pid_speed.lasterror); //600 per rotation 1:4 -> 15mm
        }
    }
}
#endif //FOOSBALLMOTORTEST_SYSTEM_H
