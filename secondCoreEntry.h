//
// Created by Tim Arnold on 02.01.24.
//

#ifndef FOOSBALLMOTORTEST_SECONDCOREENTRY_H
#define FOOSBALLMOTORTEST_SECONDCOREENTRY_H

#include "communication.h"

void secondCoreEntry() {
    while(true) {
        communication::readData();
        sleep_ms(1);
       // printf("%f %f %f %d %f\n",linear_movement::current_position, linear_movement::should_speed, linear_movement::current_speed, linear_movement::should_pwm, linear_movement::should_position);
        printf("%f %f %f %d %f\n",rotation::movement::current_rotation,rotation::movement::should_rotation_speed, rotation::movement::current_rotation_speed, rotation::movement::should_pwm, rotation::movement::should_rotation);
    }
};
#endif //FOOSBALLMOTORTEST_SECONDCOREENTRY_H
