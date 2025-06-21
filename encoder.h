//
// Created by Tim Arnold on 27.02.25.
//

#ifndef ROTATIONENCODER_H
#define ROTATIONENCODER_H

#include <cstdio>
#include <pico/types.h>
#include <hardware/gpio.h>
#include <pico/time.h>

//RIGHT SIDE IS LINEAR MOTOR

//RIGHT SIDE
#define ENCODER_LINEAR_PHASE_A_PIN 18 //GREEN CABLE
#define ENCODER_LINEAR_PHASE_B_PIN 19 //WHITE CABLE

//LEFT SIDE
#define ENCODER_ROTATION_PHASE_A_PIN 20 //GREEN CABLE
#define ENCODER_ROTATION_PHASE_B_PIN 21 //WHITE CABLE

//note that a and b don't have the same meaning on in the code and on the schematics

namespace encoder
{
    namespace linear_movement
    {
        bool is_phase_a_high = false;
        bool is_phase_b_high = false;
        bool has_encoder_state_changed = false;
        int last_step = 0;
        bool is_clockwise = false;
        int current_rotation = 0;
        int encoder_errors = 0;
        uint measured_pulse_delta_time = 0;
        absolute_time_t last_pulse_time;
        absolute_time_t last_encoder_response = get_absolute_time();


        void encoder_irq(uint gpio, uint32_t events)
        {
            if (gpio == ENCODER_LINEAR_PHASE_A_PIN)
            {
                if (gpio_get(gpio))
                {
                    if (!is_phase_a_high)
                    {
                        has_encoder_state_changed = true;
                    }
                    is_phase_a_high = true;
                }
                else if (!gpio_get(gpio))
                {
                    if (is_phase_a_high)
                    {
                        has_encoder_state_changed = true;
                    }
                    is_phase_a_high = false;
                }
            }

            if (gpio == ENCODER_LINEAR_PHASE_B_PIN)
            {
                if (gpio_get(gpio))
                {
                    if (!is_phase_b_high)
                    {
                        has_encoder_state_changed = true;
                    }
                    is_phase_b_high = true;
                }
                else if (!gpio_get(gpio))
                {
                    if (is_phase_b_high)
                    {
                        has_encoder_state_changed = true;
                    }
                    is_phase_b_high = false;
                }
            }

            if (has_encoder_state_changed)
            {
                measured_pulse_delta_time = absolute_time_diff_us(last_pulse_time, get_absolute_time());
                last_pulse_time = get_absolute_time();

                if (!is_phase_a_high && is_phase_b_high)
                {
                    if (last_step == 1)
                    {
                        current_rotation--;
                        is_clockwise = false;
                    }
                    else if (last_step == 3)
                    {
                        current_rotation++;
                        is_clockwise = true;
                    }
                    else
                    {
                        encoder_errors++;
                    }
                    //STEP 0
                    last_step = 0;

                    last_pulse_time = get_absolute_time();
                }

                if (!is_phase_a_high && !is_phase_b_high)
                {
                    if (last_step == 2)
                    {
                        current_rotation--;
                        is_clockwise = false;
                    }
                    else if (last_step == 0)
                    {
                        current_rotation++;
                        is_clockwise = true;
                    }
                    else
                    {
                        encoder_errors++;
                    }

                    //STEP 1
                    last_step = 1;
                }

                if (is_phase_a_high && !is_phase_b_high)
                {
                    if (last_step == 3)
                    {
                        current_rotation--;
                        is_clockwise = false;
                    }
                    else if (last_step == 1)
                    {
                        current_rotation++;
                        is_clockwise = true;
                    }
                    else
                    {
                        encoder_errors++;
                    }
                    //STEP 2
                    last_step = 2;
                }

                if (is_phase_a_high && is_phase_b_high)
                {
                    if (last_step == 0)
                    {
                        current_rotation--;
                        is_clockwise = false;
                    }
                    else if (last_step == 2)
                    {
                        current_rotation++;
                        is_clockwise = true;
                    }
                    else
                    {
                        encoder_errors++;
                    }
                    //STEP 3
                    last_step = 3;
                }
                last_encoder_response = get_absolute_time();
                has_encoder_state_changed = false;
            }
            else
            {
                // encoder_errors++;
            }
        }
    }


