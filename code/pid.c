#include "zf_common_headfile.h"

cascade_value_struct balance_cascade;
cascade_value_struct balance_cascade_resave;

cascade_value_struct steer_balance_cascade;
cascade_value_struct steer_balance_cascade_resave;

//--------
//函数简介          一阶互补滤波
//参数说明      filter_value    滤波参数结构体
//参数说明      gyro_raw_data   陀螺仪原始数据
//参数说明      acc_raw_data    加速度原始数据
//返回参数      void
//使用示例          first_order_complementary_filtering(&balance_cascade.cascade_value,*balance_cascade.cascade_value.gyro_raw_data,*balance_cascade.cascade_value.acc_raw_data);
//备注信息
//------
void first_order_complementary_filtering(cascade_common_value_struct *filter_value,int16 gyro_raw_data,int16 acc_raw_data)
{
    float gyro_temp;            //角速度计算临时变量f
    float acc_temp;          //加速度计算临时变量

    gyro_temp =gyro_raw_data *filter_value->gyro_ration;    //角速度数据*角速度置信度(一般给4)

    acc_temp =(acc_raw_data -filter_value->angle_temp)*filter_value->acc_ration;//加速度微分数据*加速度置信度(一般给4)

    filter_value->angle_temp +=((gyro_temp +acc_temp)*filter_value->call_cycle);  //两数之和*调用周期并积分到角度输出

    filter_value->filtering_angle=filter_value->angle_temp +filter_value->mechanical_zero;  //最终滤波角度成去零点位置即可
}


//-----
//函数简介      PID闭环计算(位置式)
//参数说明      pid_cycle               PID参数结构体
//参数说明      target                  目标值
//参数说明      real                    当前值
//返回参数      void
//使用示例      pid_control(&balance cascade speed cycle,θ,(left_motor.encoder_data+right_motor.encoder_data)/2);
//备注信息
//---
void pid_control (pid_cycle_struct *pid_cycle,float target,float real)
{
    float   proportion_value=0;     //比例量

    float   differential_value=0;   //微分量

    proportion_value =target -real; //比例量=目标值-实际值

    pid_cycle->i_value +=(proportion_value *pid_cycle->i_value_pro);    //积分量=积分量+比例量*积分程度

    pid_cycle->i_value =func_limit_ab(pid_cycle->i_value,-pid_cycle->i_value_max,pid_cycle->i_value_max);//积分量限幅

    differential_value =proportion_value -pid_cycle->p_value_last;      //微分量=比例量-上一次比例量

    pid_cycle->out =(int16)(pid_cycle->p*proportion_value +pid_cycle->i*pid_cycle->i_value +pid_cycle->d*differential_value);//PIOM合

    pid_cycle->out =func_limit_ab(pid_cycle->out,-pid_cycle->out_max,pid_cycle->out_max);   //PID输出限幅

    pid_cycle->p_value_last =proportion_value;  //保存比例量
}
//---
//函数简介      PID闭环计算(增量式)
//参数说明      pid_cycle           PID参数结构体
//参数说明      target              目标值
//参数说明      real                当前值
//返回参数      void
//使用示例      pid_control_incremental（&balancecascade.speed cycle,0,（left_motor.encoder_data+right_motor.encoder_data)/ 2);
//备注信息
/*void pid_control_incremental(pid_cycle_struct *pid_cycle,float target,float real)
{
    float   proportion_value=0;//比例璧

    float   differential_value=0;//微分量

    pid_cycle->i_value=target-real;//积分里=目标值-实际值增里式PIDP--->I

    differential_value=proportion_value-2*pid_cycle->incremental_data[0]+pid_cycle->incremental_data[1];//微分量增量式PIDI--->D

    proportion_value=proportion_value-pid_cycle->incremental_data[0];//比例量增量式PIDD--->P

    pid_cycle->incremental_data[1]= pid_cycle->incremental_data[0];//增量式PID保存参数

    pid_cycle->incremental_data[0]=proportion_value;

    pid_cycle->out +=(int16)(pid_cycle->p *proportion_value +pid_cycle->i*pid_cycle->i_value +pid_cycle->d * differential_value);// PID拟合

    pid_cycle->out=func_limit_ab(pid_cycle->out,-pid_cycle->out_max,pid_cycle->out_max);//PID输出限幅

}*/
//void pid_control_incremental(pid_cycle_struct *pid_cycle, float target, float real)
//{
//    // 1. 计算当前误差
//    float error = target - real;
//
//    // 2. 计算三项分量
//    float proportion_value = error - pid_cycle->incremental_data[0];          // P: e(k)-e(k-1)
//    float integral_value = error;                                            // I: e(k)
//    float differential_value = error - 2*pid_cycle->incremental_data[0]
//                              + pid_cycle->incremental_data[1];             // D: e(k)-2e(k-1)+e(k-2)
//
//    // 3. 更新历史误差（移位存储）
//    pid_cycle->incremental_data[1] = pid_cycle->incremental_data[0];  // e(k-2) = 旧 e(k-1)
//    pid_cycle->incremental_data[0] = error;                           // e(k-1) = 当前误差
//
//    // 4. 计算输出增量并累加
//    float delta_out = pid_cycle->p * proportion_value
//                    + pid_cycle->i * integral_value
//                    + pid_cycle->d * differential_value;
//
//    pid_cycle->out += (int16_t)delta_out;
//
//    // 5. 输出限幅
//    pid_cycle->out = func_limit_ab(pid_cycle->out, -pid_cycle->out_max, pid_cycle->out_max);
//}
float KP=0.06,KD=0.15,KPP=0,KDD=0;//0.15
int error,last_error=0;
int PD_OUT=0;
int P_X,D_x,DD_x;
int last_real;
void pid_control_incremental(pid_cycle_struct *pid_cycle, float real_g, float real)
{
    static float a=0.5,target=0;

    //2、低通滤波
    real=(1-a)*real+a*last_real;
    last_real=real;
    // 1. 计算当前误差
    float error = target - real;




    PD_OUT= KPP*(error*func_abs(error))+KP*error+KD*(error-last_error)+KDD*(real_g);
    P_X=KPP*error*func_abs(error)+KP*error;
    D_x=KD*(error-last_error);
    DD_x=KDD*(real_g);
last_error=error ;




    pid_cycle->out = (int16_t)PD_OUT;

    // 5. 输出限幅
    pid_cycle->out = func_limit_ab(pid_cycle->out, -pid_cycle->out_max, pid_cycle->out_max);
}

