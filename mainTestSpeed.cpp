#include <cmath>
#include <cstdio>
#include <hardware/structs/clocks.h>
#include <hardware/clocks.h>
#include <pico/bootrom.h>
#include <hardware/sync.h>
#include <hardware/structs/ioqspi.h>
#include <cmath>
#include "pico/stdlib.h"
#include "pico/time.h"
#include "hardware/irq.h"
#include "hardware/pwm.h"
#include "communication.h"
#include "PID.h"

#define SERVO_PIN 21
#define LED_RED_PIN 13
#define LED_YELLOW_PIN 14
#define LED_BLUE_PIN 15

#define ENCODER_PHASE_A_PIN 11 //GREEN CABLE
#define ENCODER_PHASE_B_PIN 10 //WHITE CABLE

#define MOTOR_ENABLE_FORWARD_PIN 5
#define MOTOR_ENABLE_REVERSE_PIN 8

#define MOTOR_PWM_FORWARD 6 //TO MOTOR
#define MOTOR_PWM_REVERSE 7 //FROM MOTOR

//CLOCKWISE MEANS MOVING TO THE MOTOR!!!


//motor is connected to 17 and 16

void setPulse(uint pin, uint16_t micros) {
    pwm_set_gpio_level(pin, (uint16_t) (((float) clock_get_hz(clk_sys) / 1000.0f * (float) micros / (float) 1000) /
                                        (float) 16));
}

void initServoPWM(uint pin) {

    gpio_set_function(pin, GPIO_FUNC_PWM);

    uint slice_num_16 = pwm_gpio_to_slice_num(pin);

    pwm_config config_16 = pwm_get_default_config();

    pwm_config_set_clkdiv(&config_16, 16.0f);
    pwm_config_set_wrap(&config_16, ((float) clock_get_hz(clk_sys) / 1000 * 4) / (float) 16);
    pwm_init(slice_num_16, &config_16, true);


    setPulse(pin, 1300);

    pwm_set_enabled(slice_num_16, true);

    sleep_ms(100);
}


void initMotor() {
    gpio_init(MOTOR_ENABLE_FORWARD_PIN);
    gpio_init(MOTOR_ENABLE_REVERSE_PIN);

    gpio_set_dir(MOTOR_ENABLE_FORWARD_PIN, true);
    gpio_set_dir(MOTOR_ENABLE_REVERSE_PIN, true);

    gpio_put(MOTOR_ENABLE_FORWARD_PIN, true);
    gpio_put(MOTOR_ENABLE_REVERSE_PIN, true);

    gpio_set_function(MOTOR_PWM_FORWARD, GPIO_FUNC_PWM);
    gpio_set_function(MOTOR_PWM_REVERSE, GPIO_FUNC_PWM);

    uint slice_num = pwm_gpio_to_slice_num(MOTOR_PWM_FORWARD);

    pwm_config config = pwm_get_default_config();

    pwm_config_set_clkdiv(&config, 200.0f);
    pwm_config_set_wrap(&config, 625);
    pwm_init(slice_num, &config, false);

    pwm_set_gpio_level(MOTOR_PWM_FORWARD, 0); //clockwise
    pwm_set_gpio_level(MOTOR_PWM_REVERSE, 0); //counterclockwise


    pwm_set_enabled(slice_num, true);

}

bool is_phase_a_high = false;
bool is_phase_b_high = false;

int current_rotation = 0;
int last_step = 0;

bool changed = false;

int encoder_errors = 0;

uint measured_pulse_delta_time = 0;

absolute_time_t last_pulse_time;

