//
// Created by Tim Arnold on 09.12.23.
//

#ifndef FOOSBALLMOTORTEST_PID_SETTINGS_H
#define FOOSBALLMOTORTEST_PID_SETTINGS_H

#include "PID.h"

namespace pid_settings {
    //linear //TODO increase P PID_POS to handle small distances better
    ASYMETRIC_PID pid_speed = ASYMETRIC_PID(0.012f, 0, 0.0002f,0.012f,0,0.00075f, 100, 21); // mm/s -> pwm
    PID pid_pos = PID(12.0f, 0, 0.01f, 40000, 2000); // mm -> mm/s

    //Um die Geschwindigkeit zu erhöhen, muss der P-Wert des Positions Controllers erhöht werden, gleichzeitig kann der P Wert des Geschwindigkeits Controllers
    //erhöht werden. Darauf folgt ein Überschwingen, was durch einen höheren P-Wert des Geschwindigkeits-Brems Controllers verringert werden kann.
    //Gleichzeitig kann auch der D-Wert des Geschwindigkeits-Brems Controllers verringert werden.

    //rotation
    ASYMETRIC_PID pid_rotation_speed = ASYMETRIC_PID(0.010f, 0, 0.0001f,0.010f,0,0.00045f, 100, 21); // deg/s -> delta pwm //limit 600
    PID pid_rotation = PID(56.0f, 0, 0.20f, 40000, 2000); // deg -> deg/s

}
#endif //FOOSBALLMOTORTEST_PID_SETTINGS_H
