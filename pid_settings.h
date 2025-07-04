//
// Created by Tim Arnold on 09.12.23.
//

#ifndef FOOSBALLMOTORTEST_PID_SETTINGS_H
#define FOOSBALLMOTORTEST_PID_SETTINGS_H

#include "PID.h"

namespace pid_settings {
    //linear
    ASYMETRIC_PID pid_speed = ASYMETRIC_PID(0.07f, 0, 0.00004f,0.07f,0,0.00008f, 100, 21); // mm/s -> pwm
    PID pid_pos = PID(17.0f, 0, 0.25f, 40000, 2000); // mm -> mm/s

    //rotation
    ASYMETRIC_PID pid_rotation_speed = ASYMETRIC_PID(0.017f, 0, 0.00006f,0.017f,0,0.0002f, 100, 21); // deg/s -> delta pwm //limit 600
    PID pid_rotation = PID(56.0f, 0, 0.20f, 40000, 2000); // deg -> deg/s

}
#endif //FOOSBALLMOTORTEST_PID_SETTINGS_H