void encoder_irq(uint gpio, uint32_t events) {

    if (gpio == ENCODER_PHASE_A_PIN) { //clockwise
        if (gpio_get(gpio)) {
            if(!is_phase_a_high) {
                changed = true;
            }
            is_phase_a_high = true;
        } else if(!gpio_get(gpio)){
            if(is_phase_a_high) {
                changed = true;
            }
            is_phase_a_high = false;
        }
    }

    if (gpio == ENCODER_PHASE_B_PIN) { //counterclockwise
        if (gpio_get(gpio)) {
            if(!is_phase_b_high) {
                changed = true;
            }
            is_phase_b_high = true;
        } else if(!gpio_get(gpio)) {
            if(is_phase_b_high) {
                changed = true;
            }
            is_phase_b_high = false;
        }
    }

   if(changed) {
       if(!is_phase_a_high&&is_phase_b_high){
           if(last_step == 1) {
               current_rotation--;
           }else if(last_step == 3) {
               current_rotation++;
           }else{
               encoder_errors++;
           }
           //STEP 0
           last_step = 0;

           last_pulse_time = get_absolute_time();
       }

       if(!is_phase_a_high&&!is_phase_b_high){
           if(last_step == 2) {
               current_rotation--;
           }else if(last_step==0){
               current_rotation++;
           }else{
               encoder_errors++;
           }

           //STEP 1
           last_step = 1;
       }

       if(is_phase_a_high&&!is_phase_b_high){
           if(last_step == 3) {
               current_rotation--;
           }else if(last_step == 1) {
               current_rotation++;
           }else{
               encoder_errors++;
           }
           //STEP 2
           last_step = 2;
       }

       if(is_phase_a_high&&is_phase_b_high){
           if(last_step == 0) {
               current_rotation--;
           }else if(last_step == 2) {
               current_rotation++;
           }else{
               encoder_errors++;
           }
           //STEP 3
           last_step = 3;

           measured_pulse_delta_time = absolute_time_diff_us(last_pulse_time, get_absolute_time());
       }
       changed = false;
   }else{
      // encoder_errors++;
   }

}

bool fatal_error = false;
bool is_in_safe_zone_bottom = false;
bool is_in_safe_zone_top = false;

float real_speed = 0;


