//
// Created by Tim Arnold on 09.12.23.
//

#ifndef FOOSBALLMOTORTEST_PID_SETTINGS_H
#define FOOSBALLMOTORTEST_PID_SETTINGS_H

#include "PID.h"

namespace pid_settings {
    PID pid_speed = PID(2.6f, 0, 0.003f, 500, 21); // mm/s -> pwm //limit 600
    PID pid_pos = PID(15, 0, 0.09, 400, 21); // mm -> mm/s

    ASYMETRIC_PID pid_rotation_speed = ASYMETRIC_PID(0.017f, 0, 0.00006f,0.017f,0,0.0002f, 100, 21); // deg/s -> delta pwm //limit 600
    PID pid_rotation = PID(56.0f, 0, 0.20f, 40000, 2000); // deg -> deg/s

}
#endif //FOOSBALLMOTORTEST_PID_SETTINGS_H
