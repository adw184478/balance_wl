/*
 * image.h
 *
 *  Created on: 2025年1月11日
 *      Author: 零点
 */

#ifndef CODE_IMAGE_H_
#define CODE_IMAGE_H_
#include "zf_common_headfile.h"

//颜色定义  因为有先例，连颜色都改不来，我直接放这了
#define uesr_RED     0XF800    //红色
#define uesr_GREEN   0X07E0    //绿色
#define uesr_BLUE    0X001F    //蓝色

//宏定义
#define image_h 60//图像高度
#define image_w 120//图像宽度

#define white_pixel 255
#define black_pixel 0
#define mid_w 60  //中线点的初始值

#define bin_jump_num    1//跳过的点数
#define border_max  image_w-2 //边界最大值
#define border_min  1   //边界最小值
extern uint8 original_image[image_h][image_w];
extern uint8 bin_image[image_h][image_w];//图像数组

extern void image_process(void); //直接在中断或循环里调用此程序就可以循环执行了
int find_mid_line_count(void);

#endif /* CODE_IMAGE_H_ */
