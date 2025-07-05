
#include <cstdio>
#include <pico/multicore.h>
//#include "tusb.h"
#include "pico/stdlib.h"
#include "pico/time.h"
//#include "communication.h"
#include "linear_movement/linear_movement.h"
#include "error_checker.h"
#include "system/system.h"
#include "secondCoreEntry.h"
#include "encoder.h"
#include "rotation/rotative_movement.h"
//#include "tusb_config.h"


absolute_time_t last_communication = get_absolute_time();

/*
// Invoked when device is mounted
void tud_mount_cb(void)
{
    communication::mounted = true;
}

// Invoked when device is unmounted
void tud_umount_cb(void)
{
    communication::mounted = false;
}

void tud_suspend_cb(bool remote_wakeup_en)
{
    (void) remote_wakeup_en;

}

// Invoked when usb bus is resumed
void tud_resume_cb(void)
{

}

uint16_t tud_hid_get_report_cb(uint8_t itf, uint8_t report_id, hid_report_type_t report_type, uint8_t* buffer, uint16_t reqlen)
{
    // TODO not Implemented
    (void) itf;
    (void) report_id;
    (void) report_type;
    (void) buffer;
    (void) reqlen;

    return 0;
}

// Invoked when received SET_REPORT control request or
// received data on OUT endpoint ( Report ID = 0, Type = 0 )
void tud_hid_set_report_cb(uint8_t itf, uint8_t report_id, hid_report_type_t report_type, uint8_t const* buffer, uint16_t bufsize)
{
    // This example doesn't use multiple report and report ID
    (void) itf;
    (void) report_id;
    (void) report_type;

    // HEADER | STATUS BYTE | POSITION 1 | POSITION 2 | POSITION 3 | POSITION 4 | ROTATION 1 | ROTATION 2 | END

    if(buffer[0] == 0x54)
    {
        rom_reset_usb_boot(0,0);
        return;
    }

    if(bufsize != 9) return;

    if(buffer[0] != 0x02) return;

    if(buffer[8] != 0x04) return;

    uint8_t status = buffer[1];

    union data_holder
    {
        float f;
        uint8_t b[4];
    } data{};

    memcpy(data.b, buffer+2, 4);

    linear_movement::set_should_position(data.f);

    //rotation little endian
    int rotation = buffer[6] | (buffer[7] << 8);

    last_communication = get_absolute_time();

    rotation::setPulse(rotation);

}

void send_host_report()
{

    if(!tud_mounted()) return;

    union data_holder
    {
        float f;
        uint8_t b[4];
    } data;

    uint8_t * buffer = new uint8_t[7];
    buffer[0] = 0x02; // HEADER | STATUS BYTE | POSITION 1 | POSITION 2 | POSITION 3 | POSITION 4 | END
    buffer[1] = error_checker::fatal_error | (error_checker::out_of_bounds << 1) | (error_checker::encoder_error << 2) | (error_checker::encoder_timeout << 3) | (linear_movement::is_in_safe_zone_top << 4) | (linear_movement::is_in_safe_zone_bottom << 5);
    data.f = linear_movement::current_position;
    memcpy(buffer+2, data.b, 4); // POSITION 1 | POSITION 2 | POSITION 3 | POSITION 4
    buffer[6] = 0x04;

    tud_hid_report(0, buffer, 7);

    delete [] buffer;

}

*/

int main() {
    stdio_init_all();
  //  tud_init(BOARD_TUD_RHPORT);

    sleep_ms(4000);

    //rotation::initServo();
    systemPCB::initPCB();
    systemPCB::ledTest();

    encoder::init_encoder();
    rotation::movement::initMotor();

    linear_movement::initMotor();

    multicore_launch_core1(secondCoreEntry);

    absolute_time_t last_update = get_absolute_time();

    while (true) {
        last_update = get_absolute_time();
        rotation::movement::calculate_state();
        rotation::movement::calculate_motor_pwm();
        rotation::movement::apply_motor_pwm();

        linear_movement::calculate_state();
        linear_movement::calculate_rotation_compensation();
        linear_movement::bounds_safety();
        linear_movement::calculate_motor_pwm();
        linear_movement::apply_motor_pwm();

        error_checker::check_for_fatal_errors();


        if (error_checker::fatal_error) {
          //  linear_movement::emergency();
        }

        systemPCB::updateLEDSignals();

        if (!rotation::movement::emergency_stop)
        {
            printf("%f %f %f %d %f\n",linear_movement::current_position, linear_movement::should_speed, linear_movement::current_speed, linear_movement::should_pwm, linear_movement::should_position);

        }


        //tud_task();

      //  send_host_report();

        sleep_ms(1);

        //communication::connected = absolute_time_diff_us(last_communication, get_absolute_time())<1000000;


        if(absolute_time_diff_us(last_update, get_absolute_time())>4000) {
            error_checker::fatal_error = true;
        }

        systemPCB::debug_output();

    }
}