/*void pid_control_with_angular_velocity(pid_cycle_struct *pid_cycle, float target, float real, float angular_velocity)
{
    float delta_p, delta_i, delta_dw,lubo=0.3;//(0.2-0.5)    // 各项增量
    static float last_filtered_velocity;

    // 计算当前误差
    float current_error = target - real;

    // 2. 角速度低通滤波
    float filtered_velocity = lubo * angular_velocity + (1 - lubo) *last_filtered_velocity;
    last_filtered_velocity = filtered_velocity;

    // 比例项：Kp * [e(k) - e(k-1)]
    delta_p = pid_cycle->p * (current_error - pid_cycle->incremental_data[0]);

    // 积分项：Ki * e(k)
    delta_i = pid_cycle->i * current_error;

    // 角速度项：Kdw * ω(k)（ω为测量的角速度）
    delta_dw = pid_cycle->dw * filtered_velocity;

    // 计算总控制增量（结合传统微分项和角速度项）
    float delta_u = delta_p + delta_i+ delta_dw;

    // 输出限幅并更新
    pid_cycle->out = func_limit_ab(pid_cycle->out + delta_u, -pid_cycle->out_max, pid_cycle->out_max);

    // 更新历史误差
    pid_cycle->incremental_data[0] = current_error;
}*/

//float a=0.01,b=0.0,c=-0.003;


