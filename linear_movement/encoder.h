//
// Created by Tim Arnold on 09.12.23.
//

#ifndef FOOSBALLMOTORTEST_ENCODER_H
#define FOOSBALLMOTORTEST_ENCODER_H

#include <cstdio>
#include <pico/types.h>
#include <hardware/gpio.h>
#include <pico/time.h>

#define ENCODER_PHASE_A_PIN 11 //GREEN CABLE
#define ENCODER_PHASE_B_PIN 10 //WHITE CABLE

namespace linear_movement::encoder {

    bool is_phase_a_high = false;
    bool is_phase_b_high = false;
    bool has_encoder_state_changed = false;
    int last_step = 0;
    int current_rotation = 0;
    int encoder_errors = 0;
    uint measured_pulse_delta_time = 0;
    absolute_time_t last_pulse_time;
    absolute_time_t last_encoder_response = get_absolute_time();

    void encoder_irq(uint gpio, uint32_t events) {

        if (gpio == ENCODER_PHASE_A_PIN) { //clockwise
            if (gpio_get(gpio)) {
                if (!is_phase_a_high) {
                    has_encoder_state_changed = true;
                }
                is_phase_a_high = true;
            } else if (!gpio_get(gpio)) {
                if (is_phase_a_high) {
                    has_encoder_state_changed = true;
                }
                is_phase_a_high = false;
            }
        }

        if (gpio == ENCODER_PHASE_B_PIN) { //counterclockwise
            if (gpio_get(gpio)) {
                if (!is_phase_b_high) {
                    has_encoder_state_changed = true;
                }
                is_phase_b_high = true;
            } else if (!gpio_get(gpio)) {
                if (is_phase_b_high) {
                    has_encoder_state_changed = true;
                }
                is_phase_b_high = false;
            }
        }

        if (has_encoder_state_changed) {
            if (!is_phase_a_high && is_phase_b_high) {
                if (last_step == 1) {
                    current_rotation--;
                } else if (last_step == 3) {
                    current_rotation++;
                } else {
                    encoder_errors++;
                }
                //STEP 0
                last_step = 0;

                last_pulse_time = get_absolute_time();
            }

            if (!is_phase_a_high && !is_phase_b_high) {
                if (last_step == 2) {
                    current_rotation--;
                } else if (last_step == 0) {
                    current_rotation++;
                } else {
                    encoder_errors++;
                }

                //STEP 1
                last_step = 1;
            }

            if (is_phase_a_high && !is_phase_b_high) {
                if (last_step == 3) {
                    current_rotation--;
                } else if (last_step == 1) {
                    current_rotation++;
                } else {
                    encoder_errors++;
                }
                //STEP 2
                last_step = 2;
            }

            if (is_phase_a_high && is_phase_b_high) {
                if (last_step == 0) {
                    current_rotation--;
                } else if (last_step == 2) {
                    current_rotation++;
                } else {
                    encoder_errors++;
                }
                //STEP 3
                last_step = 3;

                measured_pulse_delta_time = absolute_time_diff_us(last_pulse_time, get_absolute_time());
            }
            last_encoder_response = get_absolute_time();
            has_encoder_state_changed = false;
        } else {
            // encoder_errors++;
        }

    }

    void init_encoder() {
        irq_set_priority(IO_IRQ_BANK0, 256u);

        gpio_set_irq_callback(&encoder_irq);

        gpio_set_irq_enabled(ENCODER_PHASE_A_PIN, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true);
        gpio_set_irq_enabled(ENCODER_PHASE_B_PIN, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true);

        irq_set_enabled(IO_IRQ_BANK0, true);
    }
}


#endif //FOOSBALLMOTORTEST_ENCODER_H
