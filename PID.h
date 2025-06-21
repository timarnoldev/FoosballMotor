//
// Created by felix & tim on 13.03.21.
//

#ifndef PICODRONE_PID_H
#define PICODRONE_PID_H
#include <math.h>

typedef struct {
    float gain_P;
    float gain_I;
    float gain_D;
    float total_limit;
    float integral_limit;
} pid_config_t;

typedef struct {
    float integral;
    float last_error;
    float error;
    float P;
    float I;
    float D;
    float out;
} pid_workingdata_t;



class nPID
{
public:
    pid_config_t *pid_config;
    pid_workingdata_t pid_workingdata;

    nPID(pid_config_t &pid_config){
        this->pid_config = &pid_config;
    }

    float calculatePID(float soll, float ist,float loopIntervalTime){

        pid_workingdata.error=soll-ist;

        //proportional term
        pid_workingdata.P=((pid_config->gain_P) * pid_workingdata.error);

        if(pid_workingdata.last_error!=0) { //if we have an error equal to zero we have performed a reset. We avoid calculating D because of unrealistic high values
            pid_workingdata.D=((pid_config->gain_D) * ((pid_workingdata.error - pid_workingdata.last_error)/loopIntervalTime));
        }else{
            pid_workingdata.D = 0;
        }

        //TODO use low pass filter for D
        //https://github.com/ArduPilot/ardupilot/blob/c83774a7cd83929b90c772beec268d0fc1bf94d2/libraries/PID/PID.cpp
        //TODO use scaler for D and P

        //integral Term
        pid_workingdata.integral += (pid_config->gain_I)*pid_workingdata.error*loopIntervalTime;
        pid_workingdata.I = pid_workingdata.integral;
        //integral limitation
        if (pid_workingdata.I > pid_config->integral_limit)pid_workingdata.I = pid_config->integral_limit;
        else if (pid_workingdata.I < pid_config->integral_limit * -1) pid_workingdata.I = pid_config->integral_limit * -1;


        pid_workingdata.out = pid_workingdata.P + pid_workingdata.I + pid_workingdata.D;

        if (pid_workingdata.out > pid_config->total_limit)pid_workingdata.out = pid_config->total_limit;
        else if (pid_workingdata.out < pid_config->total_limit * -1)pid_workingdata.out = pid_config->total_limit * -1;
        pid_workingdata.last_error = pid_workingdata.error;
        return pid_workingdata.out;
    }
    void reset(){
        pid_workingdata.integral=0;
        pid_workingdata.last_error=0;
    }
};


class ASYMETRIC_PID
{
public:
    float * Pgain=0;
    float * Igain=0;
    float * Dgain=0;

    float PGainValue = 0;
    float IGainValue = 0;
    float DGainValue = 0;
    
    float PGainBValue = 0;
    float IGainBValue = 0;
    float DGainBValue = 0;

    float  Iteil=0;
    float  integral=0;
    float lasterror=0;
    float  error=0;
    float  out=0;
    float max=0;
    float Imax=0;

    float Pteil=0;
    float Dteil=0;

  
    ASYMETRIC_PID(float Pgain, float Igain, float Dgain, float PgainB, float IgainB, float DgainB, float max, float Imax){
        this->PGainValue = Pgain;
        this->IGainValue = Igain;
        this->DGainValue = Dgain;

        this->PGainBValue = PgainB;
        this->IGainBValue = IgainB;
        this->DGainBValue = DgainB;
        

        this->Pgain=new float (Pgain);
        this->Igain=new float (Igain);
        this->Dgain=new float (Dgain);
        this->max=max;
        this->Imax=Imax;
    }

    //toggle is true if speed is positiv
    float calculatePID(float soll, float ist,float loopIntervalTime, bool toggle){

        error=soll-ist;

        if(toggle==(error > 0)) {
            (*Pgain) = PGainValue;
            (*Igain) = IGainValue;
            (*Dgain) = DGainValue;
        }else{
            (*Pgain) = PGainBValue;
            (*Igain) = IGainBValue;
            (*Dgain) = DGainBValue;
        }

        //proportional term
        Pteil=((*Pgain) * error);

        if(lasterror!=0) { //if we have an error equal to zero we have performed a reset. We avoid calculating D because of unrealistic high values
            Dteil=((*Dgain) * ((error - lasterror)/loopIntervalTime));
        }else{
            Dteil = 0;
        }

        //TODO use low pass filter for D
        //https://github.com/ArduPilot/ardupilot/blob/c83774a7cd83929b90c772beec268d0fc1bf94d2/libraries/PID/PID.cpp
        //TODO use scaler for D and P

        //integral Term
        integral += (*Igain)*error*loopIntervalTime;
        Iteil = integral;
        //integral limitation
        if (Iteil > Imax)Iteil = Imax;
        else if (Iteil < Imax * -1) Iteil = Imax * -1;


        out = Pteil + Iteil + Dteil;

        if (out > max)out = max;
        else if (out < max * -1)out = max * -1;
        lasterror = error;
        return out;
    }
    void reset(){
        integral=0;
        lasterror=0;
    }


};



