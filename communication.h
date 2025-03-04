//
// Created by Tim Arnold on 28.10.23.
//

#ifndef FOOSBALLMOTORTEST_COMMUNICATION_H
#define FOOSBALLMOTORTEST_COMMUNICATION_H

#include <pico/bootrom.h>
#include <cstdio>
#include <string>

// $RRRR,MMMM#



namespace communication {
    bool inDataline;
    int data_counter = 0;
    absolute_time_t last_packet = get_absolute_time();
    int current_rotation = 1500;
    std::string received_rotation;
    std::string received_position;
    int current_position = 0;
    bool mounted = false;
    bool connected = false;

    bool is_mounted() {
        return mounted;
    }

    bool is_connected() {
        return connected;
    }

    void readData() {

        int data = getchar_timeout_us(0);

        if (data == PICO_ERROR_TIMEOUT) {
            return;
        }

        last_packet = get_absolute_time();

        if (data == '!') {
            reset_usb_boot(0, 0);
        }

        if (data == '$') {
            inDataline = true;
            return;
        }

        if (inDataline) {

            if (data == '#') {
                if (data_counter != 9) {
                    printf("Error: Overflow\n");
                }else{
                    //current_rotation = std::stoi(received_rotation);
                    //current_position = std::stoi(received_position);
                    //printf("Rotation: %d\n", current_rotation);
                    //printf("Position: %d\n", current_position);
                }

                inDataline = false;
                data_counter = 0;
                received_rotation = "";
                received_position = "";
                return;
            }

            if (data_counter < 4) {
                received_rotation += (char) data;
            }

            if (data_counter > 4 && data_counter < 9) {
                received_position += (char) data;
            }

            if(data_counter > 8) {
                printf("Error: Overflow\n");
                inDataline = false;
                data_counter = 0;
                received_rotation = "";
                received_position = "";
                return;
            }

            data_counter++;

        }


    }



}






#endif //FOOSBALLMOTORTEST_COMMUNICATION_H
