#include <Arduino.h>
#include "Pid.h"

// 构造函数传入PID参数
PIDController::PIDController(float Kp, float Ki, float Kd)
{
    Kp_ = Kp;
    Ki_ = Ki;
    Kd_ = Kd;
}

// 设置目标值
void PIDController::Set_Target(float target)
{
    target_ = target;
}

// 计算PID输出
float PIDController::update(float current)
{
    error_ = target_ - current; // 计算误差的积分，并限幅
    error_sum_ += error_;
    if (error_sum_ > intergral_up_)
        error_sum_ = intergral_up_;
    if (error_sum_ < -1 * intergral_up_)
        error_sum_ = -1 * intergral_up_;

    derror_ = error_last_ - error_; // 记录误差的变化率
    error_last_ = error_;           // 更新误差

    float output = Kp_ * error_ + Ki_ * error_sum_ + Kd_ * derror_;

    if (output > output_up)
        output = output_up; // 输出值限幅
    else if (output < output_down)
        output = output_down;
    return output;
}

// 初始化设置参数
void PIDController::Set_Pid(float Kp, float Ki, float Kd)
{
    Kp_ = Kp;
    Ki_ = Ki;
    Kd_ = Kd;
}

// 充值参数
void PIDController::Reset()
{
    Kp_, Ki_, Kd_ = 0;
    error_, error_last_, error_sum_ = 0;
}

// 设置输出限幅的值
void PIDController::Out_limit(float max, float min)
{
    output_down = min;
    output_up = max;
}