//函数简介      串级平衡算法初始化
//返回参数      void
//使用示例      balance_cascade_init();
//备注信息      设置串级平衡参数
//-----
void balance_cascade_init(void)
{
    balance_cascade.cascade_value.gyro_raw_data     =           &imu660ra_gyro_x;
    balance_cascade.cascade_value.acc_raw_data      =           &imu660ra_acc_y;
    balance_cascade.cascade_value.gyro_ration       =           4.0f;
    balance_cascade.cascade_value.acc_ration        =           4.0f;
    balance_cascade.cascade_value.call_cycle        =           0.005f;
    balance_cascade.cascade_value.mechanical_zero   =            0;//50  -200

    balance_cascade.cascade_value.filtering_angle   =   -balance_cascade.cascade_value.mechanical_zero;
    balance_cascade.cascade_value.angle_temp        =   -balance_cascade.cascade_value.mechanical_zero;

    balance_cascade.angular_speed_cycle.i_value_max =           1000;
    balance_cascade.angular_speed_cycle.i_value_pro =           0.0f;
    balance_cascade.angular_speed_cycle.out_max     =           10000;
    balance_cascade.angle_cycle.i_value_max         =           4000;
    balance_cascade.angle_cycle.i_value_pro         =           0.05f;
    balance_cascade.angle_cycle.out_max             =           8000;
    balance_cascade.speed_cycle.i_value_max         =           4000;
    balance_cascade.speed_cycle.i_value_pro         =           0.05f;
    balance_cascade.speed_cycle.out_max             =           1500;

    balance_cascade.angular_speed_cycle.p           =           1.3f;//0.7
    balance_cascade.angular_speed_cycle.i           =           0.000f;//0.003
    balance_cascade.angular_speed_cycle.d           =           0.05f;

    balance_cascade.angle_cycle.p                   =           9.0f;//8
    balance_cascade.angle_cycle.i                   =           0.0f;
    balance_cascade.angle_cycle.d                   =           0.05f;

    balance_cascade.speed_cycle.p                   =          8.0f;//8
    balance_cascade.speed_cycle.i                   =           0.0f;
    balance_cascade.speed_cycle.d                   =           0.0f;

    steer_balance_cascade.cascade_value.gyro_raw_data=&imu660ra_gyro_y;
    steer_balance_cascade.cascade_value.acc_raw_data=&imu660ra_acc_x;
    steer_balance_cascade.cascade_value.gyro_ration=4.3f;
    steer_balance_cascade.cascade_value.acc_ration= 4.0f;;
    steer_balance_cascade.cascade_value.call_cycle=0.005f;
    steer_balance_cascade.cascade_value.mechanical_zero=350;
    steer_balance_cascade.cascade_value.filtering_angle=-steer_balance_cascade.cascade_value.mechanical_zero;
    steer_balance_cascade.cascade_value.angle_temp=-steer_balance_cascade.cascade_value.mechanical_zero;

    steer_balance_cascade.angular_speed_cycle.i_value_max=1000;
    steer_balance_cascade.angular_speed_cycle.i_value_pro=0.1f;
    steer_balance_cascade.angular_speed_cycle.out_max=10000;
    steer_balance_cascade.angle_cycle.i_value_max=4000;
    steer_balance_cascade.angle_cycle.i_value_pro=0.05f;
    steer_balance_cascade.angle_cycle.out_max=300;
    steer_balance_cascade.speed_cycle.i_value_max=4000;
    steer_balance_cascade.speed_cycle.i_value_pro=0.05f;
    steer_balance_cascade.speed_cycle.out_max=1500;

    steer_balance_cascade.angular_speed_cycle.p=0.0f;
    steer_balance_cascade.angular_speed_cycle.i=0.0f;
    steer_balance_cascade.angular_speed_cycle.d=0.0f;

    steer_balance_cascade.angle_cycle.p=0.02f;//0.02
    steer_balance_cascade.angle_cycle.i=0.006f;//0.005
   // steer_balance_cascade.angle_cycle.dw=c;
    steer_balance_cascade.angle_cycle.d=0.0f;

    steer_balance_cascade.speed_cycle.p=0.0f;
    steer_balance_cascade.speed_cycle.i=0.0f;
    steer_balance_cascade.speed_cycle.d =0.0f;

    memcpy(&balance_cascade_resave,&balance_cascade,sizeof(balance_cascade_resave));
    memcpy(&steer_balance_cascade_resave,&steer_balance_cascade,sizeof(steer_balance_cascade_resave));


}
//float Turn_p=21,n=9;
//int Turn(float gyro_Z,int Target_turn)
//{
//    int temp;
//    temp=Turn_p*(gyro_Z+n*Target_turn);
//    return temp;
//}
//int kp1=88,kd1=14,kpp=1;// 90 30 16   550
//int Turn(float gyro_Z,int Target_turn)
//{
//
//    int temp;
//    temp=kp1*Target_turn+kpp*(Target_turn*func_abs(Target_turn))+kd1*gyro_Z;
//    return temp;
//
//}
//int kp1=70,kd1=9,kpp=1;// 90 30 16  520
//int Turn(float gyro_Z,int Target_turn)
//{
//
//    int temp;
//    temp=kp1*Target_turn+kpp*(Target_turn*func_abs(Target_turn))+kd1*gyro_Z;
//    return temp;
//
//}
int kp1=73,kd1=11,kpp=2;// 90 30 16  500   70 8 1
int Turn(float gyro_Z,int Target_turn)
{

    int temp;
    temp=kp1*Target_turn+kpp*(Target_turn*func_abs(Target_turn))+kd1*gyro_Z;
    return temp;

}
int Target_turn=0,out_Turn;