int main() {
    stdio_init_all();

    sleep_ms(1000);

    initServoPWM(SERVO_PIN);


    gpio_init(LED_RED_PIN);
    gpio_init(LED_YELLOW_PIN);
    gpio_init(LED_BLUE_PIN);

    gpio_set_dir(LED_RED_PIN, GPIO_OUT);
    gpio_set_dir(LED_YELLOW_PIN, GPIO_OUT);
    gpio_set_dir(LED_BLUE_PIN, GPIO_OUT);


    gpio_set_irq_enabled(ENCODER_PHASE_A_PIN, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true);
    gpio_set_irq_enabled(ENCODER_PHASE_B_PIN, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true);

    irq_set_priority(IO_IRQ_BANK0,256u);

    gpio_set_irq_callback(&encoder_irq);

    irq_set_enabled(IO_IRQ_BANK0, true);

    initMotor();

    Communication com = Communication();

    absolute_time_t last_packet = get_absolute_time();

    absolute_time_t time_since_error_is_zero = get_absolute_time();
    bool error_is_zero = false;

    absolute_time_t timer = get_absolute_time();

    int should_position = 60;//60;
    int should_speed = 0;//60;

    PID pid_speed = PID(1, 0, 0, 300, 21);
    //PID pid = PID(8, 200, 0.015, 100, 21);
    int real_output = 0;

    sleep_ms(4000);

    bool logged = false;

    while (true) {
        //printf("Hello, world!\n");
        float current_position = current_rotation / 2400.0f * 4.0f * 15.0f;
        int current_speed = 0;

        //float output = pid.calculatePID((float) should_position, current_position, 0.001);
        //output = output + 0.5 - (output < 0);
       // int output_int = (int) output;

      /*  if (output > 0) {

            if(is_in_safe_zone_top) {
                real_output = fmin(50,abs(output_int));
            }else{
                real_output = abs(output_int);
            }

            pwm_set_gpio_level(MOTOR_PWM_FORWARD, real_output);
            pwm_set_gpio_level(MOTOR_PWM_REVERSE, 0);
        } else {

            if(is_in_safe_zone_bottom) {
                real_output = -fmin(50,abs(output_int));
            }else{
                real_output = -abs(output_int);
            }

            pwm_set_gpio_level(MOTOR_PWM_REVERSE, -real_output);
            pwm_set_gpio_level(MOTOR_PWM_FORWARD, 0);
        }
        */

      float rotations_per_second = 1.0f/(((float) measured_pulse_delta_time/1.0e6f) * 600.0f)*4.0f;
      if(measured_pulse_delta_time==0) {
          rotations_per_second = 0;
      }
      real_speed = rotations_per_second * 0.9f + real_speed * 0.1f;
      gpio_put(MOTOR_ENABLE_FORWARD_PIN, true);
      pwm_set_gpio_level(MOTOR_PWM_FORWARD, should_speed);

        if(fatal_error) {
           /* pwm_set_gpio_level(MOTOR_PWM_FORWARD, 0);
            pwm_set_gpio_level(MOTOR_PWM_REVERSE, 0);
            gpio_put(LED_RED_PIN, true);
            gpio_put(MOTOR_ENABLE_FORWARD_PIN, false);
            gpio_put(MOTOR_ENABLE_REVERSE_PIN, false);
            */
        }

        if(std::floor(std::abs(pid_speed.error))==0.0) { //reset pid to avoid motor beep
            if(!error_is_zero){
                time_since_error_is_zero = get_absolute_time();
                error_is_zero = true;
            }

            if(absolute_time_diff_us(time_since_error_is_zero, get_absolute_time()) > 1000000){
                pid_speed.reset();
            }
        }else{
            error_is_zero = false;
        }


        if(current_position<30) {
            is_in_safe_zone_bottom = true;
        }else{
            is_in_safe_zone_bottom = false;
        }

        if(current_position>230) {
            is_in_safe_zone_top = true;
        }else{
            is_in_safe_zone_top = false;
        }

        if(is_in_safe_zone_top || is_in_safe_zone_bottom) {
           // gpio_put(MOTOR_ENABLE_FORWARD_PIN, false);
          //  gpio_put(MOTOR_ENABLE_REVERSE_PIN, false);
        }else{
           // gpio_put(MOTOR_ENABLE_FORWARD_PIN, true);
           // gpio_put(MOTOR_ENABLE_REVERSE_PIN, true);
        }

        //gpio_put(LED_YELLOW_PIN, is_in_safe_zone_top || is_in_safe_zone_bottom);
        //gpio_put(LED_YELLOW_PIN, is_in_safe_zone_top );
        //gpio_put(LED_BLUE_PIN,  is_in_safe_zone_bottom);



        if(current_position < -1 || current_position > 246) {
            fatal_error = true;
        }

        if(encoder_errors > 1) {
            fatal_error = true;
        }

        if(absolute_time_diff_us(timer, get_absolute_time()) > 90000) {
            if(!logged) {
                printf("%f %d\n", real_speed, should_speed);
                logged = true;
            }
        }

        if(absolute_time_diff_us(timer, get_absolute_time()) > 100000) {
            timer = get_absolute_time();
            should_speed++;
            logged = false;

            if(should_speed==626) {
                gpio_put(LED_BLUE_PIN,  true);
                gpio_put(LED_YELLOW_PIN,  true);
                while(true);
            }

        }

        /*if(absolute_time_diff_us(timer, get_absolute_time()) > 1000000){
            timer = get_absolute_time();

            if(should_position == 200){
                should_position = 60;
            }else if(should_position==60) {
                should_position = 120;
            }else if(should_position == 120) {
                should_position = 200;
            }

        }
         */



        int n = getchar_timeout_us(0);

        if (n != PICO_ERROR_TIMEOUT) {
            last_packet = get_absolute_time();
            com.readData(n);
        }


      /*  if (absolute_time_diff_us(last_packet, get_absolute_time()) > 1000000) {
            gpio_put(LED_YELLOW_PIN, true);
        } else {
            gpio_put(LED_YELLOW_PIN, false);
        }
        */

        //printf("Position: %f, %d, %d, EE: %d, b: %d, t: %d\n", current_position, real_output, should_position, encoder_errors, is_in_safe_zone_bottom, is_in_safe_zone_top); //600 per rotation 1:4 -> 15mm

        //printf("Rotation: %d\n", current_rotation);

        sleep_ms(1);


    }
    return 0;
}
