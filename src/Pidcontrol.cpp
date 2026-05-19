// #include "Pidcontrol.h"


// #define KP1 0.30
// #define KI1 0.07
// #define KD1 0.1

// #define KP2 0.3
// #define KI2 0.05
// #define KD2 0.05

// #define LIMIT 100

// PIDController pid_controller[2];
// void Pid_controller_init()
// {

//     pid_controller[0].Set_Pid(KP1, KI1, KD1);
//     pid_controller[1].Set_Pid(KP2, KI2, KD2);
    
//     pid_controller[0].Out_limit(LIMIT, -LIMIT);
//     pid_controller[1].Out_limit(LIMIT, -LIMIT);
   
// }

// void PID_run::Pid_run_()//-------------------------------------运行定电机PID
// {
//         //控制舵机运动的函数
// }

// void PID_run::Pid_setgoal(int ID, float TARGET)//给PID控制器设置目标值
// {
//     ID_ = ID - 1;
//     TARGET_ = TARGET;
//     pid_controller[ID_].Set_Target(TARGET_);
// }