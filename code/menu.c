#include "menu.h"
#include "zf_common_headfile.h"

int key;
//增加减少的按键还没改好

uint32 data_base_pid[10];
extern kp1,kd1,kpp,speed,Threshold_add;
int n=1;


void menu_1()
{

        int i=0,enter=0;     //1 下       2 中
            if(flash_check(62,1)) //flash读取
            {
            flash_read_page(62,1,data_base_pid,10);
            kp1=(float)data_base_pid[0]/1000;
            kd1=(float)data_base_pid[1]/1000;
            kpp=data_base_pid[2];
            speed=data_base_pid[3];
            Threshold_add=data_base_pid[4];

            }
            while(1)
            {
                key_scanner();
                if(key_get_state(KEY_2)==2){system_delay_ms(100);while(key_get_state(KEY_2)==1);system_delay_ms(100);key= 1;}
                    if(key_get_state(KEY_1)==2){system_delay_ms(100);while(key_get_state(KEY_1)==1);system_delay_ms(100);key= 2;}
                    if(key_get_state(KEY_3)==2){system_delay_ms(100);while(key_get_state(KEY_3)==1);system_delay_ms(100);key= 3;}
                    if(key_get_state(KEY_4)==2){system_delay_ms(100);while(key_get_state(KEY_4)==1);system_delay_ms(100);key= 4;}
                    //if(key_get_state(KEY_5)==2){system_delay_ms(100);while(key_get_state(KEY_5)==1);system_delay_ms(100);key= 5;}



                if(key==1) {i++; i%=5;key=0; }
                if(key==3) {i--; i%=5; key=0;}
                if(key==2&&enter==0)  {enter=1;key=0;}
                if(enter==1&&key==2)
                {
                    enter=0;key=0;
                    flash_write_page(62,1,data_base_pid,10);

                tft180_show_string(100, 0, " ");
                tft180_show_string(100, 15, " ");
                tft180_show_string(90, 30, " ");
                tft180_show_string(90, 45, " ");
                tft180_show_string(90, 60, " ");
                tft180_show_string(90, 75, " ");
                }

                    switch(i)
                    {
                    case 0: {tft180_show_string(0, 90, " ");tft180_show_string(0, 0, "*");tft180_show_string(0, 15, " "); }break;
                    case 1: {tft180_show_string(0, 0, " "); tft180_show_string(0, 15, "*");tft180_show_string(0, 30, " ");} break;
                    case 2: {tft180_show_string(0, 15, " "); tft180_show_string(0, 30, "*"); tft180_show_string(0, 45, " ");}break;
                    case 3: {tft180_show_string(0, 30, " "); tft180_show_string(0, 45, "*");tft180_show_string(0, 60, " ");}break;
                    case 4: {tft180_show_string(0, 45, " "); tft180_show_string(0, 60, "*");tft180_show_string(0, 75, " ");}break;
//
                    }


                if(enter==1&&i==0)
                {
                    tft180_show_string(100, 0, "*");
                    if(key==4)
                    {
                        kp1+=1;
                        key=0;
                    }
                    if(key==5)
                    {
                        kp1-=1;
                        key=0;
                    }
                }
                if(enter==1&&i==1)
                {
                    tft180_show_string(100, 15, "*");
                    if(key==4)
                    {
                        kd1+=0.005;
                        key=0;
                    }
                    if(key==5)
                    {
                        kd1-=0.005;
                        key=0;
                    }
                }
                if(enter==1&&i==2)
                {
                    tft180_show_string(90, 30, "*");
                    if(key==4)
                    {
                        kpp++;
                        key=0;
                    }
                    if(key==5)
                    {
                       kpp--;
                        key=0;
                    }

                }
                if(enter==1&&i==3)
                {
                    tft180_show_string(90, 45, "*");
                    if(key==4)
                    {
                        speed++;
                        key=0;
                    }
                    if(key==5)
                    {
                       speed--;
                        key=0;
                    }

                }

                if(enter==1&&i==4)
                {
                    tft180_show_string(90, 60, "*");
                    if(key==4)
                    {
                        Threshold_add++;
                        key=0;
                    }
                    if(key==5)
                    {
                        Threshold_add--;
                        key=0;
                    }

                }

                if(enter==1&&i==6)
                {
                  tft180_clear();break;
                }

                data_base_pid[0]=kp1;
                data_base_pid[1]=kd1;
                data_base_pid[2]=kpp;
                data_base_pid[3]=speed;
                data_base_pid[4]=Threshold_add;



               tft180_show_string(10, 0, "kp1");
               tft180_show_string(10, 15, "kd1");
               tft180_show_string(10,30,"kpp");
               tft180_show_string(10,45,"speed");
               tft180_show_string(10,60,"Threshold_add");


               tft180_show_float(65, 0, kp1,1,3);
               tft180_show_float(65, 15, kd1,1,3);
               tft180_show_int(65, 30,kpp, 3);
               tft180_show_int(65, 45, speed, 4);
               tft180_show_int(65, 60, Threshold_add, 3);



    }
  }