     namespace rotation
    {
        bool is_phase_a_high = false;
        bool is_phase_b_high = false;
        bool has_encoder_state_changed = false;
        int last_step = 0;
        bool is_clockwise = false;
        int current_rotation = 0;
        int encoder_errors = 0;
        uint measured_pulse_delta_time = 0;
        absolute_time_t last_pulse_time;
        absolute_time_t last_encoder_response = get_absolute_time();


        void encoder_irq(uint gpio, uint32_t events)
        {
            if (gpio == ENCODER_ROTATION_PHASE_A_PIN)
            {
                if (gpio_get(gpio))
                {
                    if (!is_phase_a_high)
                    {
                        has_encoder_state_changed = true;
                    }
                    is_phase_a_high = true;
                }
                else if (!gpio_get(gpio))
                {
                    if (is_phase_a_high)
                    {
                        has_encoder_state_changed = true;
                    }
                    is_phase_a_high = false;
                }
            }

            if (gpio == ENCODER_ROTATION_PHASE_B_PIN)
            {
                if (gpio_get(gpio))
                {
                    if (!is_phase_b_high)
                    {
                        has_encoder_state_changed = true;
                    }
                    is_phase_b_high = true;
                }
                else if (!gpio_get(gpio))
                {
                    if (is_phase_b_high)
                    {
                        has_encoder_state_changed = true;
                    }
                    is_phase_b_high = false;
                }
            }

            if (has_encoder_state_changed)
            {
                measured_pulse_delta_time = absolute_time_diff_us(last_pulse_time, get_absolute_time());
                last_pulse_time = get_absolute_time();

                if (!is_phase_a_high && is_phase_b_high)
                {
                    if (last_step == 1)
                    {
                        current_rotation--;
                        is_clockwise = false;
                    }
                    else if (last_step == 3)
                    {
                        current_rotation++;
                        is_clockwise = true;
                    }
                    else
                    {
                        encoder_errors++;
                    }
                    //STEP 0
                    last_step = 0;

                    last_pulse_time = get_absolute_time();
                }

                if (!is_phase_a_high && !is_phase_b_high)
                {
                    if (last_step == 2)
                    {
                        current_rotation--;
                        is_clockwise = false;
                    }
                    else if (last_step == 0)
                    {
                        current_rotation++;
                        is_clockwise = true;
                    }
                    else
                    {
                        encoder_errors++;
                    }

                    //STEP 1
                    last_step = 1;
                }

                if (is_phase_a_high && !is_phase_b_high)
                {
                    if (last_step == 3)
                    {
                        current_rotation--;
                        is_clockwise = false;
                    }
                    else if (last_step == 1)
                    {
                        current_rotation++;
                        is_clockwise = true;
                    }
                    else
                    {
                        encoder_errors++;
                    }
                    //STEP 2
                    last_step = 2;
                }

                if (is_phase_a_high && is_phase_b_high)
                {
                    if (last_step == 0)
                    {
                        current_rotation--;
                        is_clockwise = false;
                    }
                    else if (last_step == 2)
                    {
                        current_rotation++;
                        is_clockwise = true;
                    }
                    else
                    {
                        encoder_errors++;
                    }
                    //STEP 3
                    last_step = 3;
                }
                last_encoder_response = get_absolute_time();
                has_encoder_state_changed = false;
            }
            else
            {
                // encoder_errors++;
            }
        }
    }


    void global_irq(uint gpio, uint32_t events)
    {
        if (gpio == ENCODER_LINEAR_PHASE_A_PIN || gpio == ENCODER_LINEAR_PHASE_B_PIN)
        {
            linear_movement::encoder_irq(gpio, events);
        }

        if (gpio == ENCODER_ROTATION_PHASE_A_PIN || gpio == ENCODER_ROTATION_PHASE_B_PIN)
        {
            rotation::encoder_irq(gpio, events);
        }
    }

    void init_encoder()
    {
        irq_set_priority(IO_IRQ_BANK0, 256u);

        gpio_set_irq_callback(&global_irq);

        gpio_set_irq_enabled(ENCODER_LINEAR_PHASE_A_PIN, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true);
        gpio_set_irq_enabled(ENCODER_LINEAR_PHASE_B_PIN, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true);
        gpio_set_irq_enabled(ENCODER_ROTATION_PHASE_A_PIN, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true);
        gpio_set_irq_enabled(ENCODER_ROTATION_PHASE_B_PIN, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true);

        irq_set_enabled(IO_IRQ_BANK0, true);
    }
}


#endif //ROTATIONENCODER_H
