#ifndef CODE_PID_H_
#define CODE_PID_H_
#include "zf_common_typedef.h"


typedef struct
{
    float p;
    float i;
    float d;
    float dw;
    float out;
    float out_max;
    float p_value_last;
    float i_value;
    float i_value_pro;
    float incremental_data[2];
    float i_value_max;

}pid_cycle_struct;

typedef struct
{
    int16 *gyro_raw_data;           //陀螺仪原始数据
    int16 *acc_raw_data;            //角速度原始数据
    float gyro_ration;              //一阶互补滤波中的角速度滤波信任程度
    float acc_ration;               //一阶互补滤波中的加速度滤波信任程度
    float gyro_ration1;              //一阶互补滤波中的角速度滤波信任程度
    float acc_ration1;               //一阶互补滤波中的加速度滤波信任程度
    float call_cycle  ;              //互补滤波函数的调用周期单位：s（秒）
    float angle_temp;               //互补滤波中保存上一次计算值
    float mechanical_zero;          //机械零点
    float filtering_angle;         //滤波后的角度
}cascade_common_value_struct;

//串级参数结构体
typedef struct
{
     cascade_common_value_struct cascade_value;     //串级参数
     pid_cycle_struct angular_speed_cycle;          //角速度环参数
     pid_cycle_struct angle_cycle;                  //角度环参数
     pid_cycle_struct  speed_cycle;                 //速度环参数
}cascade_value_struct;

extern cascade_value_struct balance_cascade;
extern cascade_value_struct balance_cascade_resave;

extern cascade_value_struct steer_balance_cascade;
extern cascade_value_struct steer_balance_cascade_resave;

void first_order_complementary_filtering(cascade_common_value_struct *filter_value,int16 gyro_raw_data,int16 acc_raw_data);
void pid_control (pid_cycle_struct *pid_cycle,float target,float real);
void pid_control_incremental (pid_cycle_struct *pid_cycle,float target,float real);
void balance_cascade_init (void);
void dynamic_motor_control(void);
void pit_isr_callback(void);

#endif
