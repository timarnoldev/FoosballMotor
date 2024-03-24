//
// Created by Tim Arnold on 27.12.23.
//

#ifndef FOOSBALLMOTORTEST_ROTATION_H
#define FOOSBALLMOTORTEST_ROTATION_H

#include <cstdio>
#include <hardware/gpio.h>
#include <hardware/pwm.h>
#include <pico/time.h>
#include <hardware/structs/clocks.h>
#include <hardware/clocks.h>
#include "../constants.h"


namespace rotation {

    #define START_PULSE 1500 //--> greater pulse means moving clockwise --> thereby moving the player to the motor (greater distance)
    #define PULSE_PER_ROTATION 950 //--> greater pulse means moving clockwise --> thereby moving the player to the motor (greater distance)

    uint16_t currentPulse = START_PULSE;

    void setPulse(uint16_t micros) {
        pwm_set_gpio_level(SERVO_PIN,
                           (uint16_t) (((float) clock_get_hz(clk_sys) / 1000.0f * (float) micros / (float) 1000) /
                                       (float) 16));
        currentPulse = micros;
    }

    void initServo() {

        gpio_set_function(SERVO_PIN, GPIO_FUNC_PWM);

        uint slice_num_16 = pwm_gpio_to_slice_num(SERVO_PIN);

        pwm_config config_16 = pwm_get_default_config();

        pwm_config_set_clkdiv(&config_16, 16.0f);
        pwm_config_set_wrap(&config_16, ((float) clock_get_hz(clk_sys) / 1000 * 4) / (float) 16);
        pwm_init(slice_num_16, &config_16, true);

        setPulse(START_PULSE);

        pwm_set_enabled(slice_num_16, true);

        sleep_ms(100);
    }


}
#endif //FOOSBALLMOTORTEST_ROTATION_H