class PID
{
public:
    float * Pgain=0;
    float * Igain=0;
    float * Dgain=0;
    float  Iteil=0;
    float  integral=0;
    float lasterror=0;
    float  error=0;
    float  out=0;
    float max=0;
    float Imax=0;

    float Pteil=0;
    float Dteil=0;

    PID(float * Pgain, float * Igain, float * Dgain){
        this->Pgain=Pgain;
        this->Igain=Igain;
        this->Dgain=Dgain;
    }
    PID(float *  Pgain, float  * Igain, float *  Dgain, float   max, float  Imax){
        this->Pgain=Pgain;
        this->Igain=Igain;
        this->Dgain=Dgain;
        this->max=max;
        this->Imax=Imax;
    }
    PID(float Pgain, float Igain, float Dgain, float max, float Imax){
        this->Pgain=new float (Pgain);
        this->Igain=new float (Igain);
        this->Dgain=new float (Dgain);
        this->max=max;
        this->Imax=Imax;
    }


    float calculatePID(float soll, float ist,float loopIntervalTime){

        error=soll-ist;

        //proportional term
        Pteil=((*Pgain) * error);

        if(lasterror!=0) { //if we have an error equal to zero we have performed a reset. We avoid calculating D because of unrealistic high values
            Dteil=((*Dgain) * ((error - lasterror)/loopIntervalTime));
        }else{
            Dteil = 0;
        }

        //TODO use low pass filter for D
        //https://github.com/ArduPilot/ardupilot/blob/c83774a7cd83929b90c772beec268d0fc1bf94d2/libraries/PID/PID.cpp
        //TODO use scaler for D and P

        //integral Term
        integral += (*Igain)*error*loopIntervalTime;
        Iteil = integral;
        //integral limitation
        if (Iteil > Imax)Iteil = Imax;
        else if (Iteil < Imax * -1) Iteil = Imax * -1;


        out = Pteil + Iteil + Dteil;

        if (out > max)out = max;
        else if (out < max * -1)out = max * -1;
        lasterror = error;
        return out;
    }
    void reset(){
        integral=0;
        lasterror=0;
    }


};

class NON_LINEAR_PID
{
public:
    float * Pgain=0;
    float * Igain=0;
    float * Dgain=0;
    float  Iteil=0;
    float  integral=0;
    float lasterror=0;
    float  error=0;
    float  out=0;
    float max=0;
    float Imax=0;

    float Pteil=0;
    float Dteil=0;

    NON_LINEAR_PID(float * Pgain, float * Igain, float * Dgain){
        this->Pgain=Pgain;
        this->Igain=Igain;
        this->Dgain=Dgain;
    }
    NON_LINEAR_PID(float *  Pgain, float  * Igain, float *  Dgain, float   max, float  Imax){
        this->Pgain=Pgain;
        this->Igain=Igain;
        this->Dgain=Dgain;
        this->max=max;
        this->Imax=Imax;
    }
    NON_LINEAR_PID(float Pgain, float Igain, float Dgain, float max, float Imax){
        this->Pgain=new float (Pgain);
        this->Igain=new float (Igain);
        this->Dgain=new float (Dgain);
        this->max=max;
        this->Imax=Imax;
    }



    static float function(const float x)
    {
        return powf(M_E, -powf(x,2))+1;
    }

    float calculatePID(float soll, float ist,float loopIntervalTime){

        error=soll-ist;

        //proportional term
        Pteil=((*Pgain) * error)*(function(fabsf(error/5.0f)));

        if(lasterror!=0) { //if we have an error equal to zero we have performed a reset. We avoid calculating D because of unrealistic high values
            Dteil=((*Dgain) * ((error - lasterror)/loopIntervalTime));
        }else{
            Dteil = 0;
        }

        //TODO use low pass filter for D
        //https://github.com/ArduPilot/ardupilot/blob/c83774a7cd83929b90c772beec268d0fc1bf94d2/libraries/PID/PID.cpp
        //TODO use scaler for D and P

        //integral Term
        integral += (*Igain)*error*loopIntervalTime;
        Iteil = integral;
        //integral limitation
        if (Iteil > Imax)Iteil = Imax;
        else if (Iteil < Imax * -1) Iteil = Imax * -1;


        out = Pteil + Iteil + Dteil;

        if (out > max)out = max;
        else if (out < max * -1)out = max * -1;
        lasterror = error;
        return out;
    }
    void reset(){
        integral=0;
        lasterror=0;
    }
};
#endif //PICODRONE_PID_H