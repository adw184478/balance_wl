#include "zf_common_headfile.h"
float pitching_angle,roll_angle;                //数据融合后的角度
float acc_ratio1 =1.5,gyro_ratio1 = 1.8;
float acc_ratio2 =1.2,gyro_ratio2 = 1.6;
float dt = 0.005;           //采样周期(不要更改）
//----------------------------------------------------------------
//  @brief      一阶互补滤波
//  @param      angle_m     加速度计数据
//  @param      gyro_m      陀螺仪数据
//  @return     float       数据融合后的角度
//----------------------------------------------------------------
float angle_calc1(float angle_m, float gyro_m)
{
    float temp_angle;
    float gyro_now;
    float error_angle;
    static float last_angle;
    static uint8 first_angle;
    if(!first_angle)//判断是否为第一次运行本函数
    {
        //如果是第一次运行，则将上次角度值设置为与加速度值一致
        first_angle = 1;
        last_angle = angle_m;
    }
    gyro_now = gyro_m * gyro_ratio1;
    //根据测量到的加速度值转换为角度之后与上次的角度值求偏差
    error_angle = (angle_m - last_angle)*acc_ratio1;
    //根据偏差与陀螺仪测量得到的角度值计算当前角度值
    temp_angle = last_angle + (error_angle + gyro_now)*dt;
    //保存当前角度值
    last_angle = temp_angle;
    return temp_angle;
}

float angle_calc2(float angle_m, float gyro_m)
{
    float temp_angle;
    float gyro_now;
    float error_angle;
    static float last_angle;
    static uint8 first_angle;
    if(!first_angle)//判断是否为第一次运行本函数
    {
        //如果是第一次运行，则将上次角度值设置为与加速度值一致
        first_angle = 1;
        last_angle = angle_m;
    }
    gyro_now = gyro_m * gyro_ratio2;
    //根据测量到的加速度值转换为角度之后与上次的角度值求偏差
    error_angle = (angle_m - last_angle)*acc_ratio2;
    //根据偏差与陀螺仪测量得到的角度值计算当前角度值
    temp_angle = last_angle + (error_angle + gyro_now)*dt;
    //保存当前角度值
    last_angle = temp_angle;
    return temp_angle;
}

void updata_angle(void)
{
        imu660ra_get_gyro             ();//获取陀螺仪数据
        imu660ra_get_acc              ();//获取加速度数据
        pitching_angle = angle_calc1(imu660ra_acc_y, imu660ra_gyro_x)/67.03; //俯仰角
        roll_angle=-angle_calc2(-imu660ra_acc_x, imu660ra_gyro_y)/67.03;     //翻滚角
}
