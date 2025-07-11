//
// Created by Tim Arnold on 27.02.25.
//

#ifndef ROTATIVE_MOVEMENT_H
#define ROTATIVE_MOVEMENT_H

#include <hardware/gpio.h>
#include <hardware/pwm.h>

#define MOTOR_ROTATIVE_ENABLE_FORWARD_PIN 5
#define MOTOR_ROTATIVE_ENABLE_REVERSE_PIN 8

#define MOTOR_ROTATIVE_PWM_FORWARD 6 //TODO determine move direction
#define MOTOR_ROTATIVE_PWM_REVERSE 7 //TODO determine move direction

#include "../pid_settings.h"
#include "../encoder.h"

namespace rotation::movement {

    float current_rotation = 0;
    float current_rotation_speed = 0;
    float current_rotation_speed_raw = 0;


    float should_rotation = 0;
    float should_rotation_speed = 0; //deg/s

    int should_pwm = 0;
    bool emergency_stop = false;


    //TODO TMP:

    absolute_time_t last_change = 0;
    bool a = false;
    bool disable_control = false;
    absolute_time_t time_since_disable = 0;
    absolute_time_t time_since_nintee = 0;
    bool has_ninte = false;
    bool in_shoot = false;


    void initMotor() {
        gpio_init(MOTOR_ROTATIVE_ENABLE_FORWARD_PIN);
        gpio_init(MOTOR_ROTATIVE_ENABLE_REVERSE_PIN);

        gpio_set_dir(MOTOR_ROTATIVE_ENABLE_FORWARD_PIN, true);
        gpio_set_dir(MOTOR_ROTATIVE_ENABLE_REVERSE_PIN, true);

        gpio_put(MOTOR_ROTATIVE_ENABLE_FORWARD_PIN, true);
        gpio_put(MOTOR_ROTATIVE_ENABLE_REVERSE_PIN, true);

        gpio_set_function(MOTOR_ROTATIVE_PWM_FORWARD, GPIO_FUNC_PWM);
        gpio_set_function(MOTOR_ROTATIVE_PWM_REVERSE, GPIO_FUNC_PWM);

        uint slice_num = pwm_gpio_to_slice_num(MOTOR_ROTATIVE_PWM_FORWARD);

        pwm_config config = pwm_get_default_config();

        pwm_config_set_clkdiv(&config, 200.0f);
        pwm_config_set_wrap(&config, 625);
        pwm_init(slice_num, &config, false);

        pwm_set_gpio_level(MOTOR_ROTATIVE_PWM_FORWARD, 0); //clockwise
        pwm_set_gpio_level(MOTOR_ROTATIVE_PWM_REVERSE, 0); //counterclockwise


        pwm_set_enabled(slice_num, true);
    }


    bool reached_90 = false;
    absolute_time_t time_reached_90 = 0;

    bool reached_plus_90 = false;
    absolute_time_t time_reached_plus_90 = 0;

    bool shoot_running = false;
    void schuss()
    {

        if (!shoot_running)
        {
            should_rotation=-90;
            shoot_running = true;
        }


        if (abs(should_rotation-current_rotation) < 12)
        {
            if (!reached_90)
            {
                reached_90 = true;
                time_reached_90 = get_absolute_time();
            }

            if (absolute_time_diff_us(time_reached_90, get_absolute_time()) > 2000000)
            {
                should_pwm = 625;
                disable_control = true;
            }

        }

        if(current_rotation > 90.0f)
        {
            should_pwm = 0;
            should_rotation = 90;
            disable_control = false;
            pid_settings::pid_rotation.reset();


            if (!reached_plus_90)
            {
                reached_plus_90 = true;
                time_reached_plus_90 = get_absolute_time();
            }

            if (absolute_time_diff_us(time_reached_plus_90, get_absolute_time()) > 3000000)
            {


                should_rotation = 0;

                in_shoot = false;
                shoot_running = false;
                reached_90 = false;
                reached_plus_90 = false;
            }


        }

    }

    void calculate_state()
    {
        current_rotation = (float)encoder::rotation::current_rotation / 2400.0f * 360.0f;

        if (encoder::rotation::measured_pulse_delta_time == 0) {
            current_rotation_speed_raw = 0;
        } else {
            current_rotation_speed_raw = 1.0f / (((double) encoder::rotation::measured_pulse_delta_time / 1.0e6) * 2400.0f) * 360.0f;
            if(!encoder::rotation::is_clockwise)
            {
                current_rotation_speed_raw *= -1;
            }
        }

        current_rotation_speed = current_rotation_speed_raw * 0.30f + current_rotation_speed * 0.70f;

        if(absolute_time_diff_us(encoder::rotation::last_pulse_time, get_absolute_time())>100000) {
            current_rotation_speed = 0;
        }

        //printf("Current Rotation  %f; Should rotation  %f; Should PWM %d\n", current_rotation, should_rotation, should_pwm);
        //printf("%f %f %f %f %d\n", current_rotation, should_rotation, current_rotation_speed, should_rotation_speed, should_pwm);


        if (absolute_time_diff_us(last_change, get_absolute_time())>3000000)
        {

            last_change = get_absolute_time();
            if (a){
                should_rotation = 90;
              //  should_rotation_speed = 10000;
                should_pwm = 0;
               // pid_settings::pid_rotation_speed.reset();
            }else{
               /// should_rotation_speed = 360;
               should_rotation = -45;
              //  should_rotation_speed = 0;
                should_pwm = 0;

                ////pid_settings::pid_rotation_speed.reset();
            }

            a = !a;

            //in_shoot = true;
        }

        if (in_shoot)
        {
            //rotation::movement::schuss();
        }

    }



    bool set_should_rotation(float rotation)
    {
        should_rotation = rotation;

    }

    void calculate_motor_pwm()
    {


       // if (disable_control) return;


        should_rotation_speed = pid_settings::pid_rotation.calculatePID(should_rotation, current_rotation, 0.001f);

       // should_rotation_speed = 11000; // 360 °/s

       // should_pwm = static_cast<int>(should_rotation_speed / 3000.0f * 625.0f);
        should_pwm += (int)pid_settings::pid_rotation_speed.calculatePID(should_rotation_speed, current_rotation_speed, 0.001f, current_rotation_speed > 0);

        if (should_pwm > 625)
        {
            should_pwm = 625;
        }

        if (should_pwm < -625)
        {
            should_pwm = -625;
        }



    }

    void apply_motor_pwm()
    {
        if(emergency_stop) return;

        if (should_pwm > 0) {
            pwm_set_gpio_level(MOTOR_ROTATIVE_PWM_FORWARD, abs(should_pwm));
            pwm_set_gpio_level(MOTOR_ROTATIVE_PWM_REVERSE, 0);
        } else {
            pwm_set_gpio_level(MOTOR_ROTATIVE_PWM_REVERSE, abs(should_pwm));
            pwm_set_gpio_level(MOTOR_ROTATIVE_PWM_FORWARD, 0);
        }
    }

    void emergency() {
        emergency_stop = true;

        pwm_set_gpio_level(MOTOR_ROTATIVE_PWM_FORWARD, 0);
        pwm_set_gpio_level(MOTOR_ROTATIVE_PWM_REVERSE, 0);
        gpio_put(MOTOR_ROTATIVE_ENABLE_FORWARD_PIN, false);
        gpio_put(MOTOR_ROTATIVE_ENABLE_REVERSE_PIN, false);
    }


}
#endif //ROTATIVE_MOVEMENT_H
