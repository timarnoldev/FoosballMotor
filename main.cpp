#include <cstdio>
#include <pico/multicore.h>
#include "pico/stdlib.h"
#include "pico/time.h"
#include "communication.h"
#include "linear_movement/encoder.h"
#include "linear_movement/linear_movement.h"
#include "error_checker.h"
#include "rotation/rotation.h"
#include "system/system.h"
#include "secondCoreEntry.h"

int main() {
    stdio_init_all();

    sleep_ms(1000);

    rotation::initServo();
    systemPCB::initPCB();
    systemPCB::ledTest();

    linear_movement::encoder::init_encoder();
    linear_movement::initMotor();

    multicore_launch_core1(secondCoreEntry);

    absolute_time_t last_update = get_absolute_time();

    while (true) {
        last_update = get_absolute_time();
        linear_movement::calculate_state();
        linear_movement::calculate_rotation_compensation();
        linear_movement::bounds_safety();
        linear_movement::calculate_motor_pwm();
        linear_movement::apply_motor_pwm();

        error_checker::check_for_fatal_errors();



        if (error_checker::fatal_error) {
            linear_movement::emergency();
        }

        systemPCB::updateLEDSignals();

        linear_movement::set_should_position((float)communication::current_position);
        rotation::setPulse(communication::current_rotation);


        sleep_ms(1);


        if(absolute_time_diff_us(last_update, get_absolute_time())>4000) {
            printf("LOOP TOOK TOO LONG\n");
            printf("LOOP TOOK TOO LONG\n");
            printf("LOOP TOOK TOO LONG\n");
            printf("LOOP TOOK TOO LONG\n");
            printf("LOOP TOOK TOO LONG\n");
            printf("LOOP TOOK TOO LONG\n");
            printf("LOOP TOOK TOO LONG\n");
            printf("LOOP TOOK TOO LONG\n");
            printf("LOOP TOOK %lld us\n", absolute_time_diff_us(last_update, get_absolute_time()));
            error_checker::fatal_error = true;
        }else{
            //print loop time
        }

        systemPCB::debug_output();

    }
}
