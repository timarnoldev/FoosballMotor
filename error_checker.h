//
// Created by Tim Arnold on 09.12.23.
//

#ifndef FOOSBALLMOTORTEST_ERROR_CHECKER_H
#define FOOSBALLMOTORTEST_ERROR_CHECKER_H

#include "linear_movement/linear_movement.h"

namespace error_checker {
    bool fatal_error = false;
    bool out_of_bounds = false;
    bool encoder_error = false;
    bool encoder_timeout = false;

    void check_for_fatal_errors() {
        if(fatal_error) return;
        if(linear_movement::current_position < (lower_bound-1) || linear_movement::current_position > (upper_bound+1)) {
           // printf("------------------\n");
           // printf("LINEAR MOVEMENT IS OUT OF BOUNDS\n");
           // printf("------------------\n");
            fatal_error = true;
            out_of_bounds = true;
        }

        if(linear_movement::encoder::encoder_errors > 1) {
           // printf("------------------\n");
          //  printf("ENCODER GAVE STRANGE OUTPUT\n");
           // printf("------------------\n");
            fatal_error = true;
            encoder_error = true;
        }

        if(linear_movement::is_moving && absolute_time_diff_us(linear_movement::encoder::last_encoder_response, get_absolute_time()) > 50000) {
            //printf("------------------\n");
           // printf("ENCODER DIDNT RESPOND IN TIME\n");
          //  printf("------------------\n");
            fatal_error = true;
            encoder_timeout = true;
        }
    }
}

#endif //FOOSBALLMOTORTEST_ERROR_CHECKER_H
