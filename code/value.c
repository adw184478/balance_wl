/*
 * user_values.c
 *
 *  Created on: 2025年3月7日
 *      Author: 94153
 */
#include "value.h"
#include "zf_common_headfile.h"

uint32 system_count;//系统时间计数
uint8 run_flag=1 ;//运行标志位
uint8 jump_flag;//跳跃标志位

uint32 jump_time;//跳跃时间计数
int16 car_speed;//当前小车速度

