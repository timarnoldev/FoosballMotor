//
// Created by Tim Arnold on 02.01.24.
//

#ifndef FOOSBALLMOTORTEST_SECONDCOREENTRY_H
#define FOOSBALLMOTORTEST_SECONDCOREENTRY_H

#include "communication.h"

void secondCoreEntry() {
    while(true) {
        communication::readData();
    }
};
#endif //FOOSBALLMOTORTEST_SECONDCOREENTRY_H
