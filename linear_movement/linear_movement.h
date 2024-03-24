//
// Created by Tim Arnold on 09.12.23.
//

#include <hardware/gpio.h>
#include <hardware/pwm.h>
#include <valarray>
#include "encoder.h"
#include "../pid_settings.h"
#include "../constants.h"
#include "../rotation/rotation.h"


#define MOTOR_ENABLE_FORWARD_PIN 5
#define MOTOR_ENABLE_REVERSE_PIN 8

#define MOTOR_PWM_FORWARD 6 //TO MOTOR
#define MOTOR_PWM_REVERSE 7 //FROM MOTOR


//CLOCKWISE MEANS MOVING TO THE MOTOR!!!

//zero point is at the servo

#ifndef FOOSBALLMOTORTEST_LINEAR_MOVEMENT_H
#define FOOSBALLMOTORTEST_LINEAR_MOVEMENT_H
namespace linear_movement {
    void initMotor() {
        gpio_init(MOTOR_ENABLE_FORWARD_PIN);
        gpio_init(MOTOR_ENABLE_REVERSE_PIN);

        gpio_set_dir(MOTOR_ENABLE_FORWARD_PIN, true);
        gpio_set_dir(MOTOR_ENABLE_REVERSE_PIN, true);

        gpio_put(MOTOR_ENABLE_FORWARD_PIN, true);
        gpio_put(MOTOR_ENABLE_REVERSE_PIN, true);

        gpio_set_function(MOTOR_PWM_FORWARD, GPIO_FUNC_PWM);
        gpio_set_function(MOTOR_PWM_REVERSE, GPIO_FUNC_PWM);

        uint slice_num = pwm_gpio_to_slice_num(MOTOR_PWM_FORWARD);

        pwm_config config = pwm_get_default_config();

        pwm_config_set_clkdiv(&config, 200.0f);
        pwm_config_set_wrap(&config, 625);
        pwm_init(slice_num, &config, false);

        pwm_set_gpio_level(MOTOR_PWM_FORWARD, 0); //clockwise
        pwm_set_gpio_level(MOTOR_PWM_REVERSE, 0); //counterclockwise


        pwm_set_enabled(slice_num, true);
    }

    float current_position = 0;
    float rotations_per_second = 0;
    float current_speed = 0; //TODO has to be 15 times higher because of Rotations/s --> mm/s

    float should_position = 0; // in mm
    float loop_should_position = 0; //internal should position to break before safety zone

    float rotation_compensation = 0;

    float should_speed = 0;
    int should_pwm = 0;

    bool is_allowed_in_safe_zone_bottom = false;
    bool is_allowed_in_safe_zone_top = false;

    bool is_in_safe_zone_bottom = false;
    bool is_in_safe_zone_top = false;

    bool emergency_stop = false;

    bool is_moving = true;

    void calculate_state() {
        current_position = (float)encoder::current_rotation / 2400.0f * 4.0f * 15.0f;
        if (encoder::measured_pulse_delta_time == 0) {
            rotations_per_second = 0;
        } else {
            rotations_per_second = 1.0f / (((float) encoder::measured_pulse_delta_time / 1.0e6f) * 600.0f) * 4.0f;
        }

        current_speed = rotations_per_second * 0.95f + current_speed * 0.05f;

        if(absolute_time_diff_us(encoder::last_pulse_time, get_absolute_time())>100000) {
            current_speed = 0;
        }

        if(current_speed<13 && current_position<(lower_bound+20)) {
            is_allowed_in_safe_zone_bottom = true;
        }

        if(current_speed<13 && current_position>(upper_bound-20)) {
            is_allowed_in_safe_zone_top = true;
        }

        if(current_position<(lower_bound+20)) {
            is_in_safe_zone_bottom = true;
        }else{
            is_in_safe_zone_bottom = false;
            is_allowed_in_safe_zone_bottom = false;
        }

        if(current_position>(upper_bound-20)) {
            is_in_safe_zone_top = true;
        }else{
            is_in_safe_zone_top = false;
            is_allowed_in_safe_zone_top = false;
        }
    }

    bool set_should_position(float position) {
        if(position > upper_bound || position < lower_bound) {
            return false;
        }

        should_position = position;

        return true;
    }

    void bounds_safety() {
        float accumulated_position = should_position + rotation_compensation;
        if(accumulated_position<(lower_bound+20)&&!is_allowed_in_safe_zone_bottom) {
            loop_should_position = lower_bound+19;
        }else if(accumulated_position>(upper_bound-20)&&!is_allowed_in_safe_zone_top) {
            loop_should_position = upper_bound-19;
        }else{
            loop_should_position = accumulated_position; //TODO check for bounds after applying values --> fatal error maid be caused
        }
    }

    void calculate_rotation_compensation() {
        rotation_compensation = -((float)rotation::currentPulse-START_PULSE)/PULSE_PER_ROTATION*15;
        //rotation_compensation = 0;
    }

    void calculate_motor_pwm() {
        should_speed = pid_settings::pid_pos.calculatePID((float) loop_should_position, current_position, 0.001);

        //set bounds for should_speed in safety zone
        if(is_in_safe_zone_bottom) {
            if (should_speed < -20) {
                should_speed = -20;
            }
        }

        if(is_in_safe_zone_top) {
            if(should_speed > 20) {
                should_speed = 20;
            }
        }


        bool was_zero = abs(should_pwm) < 60;

        should_pwm = (int) pid_settings::pid_speed.calculatePID((float) should_speed, current_speed, 0.001);

        is_moving = abs(should_pwm) > 60;

        if(is_moving && was_zero) {
            encoder::last_encoder_response = get_absolute_time(); //reset timer to avoid emergency stop
        }


        if(is_in_safe_zone_bottom) {
            if (should_pwm < -50) {
                should_pwm = -50;
            }
        }

        if(is_in_safe_zone_top) {
            if(should_pwm > 50) {
                should_pwm = 50;
            }
        }
    }

    void apply_motor_pwm() {
        if(emergency_stop) return;

        if (should_pwm > 0) {
            pwm_set_gpio_level(MOTOR_PWM_FORWARD, abs(should_pwm));
            pwm_set_gpio_level(MOTOR_PWM_REVERSE, 0);
        } else {
            pwm_set_gpio_level(MOTOR_PWM_REVERSE, abs(should_pwm));
            pwm_set_gpio_level(MOTOR_PWM_FORWARD, 0);
        }
    }

    void emergency() {
        emergency_stop = true;

        pwm_set_gpio_level(MOTOR_PWM_FORWARD, 0);
        pwm_set_gpio_level(MOTOR_PWM_REVERSE, 0);
        gpio_put(LED_RED_PIN, true);
        gpio_put(MOTOR_ENABLE_FORWARD_PIN, false);
        gpio_put(MOTOR_ENABLE_REVERSE_PIN, false);
    }



}
#endif //FOOSBALLMOTORTEST_LINEAR_MOVEMENT_H
