//
// Created by Tim Arnold on 09.12.23.
//


#ifndef FOOSBALLMOTORTEST_CONSTANTS_H
#define FOOSBALLMOTORTEST_CONSTANTS_H



//-------SELECTION -------

// 0: goal keeper
// 1: defender
// 2: mid fielder
// 3: striker

#define DEVICE_POSITION 0

//-------SELECTION END -------


#define SERVO_PIN 21
#define LED_RED_PIN 13
#define LED_YELLOW_PIN 14
#define LED_BLUE_PIN 15
#if DEVICE_POSITION == 0
    #define lower_bound 0
    #define upper_bound 246
#else
    #error "Device position not defined"
#endif

#endif //FOOSBALLMOTORTEST_CONSTANTS_H
