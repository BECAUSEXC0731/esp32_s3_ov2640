#ifndef PIDCONTROL_H
#define PIDCONTROL_H 

#include "Pid.h"


void Pid_control_init();

void Pid_control_run(float TARGET);


class PID_run
{
public:
    PID_run() = default;
   
private:
    int ID_;
    float TARGET_;

public:
    void Pid_run_();
    void Pid_setgoal(int ID,float TARGET);
};

extern PID_run run[2];//实际运行特定的PID
extern PIDController pid_controller[2];//用来赋值PID控制的参数 

#endif
