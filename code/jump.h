

#ifndef CODE_JUMP_H_
#define CODE_JUMP_H_

#include "zf_common_headfile.h"

typedef void(*HandlerFunc)(int value);  //定义处理函数类型

typedef struct
{
        int16 min;                      //执行时间起点
        int16 max;                      //执行时间结束
        HandlerFunc handler;            //执行内容
        const char *description;        //行为描述
}jump_control_struct;

void dynamic_jump_control(void);





#endif /* CODE_JUMP_CONTROL_H_ */
