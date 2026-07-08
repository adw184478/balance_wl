

#include"zf_common_headfile.h"

steer_control_struct steer_1;
steer_control_struct steer_2;
steer_control_struct steer_3;
steer_control_struct steer_4;

void steer_control_init(void)
{
steer_1.pwm_pin=STEER_1_PWM;
steer_1.control_frequency= STEER_1_FRE;
steer_1.steer_dir= STEER_1_DIR;
steer_1.center_num= STEER_1_CENTER;

steer_2.pwm_pin=STEER_2_PWM;
steer_2.control_frequency= STEER_2_FRE;
steer_2.steer_dir=STEER_2_DIR;
steer_2.center_num= STEER_2_CENTER;

steer_3.pwm_pin=STEER_3_PWM;
steer_3.control_frequency= STEER_3_FRE;
steer_3.steer_dir= STEER_3_DIR;
steer_3.center_num=STEER_3_CENTER;

steer_4.pwm_pin=STEER_4_PWM;
steer_4.control_frequency= STEER_4_FRE;
steer_4.steer_dir= STEER_4_DIR;
steer_4.center_num=STEER_4_CENTER;

steer_1.center_num =steer_1.center_num -1000 * steer_1.steer_dir;
steer_2.center_num =steer_2.center_num -1000 *steer_2.steer_dir;
steer_3.center_num =steer_3.center_num -1000 * steer_3.steer_dir;
steer_4.center_num =steer_4.center_num -1000*steer_4.steer_dir;

steer_1.now_location= steer_1.center_num;
steer_2.now_location= steer_2.center_num;
steer_3.now_location= steer_3.center_num;
steer_4.now_location= steer_4.center_num;

pwm_init(steer_1.pwm_pin,steer_1.control_frequency,steer_1.now_location);
pwm_init(steer_2.pwm_pin,steer_2.control_frequency,steer_2.now_location);
pwm_init(steer_3.pwm_pin,steer_3.control_frequency,steer_3.now_location);
pwm_init(steer_4.pwm_pin,steer_4.control_frequency,steer_4.now_location);
}

void steer_duty_set(steer_control_struct *control_data,int16 duty)
{
    control_data->now_location = func_limit_ab(duty,-10000,10000);
    pwm_set_duty(control_data->pwm_pin,duty);
}

void steer_control(steer_control_struct *control_data,int16 move_num)
{
    control_data->now_location =control_data->now_location+(control_data->steer_dir ==1?move_num:-move_num);
    control_data->now_location =func_limit_ab(control_data->now_location,2000,6000);
    pwm_set_duty(control_data->pwm_pin,control_data->now_location);
}
extern float steer_flag,abcd;
float abc;
float ab=6.0;
int16 steer_output_duty=0;
float steer_output_duty_filter=0;
float steer_balance_angle = 0;
void dynamic_steer_control(void)
{
int16 steer_location_offset[4]={0};
int16 steer_target_offset[4]={0};


//if(steer_flag==0)//steer_flag==3
//{

    steer_output_duty = -func_limit_ab(-balance_cascade.speed_cycle.out / 7,-350,350) * ab;

    steer_output_duty_filter=(steer_output_duty_filter *19 +(float)steer_output_duty)/ 20.0f;

//}

if(jump_flag == 0)
{
    if(abcd==1)
    {
        abc=6.0;
    }
    else
    {
        abc=1.0;
       // steer_balance_cascade.angle_cycle.p=steer_balance_cascade_resave.angle_cycle.p*2;
    }
    steer_balance_angle = func_limit_ab(steer_balance_cascade.angle_cycle.out,-280,280) *abc;
}
else
{
    steer_balance_angle =0;
}

steer_location_offset[0]=(steer_1.now_location -steer_1.center_num)*steer_1.steer_dir;
steer_location_offset[1]=(steer_2.now_location -steer_2.center_num)*steer_2.steer_dir;
steer_location_offset[2]=(steer_3.now_location -steer_3.center_num)*steer_3.steer_dir;
steer_location_offset[3]=(steer_4.now_location -steer_4.center_num)*steer_4.steer_dir;

steer_target_offset[0]=steer_output_duty_filter -(steer_balance_angle>0?0:steer_balance_angle);
steer_target_offset[1]=steer_output_duty_filter +(steer_balance_angle<0?0:steer_balance_angle);
steer_target_offset[2]=-steer_output_duty_filter -(steer_balance_angle>0?0:steer_balance_angle);
steer_target_offset[3]=-steer_output_duty_filter +(steer_balance_angle<0?0:steer_balance_angle);

if(run_flag ==1)
{
    if(jump_flag ==0&&steer_flag==0)
    {
        steer_control(&steer_1,func_limit_ab(steer_target_offset[0]-steer_location_offset[0],-5,5));
        steer_control(&steer_2,func_limit_ab(steer_target_offset[1]-steer_location_offset[1],-5,5));
        steer_control(&steer_3,func_limit_ab(steer_target_offset[2]-steer_location_offset[2],-5,5));
        steer_control(&steer_4,func_limit_ab(steer_target_offset[3]-steer_location_offset[3],-5,5));
    }
}
else
{
    steer_control(&steer_1,func_limit_ab(steer_1.center_num -steer_1.now_location,-1,1)*steer_1.steer_dir);
    steer_control(&steer_2,func_limit_ab(steer_2.center_num -steer_2.now_location,-1,1)*steer_1.steer_dir);
    steer_control(&steer_3,func_limit_ab(steer_3.center_num -steer_3.now_location,-1,1)*steer_1.steer_dir);
    steer_control(&steer_4,func_limit_ab(steer_4.center_num -steer_4.now_location,-1,1)*steer_4.steer_dir);
}

}

