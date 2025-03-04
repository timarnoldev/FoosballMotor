//
// Created by Tim Arnold on 09.12.23.
//

#ifndef FOOSBALLMOTORTEST_PID_SETTINGS_H
#define FOOSBALLMOTORTEST_PID_SETTINGS_H

#include "PID.h"

namespace pid_settings {
    PID pid_speed = PID(2.6f, 0, 0.003f, 500, 21); // mm/s -> pwm //limit 600
    PID pid_pos = PID(15, 0, 0.09, 400, 21); // mm -> mm/s

    PID pid_rotation_speed = PID(0.005f, 0, 0.003f, 100, 21); // deg/s -> delta pwm //limit 600

    NON_LINEAR_PID pid_rotation = NON_LINEAR_PID(3, 0, 0.00f, 4000, 2000); // deg -> deg/s

}
#endif //FOOSBALLMOTORTEST_PID_SETTINGS_H
