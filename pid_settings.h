//
// Created by Tim Arnold on 09.12.23.
//

#ifndef FOOSBALLMOTORTEST_PID_SETTINGS_H
#define FOOSBALLMOTORTEST_PID_SETTINGS_H

#include "PID.h"

namespace pid_settings {
    ASYMETRIC_PID pid_speed = ASYMETRIC_PID(0.1f, 0, 0.00028f,0.1f,0,0.00016f, 100, 21); // mm/s -> pwm
    PID pid_pos = PID(56.0f, 0, 0.20f, 40000, 2000); // mm -> mm/s

    ASYMETRIC_PID pid_rotation_speed = ASYMETRIC_PID(0.017f, 0, 0.00006f,0.017f,0,0.0002f, 100, 21); // deg/s -> delta pwm //limit 600
    PID pid_rotation = PID(56.0f, 0, 0.20f, 40000, 2000); // deg -> deg/s

}
#endif //FOOSBALLMOTORTEST_PID_SETTINGS_H