void dynamic_motor_control(void)
{
    int16 left_motor_duty;
    int16 right_motor_duty;
    if(run_flag)
    {
        if(func_abs(balance_cascade.cascade_value.filtering_angle)>2000)//倾倒角度过大停止转动
        {
            run_flag =0;
        }
        out_Turn=Turn(imu660ra_gyro_transition(imu660ra_gyro_z),Target_turn);
        left_motor_duty =func_limit_ab(balance_cascade.angular_speed_cycle.out-out_Turn ,-4000,4000);
        right_motor_duty =func_limit_ab(balance_cascade.angular_speed_cycle.out+out_Turn ,-4000,4000);
        small_driver_set_duty(left_motor_duty,-right_motor_duty);
    }

    else
    {
        small_driver_set_duty(0,0);
    }
}
void imu_date_get()
{
    imu660ra_get_gyro             ();//获取陀螺仪数据
    imu660ra_get_acc              ();//获取加速度数据
    imu660ra_gyro_y =imu660ra_gyro_y-2.5 ;
    imu660ra_gyro_z=imu660ra_gyro_z+2.5;
    if(func_abs(imu660ra_gyro_y)<=5)
    {
        imu660ra_gyro_y=0;
    }
    if(func_abs(imu660ra_gyro_z)<=5)
    {
        imu660ra_gyro_z=0;
    }
    if(func_abs(imu660ra_gyro_x)<=5)
    {
        imu660ra_gyro_x=0;
    }
}
int speed=400;
float out;
extern float steer_flag;
extern float abcd;
void pit_isr_callback(void)
{
    //abcd=1;
    system_count++;
    imu_date_get();
    jump_a();
    if(system_count%20==0)
    {
       car_speed =(-motor_value.receive_right_speed_data+motor_value.receive_left_speed_data)/2;
        pid_control(&balance_cascade.speed_cycle,speed ,car_speed);//


    }

    if(system_count%5==0)
    {
        first_order_complementary_filtering( &balance_cascade.cascade_value,*balance_cascade.cascade_value.gyro_raw_data ,*balance_cascade.cascade_value.acc_raw_data );

        pid_control(&balance_cascade.angle_cycle,0,balance_cascade.cascade_value.filtering_angle);
if(abcd)
{
first_order_complementary_filtering(&steer_balance_cascade.cascade_value,*steer_balance_cascade.cascade_value.gyro_raw_data ,-*steer_balance_cascade.cascade_value.acc_raw_data) ;

pid_control_incremental(&steer_balance_cascade.angle_cycle,0 ,steer_balance_cascade.cascade_value.filtering_angle);


}
    }

      pid_control(&balance_cascade.angular_speed_cycle,balance_cascade.angle_cycle.out, *balance_cascade.cascade_value.gyro_raw_data);

      dynamic_steer_control();

       dynamic_motor_control();

      dynamic_jump_control();


}
