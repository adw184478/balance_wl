
#include "zf_common_headfile.h"

void jump_step_a(int step_num)
{
    switch(step_num)
    {
        case 0:
               {
                   steer_duty_set(&steer_1,steer_1.center_num -1000);
                   steer_duty_set(&steer_2,steer_2.center_num +1000);
                   steer_duty_set(&steer_3,steer_3.center_num +1000);
                   steer_duty_set(&steer_4,steer_4.center_num -1000);
               }break;
        case 1:
        {
            steer_duty_set(&steer_1,steer_1.center_num +3000);
            steer_duty_set(&steer_2,steer_2.center_num -3000);
            steer_duty_set(&steer_3,steer_3.center_num -3000);
            steer_duty_set(&steer_4,steer_4.center_num +3000);
        }break;
        case 2:
        {
            steer_duty_set(&steer_1,steer_1.center_num-1000);
            steer_duty_set(&steer_2,steer_2.center_num+1000);
            steer_duty_set(&steer_3,steer_3.center_num+1000);
            steer_duty_set(&steer_4,steer_4.center_num-1000);
        }break;
        case 3:
        {
            steer_duty_set(&steer_1,steer_1.center_num+400 );
            steer_duty_set(&steer_2,steer_2.center_num-400 );
            steer_duty_set(&steer_3,steer_3.center_num-400 );
            steer_duty_set(&steer_4,steer_4.center_num+400 );
        }break;
        case 4:
        {
            steer_control(&steer_1,-14);
            steer_control(&steer_2,-14);
            steer_control(&steer_3,-14);
            steer_control(&steer_4,-14);
        }break;
        case 5:
                {
                    steer_duty_set(&steer_1,steer_1.center_num);
                    steer_duty_set(&steer_2,steer_2.center_num);
                    steer_duty_set(&steer_3,steer_3.center_num);
                    steer_duty_set(&steer_4,steer_4.center_num);
                }break;
        default:break;
    }
}

const jump_control_struct jump_control_config[]=
{
    {0, 100,jump_step_a,"  收腿起跳准备 "},
    {100,200,jump_step_a,"  起跳  "},
    {200,310,jump_step_a," 收脚"},
    {310,320,jump_step_a," 准备缓冲"},
    {320,420,jump_step_a,"ִ执行缓冲 "},
     {600,700,jump_step_a,"ִ恢复初始位置 "},
};


const uint8 jump_step =sizeof(jump_control_config)/sizeof(jump_control_struct);
void dynamic_jump_control(void)
{
    if(jump_flag &&run_flag)
    {
       jump_time ++;


       if(jump_time<jump_control_config[jump_step -1].max)
        {
           for(int i=0;i<jump_step;i++)
           {
               if(jump_time >=jump_control_config[i].min &&jump_time <=jump_control_config[i].max)
               {
                   jump_control_config[i].handler(i);
                   break;
               }
           }

        }
       else
       {
           jump_flag =0;
           jump_time =0;
       }
    }
}
void jump_a()
{
//if(system_count==2000)
//{jump_flag=1;}
    if(jump_flag)
    {
        balance_cascade.angle_cycle.p = balance_cascade_resave.angle_cycle.p*0.5f;
        balance_cascade.speed_cycle.p = balance_cascade_resave.speed_cycle.p*0.5f;
    }
    else
    {
        balance_cascade.angle_cycle.p = balance_cascade_resave.angle_cycle.p;
        balance_cascade.speed_cycle.p = balance_cascade_resave.speed_cycle.p;
    }

}
