

#ifndef CODE_STEER_H_
#define CODE_STEER_H_


#include"zf_common_headfile.h"
#define STEER_4_PWM           ( ATOM1_CH7_P02_7)         //舵机控制引脚  注：右下舵机 硅胶车
#define STEER_4_FRE           (300)                    //舵机控制频率
#define STEER_4_DIR          (1)                       //舱机旋转方向（车体升高方向）
#define STEER_4_CENTER       (5400)                    //舵机中心值（初始保持位置）

#define STEER_3_PWM         (ATOM1_CH4_P02_4)          //舵机控制引脚 注：左后舵机
#define STEER_3_FRE          (300)                     //舵机控制频率
#define STEER_3_DIR            (-1)                         //舵机旋转方向（车体升高方向）
#define STEER_3_CENTER           (3700)                        //舱机中心值（初始保持位置）

#define STEER_2_PWM         (ATOM1_CH5_P02_5)           //舵机控制引脚    注：右上舵机
#define STEER_2_FRE           (300)                            //舵机控制频率
#define STEER_2_DIR            (-1)                         //舵机旋转方向（车体升高方向）
#define STEER_2_CENTER           (4200)                        //舱机中心值（初始保持位置）

#define STEER_1_PWM         (ATOM1_CH6_P02_6)           //舵机控制引脚    注：左前舵机
#define STEER_1_FRE           (300)                            //舵机控制频率
#define STEER_1_DIR             (1)                        //舵机旋转方向（车体升高方向）
#define STEER_1_CENTER           (5280)                        //舱机中心值（初始保持位置）

//#define STEER_4_PWM           ( ATOM1_CH7_P02_7)         //舵机控制引脚  注：右下舵机
//#define STEER_4_FRE           (300)                    //舵机控制频率
//#define STEER_4_DIR          (1)                       //舱机旋转方向（车体升高方向）
//#define STEER_4_CENTER       (4900)                    //舵机中心值（初始保持位置）
//
//#define STEER_3_PWM         (ATOM1_CH4_P02_4)          //舵机控制引脚 注：左后舵机
//#define STEER_3_FRE          (300)                     //舵机控制频率
//#define STEER_3_DIR            (-1)                         //舵机旋转方向（车体升高方向）
//#define STEER_3_CENTER           (3700)                        //舱机中心值（初始保持位置）
//
//#define STEER_2_PWM         (ATOM1_CH5_P02_5)           //舵机控制引脚    注：右上舵机
//#define STEER_2_FRE           (300)                            //舵机控制频率
//#define STEER_2_DIR            (-1)                         //舵机旋转方向（车体升高方向）
//#define STEER_2_CENTER           (3950)                        //舱机中心值（初始保持位置）
//
//#define STEER_1_PWM         (ATOM1_CH6_P02_6)           //舵机控制引脚    注：左前舵机
//#define STEER_1_FRE           (300)                            //舵机控制频率
//#define STEER_1_DIR             (1)                        //舵机旋转方向（车体升高方向）
//#define STEER_1_CENTER           (5350)                        //舱机中心值（初始保持位置）
typedef struct
{
   pwm_channel_enum         pwm_pin;                               //舵机控制引脚
   int16                    control_frequency;                  //舵机控制频率
   int16                    steer_dir;                          //舵机旋转方向
   int16                    center_num;                         //舵机中心值（初始保持位置）
   int16                    now_location;                       //舵机当前输出位置
}steer_control_struct;

//笔记：中心位置为四个腿臂平行于地面高度为2400  最高高度为5400达到最高高度时，四个腿臂垂直于地面
extern steer_control_struct steer_1;
extern steer_control_struct steer_2;
extern steer_control_struct steer_3;
extern steer_control_struct steer_4;
void steer_control_init(void);
void steer_duty_set(steer_control_struct *control_data,int16 duty);
void steer_control(steer_control_struct *control_data,int16 move_num);
void dynamic_steer_control(void);

#endif /* CODE_STEER_CONTROL_H_ */
