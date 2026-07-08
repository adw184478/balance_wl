//-------------------------------------------------------------------------------------------------------------------
//  简介:八邻域图像处理

//------------------------------------------------------------------------------------------------------------------

#include "zf_common_headfile.h"
uint8 finish=0;
int hou_speed=550;

int eeee=0,llll=0;
int aaa=0,bbb=0,ccc=0;
/*
函数名称：int my_abs(int value)
功能说明：求绝对值
参数说明：
函数返回：绝对值
修改时间：2022年9月8日
备    注：
example：  my_abs( x)；
 */
//绝对值函数
int my_abs(int value)
{
if(value>=0) return value;
else return -value;
}

//最小值函数
int  min(int a,int b)
{
   if(a>b)
   {
      a=b ;

   }
return a;
}
//最大值函数
int max(int a,int b)
{
    if(a<b)
    {
        a=b;
    }
    return a;

}
//限幅函数
int limit_a_b(int x, int min, int max)
{
    if(x<min) x = min;
    if(x>max) x = max;
    return x;
}

/*变量声明*/
uint8 original_image[image_h][image_w];
uint8 image_thereshold;//图像分割阈值
extern uint8 Threshold_add;
//------------------------------------------------------------------------------------------------------------------
//  @brief      获得一副灰度图像
//  @since      v1.0
//------------------------------------------------------------------------------------------------------------------
void Get_image(uint8(*mt9v03x_image)[image_w])
{
#define use_num     1   //1就是不压缩，2就是压缩一倍
    uint8 i = 0, j = 0, row = 0, line = 0;
    for (i = 0; i < image_h; i += use_num)          //
    {
        for (j = 0; j <image_w; j += use_num)     //
        {
            original_image[row][line] = mt9v03x_image[i][j];//这里的参数填写你的摄像头采集到的图像
            line++;
        }
        line = 0;
        row++;
    }
}
//------------------------------------------------------------------------------------------------------------------
//  @brief     动态阈值
//  @since      v1.0
//------------------------------------------------------------------------------------------------------------------
uint8 otsuThreshold(uint8 *image, uint16 col, uint16 row)
{
#define GrayScale 256
    uint16 Image_Width  = col;
    uint16 Image_Height = row;
    int X; uint16 Y;
    uint8* data = image;
    int HistGram[GrayScale] = {0};

    uint32 Amount = 0;
    uint32 PixelBack = 0;
    uint32 PixelIntegralBack = 0;
    uint32 PixelIntegral = 0;
    int32 PixelIntegralFore = 0;
    int32 PixelFore = 0;
    double OmegaBack=0, OmegaFore=0, MicroBack=0, MicroFore=0, SigmaB=0, Sigma=0; // 类间方差;
    uint8 MinValue=0, MaxValue=0;
    uint8 Threshold = 0;


    for (Y = 0; Y <Image_Height; Y++) //Y<Image_Height改为Y =Image_Height；以便进行 行二值化
    {
        //Y=Image_Height;
        for (X = 0; X < Image_Width; X++)
        {
        HistGram[(int)data[Y*Image_Width + X]]++; //统计每个灰度值的个数信息
        }
    }




    for (MinValue = 0; MinValue < 256 && HistGram[MinValue] == 0; MinValue++) ;        //获取最小灰度的值
    for (MaxValue = 255; MaxValue > MinValue && HistGram[MaxValue] == 0; MaxValue--) ; //获取最大灰度的值
    if (MaxValue == MinValue)
    {
        return MaxValue;          // 图像中只有一个颜色
    }
    if (MinValue + 1 == MaxValue)
    {
        return MinValue;      // 图像中只有二个颜色
    }

    for (Y = MinValue; Y <= MaxValue; Y++)
    {
        Amount += HistGram[Y];        //  像素总数
    }

    PixelIntegral = 0;
    for (Y = MinValue; Y <= MaxValue; Y++)
    {
        PixelIntegral += HistGram[Y] * Y;//灰度值总数
    }
    SigmaB = -1;
    for (Y = MinValue; Y < MaxValue; Y++)
    {
          PixelBack = PixelBack + HistGram[Y];    //前景像素点数
          PixelFore = Amount - PixelBack;         //背景像素点数
          OmegaBack = (double)PixelBack / Amount;//前景像素百分比
          OmegaFore = (double)PixelFore / Amount;//背景像素百分比
          PixelIntegralBack += HistGram[Y] * Y;  //前景灰度值
          PixelIntegralFore = PixelIntegral - PixelIntegralBack;//背景灰度值
          MicroBack = (double)PixelIntegralBack / PixelBack;//前景灰度百分比
          MicroFore = (double)PixelIntegralFore / PixelFore;//背景灰度百分比
          Sigma = OmegaBack * OmegaFore * (MicroBack - MicroFore) * (MicroBack - MicroFore);//g
          if (Sigma > SigmaB)//遍历最大的类间方差g
          {
              SigmaB = Sigma;
              Threshold = (uint8)Y;
          }
    }
   return Threshold;
}
//------------------------------------------------------------------------------------------------------------------
//  @brief      图像二值化，这里用的是大津法二值化。
//  @since      v1.0
//------------------------------------------------------------------------------------------------------------------
uint8 bin_image[image_h][image_w];//二值化后的图像数组
void turn_to_bin(void)
{
  uint8 i,j;
  image_thereshold = otsuThreshold(original_image[0], image_w, image_h)+Threshold_add;
  for(i = 0;i<image_h;i++)
  {
      for(j = 0;j<image_w;j++)
      {
          if(original_image[i][j]>image_thereshold)
          {
              bin_image[i][j] = white_pixel;
          }
          else
          {
              bin_image[i][j] = black_pixel;
          }
      }
  }
}


/*
函数名称：void get_start_point(uint8 start_row)
功能说明：寻找两个边界的边界点作为八邻域循环的起始点
参数说明：输入任意行数
函数返回：无
修改时间：2022年9月8日
备    注：
example：  get_start_point(image_h-2)
 */
uint8 origin=60;
uint8 start_point_l[2] = { 0 };//左边起点的x，y值
uint8 start_point_r[2] = { 0 };//右边起点的x，y值
uint8 get_start_point(uint8 start_row)
{
    uint8 i = 0,l_found = 0,r_found = 0;
    //清零
    start_point_l[0] = 0;//x
    start_point_l[1] = 0;//y

    start_point_r[0] = 0;//x
    start_point_r[1] = 0;//y

        //从中间往左边，先找起点
    for (i = origin; i > border_min; i--)
    {
        start_point_l[0] = i;//x            为白点坐标
        start_point_l[1] = start_row;//y
        if (bin_image[start_row][i+1]==255&&bin_image[start_row][i] == 255 && bin_image[start_row][i - 1] == 0&&bin_image[start_row][i -2]==0)
        {
            //printf("找到左边起点image[%d][%d]\n", start_row,i);
            l_found = 1;
            break;
        }
    }

    for (i = origin; i < border_max; i++)
    {
        start_point_r[0] = i;//x
        start_point_r[1] = start_row;//y
        if (bin_image[start_row][i-1] == 255&&bin_image[start_row][i] == 255 && bin_image[start_row][i + 1] == 0&& bin_image[start_row][i + 2] == 0)
        {
            //printf("找到右边起点image[%d][%d]\n",start_row, i);
            r_found = 1;
            break;
        }
    }

    if(l_found&&r_found)return 1;
    else {
        //printf("未找到起点\n");
        return 0;
    }
}

/*
函数名称：void search_l_r(uint16 break_flag, uint8(*image)[image_w],uint16 *l_stastic, uint16 *r_stastic,
                            uint8 l_start_x, uint8 l_start_y, uint8 r_start_x, uint8 r_start_y,uint8*hightest)

功能说明：八邻域正式开始找右边点的函数，输入参数有点多，调用的时候不要漏了，这个是左右线一次性找完。
参数说明：
break_flag_r            ：最多需要循环的次数
(*image)[image_w]       ：需要进行找点的图像数组，必须是二值图,填入数组名称即可
                       特别注意，不要拿宏定义名字作为输入参数，否则数据可能无法传递过来
*l_stastic              ：统计左边数据，用来输入初始数组成员的序号和取出循环次数
*r_stastic              ：统计右边数据，用来输入初始数组成员的序号和取出循环次数
l_start_x               ：左边起点横坐标
l_start_y               ：左边起点纵坐标
r_start_x               ：右边起点横坐标
r_start_y               ：右边起点纵坐标
hightest                ：循环结束所得到的最高高度
函数返回：无
修改时间：2022年9月25日
备    注：
example：
    search_l_r((uint16)USE_num,image,&data_stastics_l, &data_stastics_r,start_point_l[0],
                start_point_l[1], start_point_r[0], start_point_r[1],&hightest);
 */
#define USE_num image_h*3   //定义找点的数组成员个数按理说300个点能放下，但是有些特殊情况确实难顶，多定义了一点

 //存放点的x，y坐标
uint16 points_l[(uint16)USE_num][2] = { {  0 } };//左线
uint16 points_r[(uint16)USE_num][2] = { {  0 } };//右线
uint16 dir_r[(uint16)USE_num] = { 0 };//用来存储右边生长方向
uint16 dir_l[(uint16)USE_num] = { 0 };//用来存储左边生长方向
uint16 data_stastics_l = 0;//统计左边找到点的个数
uint16 data_stastics_r = 0;//统计右边找到点的个数
uint8  hightest = 0;//最高点
void search_l_r(uint16 break_flag, uint8(*image)[image_w], uint16 *l_stastic, uint16 *r_stastic, uint8 l_start_x, uint8 l_start_y, uint8 r_start_x, uint8 r_start_y, uint8*hightest)
{

    uint8 i = 0, j = 0;

    //左边变量
    uint8 search_filds_l[8][2] = { {  0 } };
    uint8 index_l = 0;
    uint8 temp_l[8][2] = { {  0 } };
    uint8 center_point_l[2] = {  0 };
    uint16 l_data_statics;//统计左边
    //定义八个邻域
    static int8 seeds_l[8][2] = { {0,  1},{-1,1},{-1,0},{-1,-1},{0,-1},{1,-1},{1,  0},{1, 1}, };
    //{-1,-1},{0,-1},{+1,-1},
    //{-1, 0},       {+1, 0},
    //{-1,+1},{0,+1},{+1,+1},
    //这个是顺时针

    //右边变量
    uint8 search_filds_r[8][2] = { {  0 } };
    uint8 center_point_r[2] = { 0 };//中心坐标点
    uint8 index_r = 0;//索引下标
    uint8 temp_r[8][2] = { {  0 } };
    uint16 r_data_statics;//统计右边
    //定义八个邻域
    static int8 seeds_r[8][2] = { {0,  1},{1,1},{1,0}, {1,-1},{0,-1},{-1,-1}, {-1,  0},{-1, 1}, };
    //{-1,-1},{0,-1},{+1,-1},
    //{-1, 0},       {+1, 0},
    //{-1,+1},{0,+1},{+1,+1},
    //这个是逆时针

    l_data_statics = *l_stastic;//统计找到了多少个点，方便后续把点全部画出来
    r_data_statics = *r_stastic;//统计找到了多少个点，方便后续把点全部画出来

    //第一次更新坐标点  将找到的起点值传进来
    center_point_l[0] = l_start_x;//x
    center_point_l[1] = l_start_y;//y
    center_point_r[0] = r_start_x;//x
    center_point_r[1] = r_start_y;//y

        //开启邻域循环
    while (break_flag--)
    {

        //左边
        for (i = 0; i < 8; i++)//传递8F坐标
        {
            search_filds_l[i][0] = center_point_l[0] + seeds_l[i][0];//x
            search_filds_l[i][1] = center_point_l[1] + seeds_l[i][1];//y
        }
        //中心坐标点填充到已经找到的点内
        points_l[l_data_statics][0] = center_point_l[0];//x
        points_l[l_data_statics][1] = center_point_l[1];//y
        l_data_statics++;//索引加一

        //右边
        for (i = 0; i < 8; i++)//传递8F坐标
        {
            search_filds_r[i][0] = center_point_r[0] + seeds_r[i][0];//x
            search_filds_r[i][1] = center_point_r[1] + seeds_r[i][1];//y
        }
        //中心坐标点填充到已经找到的点内
        points_r[r_data_statics][0] = center_point_r[0];//x
        points_r[r_data_statics][1] = center_point_r[1];//y

        index_l = 0;//先清零，后使用
        for (i = 0; i < 8; i++)
        {
            temp_l[i][0] = 0;//先清零，后使用
            temp_l[i][1] = 0;//先清零，后使用
        }

        //左边判断
        for (i = 0; i < 8; i++)
        {
            if (image[search_filds_l[i][1]][search_filds_l[i][0]] == 0
                && image[search_filds_l[(i + 1) & 7][1]][search_filds_l[(i + 1) & 7][0]] == 255)
            {
                temp_l[index_l][0] = search_filds_l[(i)][0];
                temp_l[index_l][1] = search_filds_l[(i)][1];
                index_l++;
                dir_l[l_data_statics - 1] = (i);//记录生长方向
            }



        }
        if (index_l)
        {
                //更新坐标点,选择 y 值最小的点（在图像上更高的点）
                center_point_l[0] = temp_l[0][0];//x    先赋值为第一次找到的中心点
                center_point_l[1] = temp_l[0][1];//y
                for (j = 1; j < index_l; j++)
                {
                    if (center_point_l[1] > temp_l[j][1])
                    {
                        center_point_l[0] = temp_l[j][0];//x
                        center_point_l[1] = temp_l[j][1];//y
                    }
                }
        }
        if ((points_r[r_data_statics][0]== points_r[r_data_statics-1][0]&& points_r[r_data_statics][0] == points_r[r_data_statics - 2][0]
            && points_r[r_data_statics][1] == points_r[r_data_statics - 1][1] && points_r[r_data_statics][1] == points_r[r_data_statics - 2][1])
            ||(points_l[l_data_statics-1][0] == points_l[l_data_statics - 2][0] && points_l[l_data_statics-1][0] == points_l[l_data_statics - 3][0]
                && points_l[l_data_statics-1][1] == points_l[l_data_statics - 2][1] && points_l[l_data_statics-1][1] == points_l[l_data_statics - 3][1]))
        {
            //printf("三次进入同一个点，退出\n");
            break;
        }
        if (my_abs(points_r[r_data_statics][0] - points_l[l_data_statics - 1][0]) < 2
            && my_abs(points_r[r_data_statics][1] - points_l[l_data_statics - 1][1] < 2)
            )
        {
            //printf("\n左右相遇退出\n");
            *hightest = (points_r[r_data_statics][1] + points_l[l_data_statics - 1][1]) >> 1;//取出最高点
            //printf("\n在y=%d处退出\n",*hightest);
            break;
        }
        if ((points_r[r_data_statics][1] < points_l[l_data_statics - 1][1]))
        {
           // printf("\n如果左边比右边高了，左边等待右边\n");
            continue;//如果左边比右边高了，左边等待右边
        }
        if (dir_l[l_data_statics - 1] == 7                                      //生长方向向下
            && (points_r[r_data_statics][1] > points_l[l_data_statics - 1][1]))//右边比左边高且已经向下生长了
        {
            //printf("\n左边开始向下了，等待右边，等待中... \n");
            center_point_l[0] = (uint8)points_l[l_data_statics - 1][0];//x
            center_point_l[1] = (uint8)points_l[l_data_statics - 1][1];//y
            l_data_statics--;
        }
        r_data_statics++;//索引加一

        index_r = 0;//先清零，后使用
        for (i = 0; i < 8; i++)
        {
            temp_r[i][0] = 0;//先清零，后使用
            temp_r[i][1] = 0;//先清零，后使用
        }

        //右边判断
        for (i = 0; i < 8; i++)
        {
            if (image[search_filds_r[i][1]][search_filds_r[i][0]] == 0
                && image[search_filds_r[(i + 1) & 7][1]][search_filds_r[(i + 1) & 7][0]] == 255)
            {
                temp_r[index_r][0] = search_filds_r[(i)][0];
                temp_r[index_r][1] = search_filds_r[(i)][1];
                index_r++;//索引加一
                dir_r[r_data_statics - 1] = (i);//记录生长方向
                //printf("dir[%d]:%d\n", r_data_statics - 1, dir_r[r_data_statics - 1]);
            }
            if (index_r)
            {

                //更新坐标点
                center_point_r[0] = temp_r[0][0];//x
                center_point_r[1] = temp_r[0][1];//y
                for (j = 0; j < index_r; j++)
                {
                    if (center_point_r[1] > temp_r[j][1])
                    {
                        center_point_r[0] = temp_r[j][0];//x
                        center_point_r[1] = temp_r[j][1];//y
                    }
                }

            }
        }
    }

    //取出循环次数
    *l_stastic = l_data_statics;
    *r_stastic = r_data_statics;
}

/*
函数名称：void get_left(uint16 total_L)
功能说明：从八邻域边界里提取需要的边线
参数说明：
total_L ：找到的点的总数
函数返回：无
修改时间：2022年9月25日
备    注：
example： get_left(data_stastics_l );
 */
uint8 l_border[image_h];//左线数组
uint8 r_border[image_h];//右线数组
uint8 l_border_buff[image_h];//左线缓存数组
uint8 r_border_buff[image_h];//右线缓存数组

uint8 center_line[image_h];//中线数组
void get_left(uint16 total_L)
{
    uint8 i = 0;
    uint16 j = 0;
    uint8 h = 0;
    //初始化
    for (i = 0;i<image_h;i++)
    {
        l_border[i] = border_min;
    }
    h = image_h - 2;
    //左边
    for (j = 0; j < total_L; j++)
    {
        //printf("%d\n", j);
        if (points_l[j][1] == h)
        {
            l_border[h] = (uint8)points_l[j][0];
        }
        else continue; //每行只取一个点，没到下一行就不记录
        h--;
        if (h == 0)
        {
            break;//到最后一行退出
        }
    }
}
/*
函数名称：void get_right(uint16 total_R)
功能说明：从八邻域边界里提取需要的边线
参数说明：
total_R  ：找到的点的总数
函数返回：无
修改时间：2022年9月25日
备    注：
example：get_right(data_stastics_r);
 */
void get_right(uint16 total_R)
{
    uint8 i = 0;
    uint16 j = 0;
    uint8 h = 0;
    for (i = 0; i < image_h; i++)
    {
        r_border[i] = border_max;//右边线初始化放到最右边，左边线放到最左边，这样八邻域闭合区域外的中线就会在中间，不会干扰得到的数据
    }
    h = image_h - 2;
    //右边
    for (j = 0; j < total_R; j++)
    {
        if (points_r[j][1] == h)
        {
            r_border[h] = (uint8)points_r[j][0];
        }
        else continue;//每行只取一个点，没到下一行就不记录
        h--;
        if (h == 0)break;//到最后一行退出
    }
}

//定义膨胀和腐蚀的阈值区间
#define threshold_max   255*5//此参数可根据自己的需求调节
#define threshold_min   255*2//此参数可根据自己的需求调节
void image_filter(uint8(*bin_image)[image_w])//形态学滤波，简单来说就是膨胀和腐蚀的思想
{
    uint16 i, j;
    uint32 num = 0;
    for (i = 1; i < image_h - 1; i++)
    {
        for (j = 1; j < (image_w - 1); j++)
        {
            //统计八个方向的像素值
            num =
                bin_image[i - 1][j - 1] + bin_image[i - 1][j] + bin_image[i - 1][j + 1]
                + bin_image[i][j - 1] + bin_image[i][j + 1]
                + bin_image[i + 1][j - 1] + bin_image[i + 1][j] + bin_image[i + 1][j + 1];
            if (num >= threshold_max && bin_image[i][j] == black_pixel)
            {
                bin_image[i][j] = white_pixel;//白  可以搞成宏定义，方便更改
            }
            if (num <= threshold_min && bin_image[i][j] == white_pixel)
            {
                bin_image[i][j] = black_pixel;//黑
            }
        }
    }
}

/*
函数名称：void image_draw_rectan(uint8(*image)[image_w])
功能说明：给图像画一个黑框
参数说明：uint8(*image)[image_w] 图像首地址
函数返回：无
修改时间：2022年9月8日
备    注：
example： image_draw_rectan(bin_image);
 */
void image_draw_rectan(uint8(*image)[image_w])
{

    uint8 i = 0;
    for (i = 0; i < image_h; i++)
    {
        image[i][0] = 0;
        image[i][1] = 0;
        image[i][image_w - 1] = 0;
        image[i][image_w - 2] = 0;

    }
    for (i = 2; i < image_w-2; i++)
    {
        image[0][i] = 0;
        image[1][i] = 0;
        //image[image_h-1][i] = 0;

    }
}

/*  判断丢线*/
//y轴5~55左边线在x轴的值<4和右边线在x轴的值>116都被判定为丢线
uint8 l_lost=0,r_lost=0,l_point_min=0,r_point_min=0;

void lost_num()
{
    uint8 i=0,j=0,count=0,map=58;
    l_point_min=58;
    r_point_min=58;
    l_lost=0;
    r_lost=0;
    if(data_stastics_l>0)
    {
        for(j=5;j<func_limit_ab(data_stastics_l-2, 10, 100);j++)
        {
            if(points_l[j][1]<map)
            {
                map=(uint8)points_l[j][1];
            }
        }
        l_point_min=map;

        for(i=map;i<55;i++)
        {
             if(l_border[i]<5)
             {
                 count++;
             }
        }
        l_lost=count;
    }
    count=0;
    map=58;

    if(data_stastics_r>0)
    {
        for(j=5;j<func_limit_ab(data_stastics_r-2, 10, 100);j++)
        {
            if(points_r[j][1]<map)
            {
                map=(uint8)points_r[j][1];
            }
        }
        r_point_min=map;
        for(i=map;i<55;i++)
        {
             if(r_border[i]>115)
             {
                 count++;
             }
        }
        r_lost=count;
    }
}


uint8 l_turn_max=0,r_turn_max=0,l_turn_point=0,r_turn_point=0;
void l_turn_judge(uint8 high,uint8 size,uint8 max) //high：y的起始高度    size：找寻的两个边界点的距离  max：记录突变距离在max之上的突变点
{
    uint8 j,gap;
    l_turn_max=0;
    l_turn_point=0;

    if(data_stastics_l>0)
    {
        for(j=high;j>points_l[func_limit_ab(data_stastics_l-5, 10, 100)][1];j--)
          {
             gap=(uint8)my_abs(l_border[j]-l_border[j-size]);
             if(gap>max)//记录边线从下往上突变超过max的点
             {
                     l_turn_point = j;             //y值（突变点前面的点）
                     l_turn_max = gap;   //x值
                     break;
             }
          }
    }
}

void r_turn_judge(uint8 high,uint8 size,uint8 max)
{
    uint8 j,gap;
    r_turn_max=0;
    r_turn_point=0;

    if(data_stastics_r>0)
    {
        for(j=high;j>points_r[func_limit_ab(data_stastics_r-5, 10, 100)][1];j--)
        {
             gap=(uint8)my_abs(r_border[j]-r_border[j-size]);
             if(gap>max)//记录边线从下往上突变超过max的点
             {
                     r_turn_point = j;             //y值
                     r_turn_max = gap;   //x值
                     break;
             }
        }
    }
}

int l_turn_max_t=0,r_turn_max_t=0,l_turn_point_t=0,r_turn_point_t=0;
void l_turn_judge_limit(uint8 y_max,uint8 size,uint8 turn_max,uint8 y_min,uint8 turn_x_min) //y_max：y的起始高度 y_min：y的最大高度   size：找寻的两个边界点的距离  turn_max：记录突变距离在max之上的突变点  turn_x_min：突变后x的最小值
{
    int j,gap;
    l_turn_max_t=0;
    l_turn_point_t=0;

    if(data_stastics_l>0)
    {
        for(j=y_max;j>func_limit_ab(points_l[data_stastics_l-1][1]+8,y_min+size,59);j--)
        {
             gap=my_abs(l_border[j]-l_border[j-size]);
             if(gap>turn_max&&l_border[j-size]>=turn_x_min)//记录边线从下往上突变超过max的点
             {
                     l_turn_point_t = j;             //y值（突变点前面的点）
                     l_turn_max_t = gap;   //x值
                     break;
             }
        }
    }
}

void r_turn_judge_limit(uint8 y_max,uint8 size,uint8 turn_max,uint8 y_min,uint8 turn_x_max)
{
    int j,gap;
    r_turn_max_t=0;
    r_turn_point_t=0;

    if(data_stastics_r>0)
    {
        for(j=y_max;j>func_limit_ab(points_r[data_stastics_r-1][1]+8,y_min+size,59);j--)
          {
             gap=my_abs(r_border[j]-r_border[j-size]);
             if(gap>turn_max&&r_border[j-size]<=turn_x_max)//记录边线从下往上突变超过max的点
             {
                     r_turn_point_t = j;             //y值
                     r_turn_max_t = gap;   //x值
                     break;
             }
          }
    }
}

/*  框选区域进行白点判断   */
uint8 bai1,bai2,bai3,bai4,bai5,bai6;
void judge_bai(uint8 mode)
{
    uint8 i,n,count1=0,count2=0,count3=0,count4=0,count5=0,count6=0;
    //判断25行和30行的5~22列的白点数
    if(mode==1)
    {
             for(i=5;i<23;i++)//X
              {
                 if(bin_image[25][i] ==255)  //判断白点
                 {
                     count1++;
                 }
                 if(bin_image[30][i] ==255)  //判断白点
                 {
                     count1++;
                 }
               }
        bai1=count1;
    }
    //判断25行和30行的115~98列的白点数
      if(mode==2)
      {
           for(i=115;i>97;i--)//X
            {
               if(bin_image[25][i] == 255)  //判断白点
               {
                   count2++;
               }
               if(bin_image[30][i] == 255)  //判断白点
               {
                   count2++;
               }
             }
           bai2=count2;
       }
      //判断15，20，25行的50~69列的白点数
      if(mode==3)
      {
          for(i=50;i<70;i++)//X
           {
              if(bin_image[15][i] == 255)  //判断白点
              {
                  count3++;
              }
              if(bin_image[20][i] == 255)  //判断白点
              {
                  count3++;
              }
              if(bin_image[25][i] == 255)  //判断白点
              {
                  count3++;
              }
//              tft180_draw_point(i, 15, uesr_BLUE);
//              tft180_draw_point(i, 20, uesr_BLUE);
//              tft180_draw_point(i, 25, uesr_BLUE);
            }

          bai3=count3;
      }
      //判断15~45行的5~10列的白点数
      if(mode==4)
      {
          for(i=45;i>15;i--)
          {
              for(n=10;n>5;n--)
              {

                  if(bin_image[i][n]==255)
                  {
                      count4++;
                  }
              }
          }
          bai4=count4;
      }
      //判断15~45行的110~115列的白点数
      if(mode==5)
      {
          for(i=45;i>15;i--)
          {
              for(n=115;n>110;n--)
              {

                  if(bin_image[i][n]==255)
                  {
                      count5++;
                  }
              }
          }
          bai5=count5;
      }
      if(mode==6)
            {
                for(i=50;i<70;i++)//X
                 {
                    if(bin_image[5][i] == 255)  //判断白点
                    {
                        count6++;
                    }
                    if(bin_image[10][i] == 255)  //判断白点
                    {
                        count6++;
                    }
                    if(bin_image[15][i] == 255)  //判断白点
                    {
                        count6++;
                    }
      //              tft180_draw_point(i, 15, uesr_BLUE);
      //              tft180_draw_point(i, 20, uesr_BLUE);
      //              tft180_draw_point(i, 25, uesr_BLUE);
                  }

                bai6=count6;
            }
}


int center_hei;
void judge_center()//判断图像中间竖线的黑点数量
{
 int i,count=0;
    for(i=58;i>2;i--)
    {
        if(bin_image[i][60]==0)
        {
            count++;

        }

    }
    center_hei=count;
}

int wan_dao(void)
{
    int wan_dao=0,j;
    if(data_stastics_l>0)
    {
        for(j=10;j<func_limit_ab(data_stastics_l-5, 10, 100);j++)
        {
            if(points_l[j][0]<points_l[2][0])
            {
                wan_dao=1;

                break;
            }
        }
    }

    if(data_stastics_r>0)
    {
        for(j=10;j<func_limit_ab(data_stastics_r-5, 10, 100);j++)
        {
            if(points_r[j][0]>points_r[2][0])
            {
                wan_dao=1;

                break;
            }
        }
    }

    return wan_dao;
}



//计算斜率   1： 第一个点  2：第二个点  3：斜率  4：截距   5：斜率的正1负（-1）
void calculate_slope(uint8 *point_1,uint8 *point_2,float *k,int *b,int8 k_state)
{
    *k=k_state*(float)(my_abs(point_1[0]-point_2[0]))/(my_abs(point_1[1]-point_2[1]));
    *b=(int)(point_1[0]-(*k)*point_1[1]);
}

void calculate(uint8 *point_1,uint8 *point_2,float *k,float *b)
{
    if((point_1[0]-point_2[0]==0)||(point_1[1]-point_2[1]==0))
    {
        *k=-100;
        return;
    }
    *k=(float)(point_1[0]-point_2[0])/(point_1[1]-point_2[1]);
    *b=point_1[0]-(*k)*point_1[1];
}

uint8 right_circle_into_state=0;
void right_circle_judge(void)//判定右圆环
{                       //圆环进入的程度
    float k=0,b=0;
    static uint8 c1[2]={0},v1[2]={0},n1[2]={0},n2[2]={0}, n2_connect[2]={100,0},v2_connet[2]={100,58},v2[2]={0},guo=0;
    uint8 find_c1=0,find_v1=0,find_n2=0,find_v2=0,right_circle_error=0;
    uint8 v1_connet[2]={0};
    uint8 i=0,j=0;

    if(right_circle_into_state!=0)//显示屏显示进元素的状态
    {
        tft180_show_int                 (120,45, 3, 1);
        tft180_show_int                 (140,45, right_circle_into_state, 1);
    }

    switch(right_circle_into_state)
    {
        case 0:
        l_turn_judge_limit(55,5,7,10,0);//找突变点
        r_turn_judge_limit(55,5,15,10,119);

        if(data_stastics_l>0)
        {
            for(j=10;j<func_limit_ab(data_stastics_l-5, 10, 100);j++)
            {
                if(points_l[j][0]<points_l[2][0])
                {
                    right_circle_error=1;
                    break;
                }
            }
        }

        if(right_circle_error==0)//&&bin_image[][(points_l[2][0]+points_r[2][0])/2][]==255
        {
            if(r_turn_max_t>0&&r_turn_point_t>25&&r_border[r_turn_point_t-5]>r_border[r_turn_point_t]&&l_border[4]-l_border[10]>0&&l_border[4]-l_border[10]<10)//&&r_border[r_turn_point_t-5]>r_border[r_turn_point_t]&&l_border[4]-l_border[10]>0&&l_border[4]-l_border[10]<10多了这个
            {
                if(l_turn_max_t<5&&l_lost<5&&r_lost>5&&r_turn_max_t>15&&r_border[r_turn_point_t-5]>80&&l_point_min<15&&r_point_min<15)
                {
                    for(j=5;j<(data_stastics_r-10);j++)//从下边开始往上找
                    {
                      if((points_r[j][1]<points_r[j+10][1])&&(points_r[j][1]<points_r[j-10][1]))//找与突变点的坐标相同的轮廓点
                      {
                          if(points_r[j][1]>func_limit_ab(r_turn_point_t-10,10,55))
                          {
                              right_circle_into_state=1;
                              gpio_high(P33_10);
                              break;
                          }

                      }
                    }
                }
            }
        }
    break;
    case 1:
        gpio_low(P33_10);
        l_turn_judge(58,5,10);//找突变点
        r_turn_judge(58,5,10);
        r_border[3]=60;
        for(j=r_turn_point-5;j>5;j--)//进环找c1点
        {
            if(bin_image[j][r_border[r_turn_point]]-bin_image[j-1][r_border[r_turn_point]]==255)
            {
                for(i=r_border[r_turn_point];i>30;i--)
                {
                    if(bin_image[j-2][i-1]-bin_image[j-2][i]==255)
                    {
                        c1[0]=i-1;
                        c1[1]=j-2;
                        find_c1=1;
                        break;
                    }
                }
                if(find_c1==1)
                    break;
            }
        }

        n1[0]=r_border[r_turn_point];//开始进入的右突变点
        n1[1]=r_turn_point+1;

        calculate(n1,c1,&k,&b);

        for(j=n1[1];j>c1[1];j--)
        {
            r_border[j]=(uint8)limit_a_b((uint8)(k*j+b),3,117);
        }

        if(r_turn_point>=50)//如果右下点>>50，更新为状态2  ||bin_image[58][110]==255
        {
            gpio_high(P33_10);
            right_circle_into_state=2;

        }
    break;
    case 2:
        r_border[3]=70;
        n1[1]=59;
        for(j=45;j>10;j--)
        {
            if(r_border[j]<r_border[j+5]&&r_border[j]<r_border[j-5])
            {
                c1[0]=func_limit_ab(r_border[j], 40, 70);
                c1[1]=j;
                break;
            }
        }
        calculate(n1,c1,&k,&b);

        for(j=59;j>c1[1];j--)
        {
            r_border[j]=(uint8)limit_a_b((uint8)(k*j+b),3,117);
        }

        if(c1[1]>=30)
        {
            gpio_high(P33_10);
            right_circle_into_state=3;
        }

    break;
    case 3:
        r_border[3]=80;
        gpio_low(P33_10);
        //最上方有没有黑点
        if(guo==0)
        {
            for(j=10;j<=(data_stastics_r-6);j++)
            {
                if((points_r[j][1]>points_r[j-5][1])&&(points_r[j][1]>points_r[j+5][1]))//找y值比较大的点
                {//tft180_show_int                 (65,90, 7, 1);
                    if(points_r[j][1]>5&&points_r[j][0]<110)
                    {
                        find_v1=1;
                        v1[0]=(uint8)points_r[j][0];//v点
                        v1[1]=(uint8)points_r[j][1];
                        //tft180_show_int                 (25,90, v_point[0], 2);
                        //tft180_show_int                 (45,90, v_point[1], 2);
                        break;
                    }
                 }
            }
            if(find_v1==0)
            {
                for(j=10;j<=(data_stastics_l-6);j++)
                {
                    if((points_l[j][1]>points_l[j-5][1])&&(points_l[j][1]>points_l[j+5][1]))//找y值比较大的点
                    {
                        if(points_l[j][1]<115)
                        {
                            find_v1=1;
                            v1[0]=(uint8)points_l[j][0];//v点
                            v1[1]=(uint8)points_l[j][1];
                            break;
                         }
                     }
                }
            }
            if(find_v1==1)
            {
                v1_connet[0]=(uint8)points_l[1][0];//右下点
                v1_connet[1]=(uint8)points_l[1][1];

                calculate(v1_connet,v1,&k,&b);

                for(j=59;j>10;j--)//补线
                {
                    l_border[j]=(uint8)limit_a_b((uint8)(k*j+b),3,117);
                }
                if(v1[1]>45)
                {
                    gpio_high(P33_10);
                    guo=1;
                }
            }
        }
        if(guo==1)
        {
            if(points_l[1][0]>5)
            right_circle_into_state=4;
            v1[0]=118;
            v1[1]=0;
            calculate(v1_connet,v1,&k,&b);
            for(j=59;j>10;j--)//补线
            {
                l_border[j]=(uint8)limit_a_b((uint8)(k*j+b),3,117);
            }

        }
    break;
    case 4:
        r_border[3]=90;
        gpio_low(P33_10);
//        tft180_show_int                 (5,100, data_stastics_r, 3);
        for(j=7;j<data_stastics_l-7;j++)//判定右边线有没有向左凸的点
        {
            if(points_l[j][0]>points_l[j+5][0]&&points_l[j][0]>points_l[j-5][0])
            {
                if(points_l[j][0]<70)
                {

                   find_n2=1;

                   n2[0]=(uint8)points_l[j][0];
                   n2[1]=(uint8)points_l[j][1];

                   break;
                 }
            }
        }

        if(find_n2==1)
        {
            calculate(n2,n2_connect,&k,&b);

            for(j=n2[1];j>0;j--)//补线
            {
               l_border[j]=(uint8)limit_a_b((uint8)(k*j+b),3,117);
            }

        }
        if(n2[1]>45)
        {
            gpio_high(P33_10);
            right_circle_into_state=5;
        }


    break;
    case 5:
        r_border[3]=100;
        gpio_low(P33_10);
        r_turn_judge(58,5,10);//找突变点

        n2[0]=1;
        n2[1]=59;

        calculate(n2,n2_connect,&k,&b);

        for(j=n2[1];j>0;j--)//补线
        {
           l_border[j]=(uint8)limit_a_b((uint8)(k*j+b),3,117);
        }
        tft180_show_int                 (100,70, r_lost, 2);
        if(r_turn_max>10&&l_lost<15)
        {
            gpio_high(P33_10);
            right_circle_into_state=6;
        }

    break;
    case 6:
        r_border[3]=110;
        gpio_low(P33_10);
        //v点在左边线
        for(j=10;j<=(data_stastics_r-6);j++)
        {
            if((points_r[j][1]>points_r[j-5][1])&&(points_r[j][1]>points_r[j+5][1]))//找y值比较大的点
            {
                if((points_r[j][0]<110)&&(points_r[j][1]<115))
                {
                    find_v2=1;
                    v2[0]=(uint8)points_r[j][0];//v点
                    v2[1]=(uint8)points_r[j][1];

                    break;
                }
            }
        }
        if(find_v2==1)
        {
            calculate(v2,v2_connet,&k,&b);

            for(j=59;j>=v2[1];j--)//补线
            {
                r_border[j]=(uint8)limit_a_b((uint8)(k*j+b),3,117);
            }

                tft180_show_int                 (45,60, v2[0], 2);
                tft180_show_int                 (45,80, v2[1], 2);
            if(v2[1]>=45)
            {
                gpio_high(P33_10);
                right_circle_into_state=7;
            }
        }

    break;
    case 7:
        r_border[3]=115;
        gpio_low(P33_10);
        //if((l_border[1]>=10)&&(r_border[1]<=110 ))
        if(points_l[1][0]>5)
        {
            right_circle_into_state=0;
            finish=1;
            find_n2=0;
            find_v2=0;
            guo=0;
            aaa=1;
            for(i=0;i<2;i++)
            {
                c1[i]=0;
                v1[i]=0;
                n1[i]=0;
                n2[i]=0;
                v2[i]=0;
            }
        }
    break;
    }
}

uint8 left_circle_into_state=0;
void left_circle_judge(void)//判定左圆环
{                       //圆环进入的程度


    float k=0,b=0;
    static uint8 c1[2]={0},v1[2]={0},n1[2]={0},n2[2]={0}, n2_connect[2]={20,0},v2_connet[2]={20,58},v2[2]={0},guo=0;
    uint8 find_c1=0,find_v1=0,find_n2=0,find_v2=0,left_circle_error=0;
    uint8 v1_connet[2]={0};
    uint8 i=0,j=0;

    if(left_circle_into_state!=0)//显示屏显示进元素的状态
    {
        tft180_show_int                 (120,45, 2, 1);
        tft180_show_int                 (140,45, left_circle_into_state, 1);
    }

//tft180_show_int                 (5,60, l_turn_point, 2);
//tft180_show_int                 (5,80, l_turn_max, 2);
//tft180_show_int                 (25,60, l_point_min, 2);
//tft180_show_int                 (25,80, r_point_min, 2);
//tft180_show_int                 (25,60, l_border[l_turn_point], 2);

    switch(left_circle_into_state)
    {
    case 0:
        l_turn_judge_limit(55,5,15,10,0);//找突变点
        r_turn_judge_limit(55,5,7,10,119);
        if(data_stastics_r>0)
        {
            for(j=10;j<func_limit_ab(data_stastics_r-5, 10, 100);j++)
            {
                if(points_r[j][0]>points_r[2][0])
                {
                    left_circle_error=1;
                    break;
                }
            }
        }
        if(left_circle_error==0)
        {
            if(l_turn_max_t>0&&l_turn_point_t>25&&l_border[l_turn_point_t-5]<l_border[l_turn_point_t]&&r_border[10]-r_border[4]>0&&r_border[10]-r_border[4]<10)//&&l_border[r_turn_point_t-5]<r_border[r_turn_point_t]多了这个
            {
            //tft180_show_int                 (25,100, l_turn_max_t, 2);
                if(r_turn_max_t<5&&l_turn_max_t>15&&r_lost<5&&l_lost>5&&l_border[l_turn_point_t-5]<40&&l_point_min<15&&r_point_min<15)
                {//tft180_show_int                 (55,80, 2, 1);
                       for(j=5;j<(data_stastics_l-10);j++)//从下边开始往上找
                       {
                             if((points_l[j][1]<points_l[j+10][1])&&(points_l[j][1]<points_l[j-10][1]))//找与突变点的坐标相同的轮廓点
                             {//tft180_show_int                 (55,100, 3, 1);
                                 if(points_l[j][1]>func_limit_ab(l_turn_point_t-10,10,55))
                                 {

                                     //tft_draw_circle(func_limit_ab(points_l[j][0], 10, 110), func_limit_ab(points_l[j][1],10,50), uesr_RED);
                                     left_circle_into_state=1;
                                     gpio_high(P33_10);
                                     break;
                                 }

                             }
                       }

                 }
            }
        }
    break;
    case 1:
        l_border[2]=10;
        gpio_low(P33_10);
        l_turn_judge(58,5,10);//找突变点
        r_turn_judge(58,5,10);

        for(j=l_turn_point-5;j>5;j--)//进环找c1点
        {
            if(bin_image[j][l_border[l_turn_point]]-bin_image[j-1][l_border[l_turn_point]]==255)
            {
                for(i=l_border[l_turn_point];i<90;i++)
                {
                    if(bin_image[j-2][i+1]-bin_image[j-2][i]==255)
                    {
                        c1[0]=i+1;
                        c1[1]=j-2;
                        find_c1=1;
                        break;
                    }
                }
                if(find_c1==1)
                    break;
            }
        }

        n1[0]=l_border[l_turn_point];//开始进入的左突变点
        n1[1]=l_turn_point+1;

        calculate(n1,c1,&k,&b);

        for(j=n1[1];j>10;j--)//j=n1[1];j>c1[1];j--
        {
            l_border[j]=(uint8)limit_a_b((uint8)(k*j+b),3,117);
        }

        if(l_turn_point>=50)//如果左下点>>50，更新为状态2
        {
            gpio_high(P33_10);
            left_circle_into_state=2;

        }
    break;
    case 2:
        l_border[2]=20;
        gpio_low(P33_10);
        n1[1]=59;
        for(j=45;j>10;j--)
        {
            if(l_border[j]>l_border[j+5]&&l_border[j]>l_border[j-5])
            {
                c1[0]=l_border[j];
                c1[1]=j;
                break;
            }
        }
        calculate(n1,c1,&k,&b);



        for(j=59;j>10;j--)//j=59;j>c1[1];j--
        {
            l_border[j]=(uint8)limit_a_b((uint8)(k*j+b),3,117);
        }

        if(c1[1]>=30)
        {
            gpio_high(P33_10);
            left_circle_into_state=3;
        }

    break;
    case 3:
        l_border[2]=30;
        gpio_low(P33_10);
        //最上方有没有黑点
        if(guo==0)
        {
            for(j=10;j<=(data_stastics_l-6);j++)
            {
                if((points_l[j][1]>points_l[j-5][1])&&(points_l[j][1]>points_l[j+5][1]))//找y值比较大的点
                {//tft180_show_int                 (65,90, 7, 1);
                    if(points_l[j][1]>5&&points_l[j][0]>10)
                    {
                        find_v1=1;
                        v1[0]=(uint8)points_l[j][0];//v点
                        v1[1]=(uint8)points_l[j][1];
                        //tft180_show_int                 (25,90, v_point[0], 2);
                        //tft180_show_int                 (45,90, v_point[1], 2);
                        break;
                    }
                 }
            }
            if(find_v1==0)
            {
                for(j=10;j<=(data_stastics_r-6);j++)
                {
                    if((points_r[j][1]>points_r[j-5][1])&&(points_r[j][1]>points_r[j+5][1]))//找y值比较大的点
                    {
                        if(points_r[j][1]>5)
                        {
                            find_v1=1;
                            v1[0]=(uint8)points_r[j][0];//v点
                            v1[1]=(uint8)points_r[j][1];
                            break;
                         }
                     }
                }
            }
            if(find_v1==1)
            {
                v1_connet[0]=(uint8)points_r[1][0];//右下点
                v1_connet[1]=(uint8)points_r[1][1];

                calculate(v1_connet,v1,&k,&b);

                for(j=59;j>10;j--)//补线
                {
                    r_border[j]=(uint8)limit_a_b((uint8)(k*j+b),3,117);
                }
    //            for(j=v1_connet[1];j>10;j--)
    //            {
    //                l_border[j]=limit_a_b(5,3,117);
    //            }
                if(v1[1]>45)
                {
                    gpio_high(P33_10);
                    guo=1;
                }
            }
        }
        if(guo==1)
        {
            if(points_r[1][0]<115)
            left_circle_into_state=4;
            v1[0]=0;
            v1[1]=0;
            calculate(v1_connet,v1,&k,&b);
            for(j=59;j>10;j--)//补线
            {
                r_border[j]=(uint8)limit_a_b((uint8)(k*j+b),3,117);
            }

        }



    break;
    case 4:
        l_border[2]=40;
        gpio_low(P33_10);
//        tft180_show_int                 (5,100, data_stastics_r, 3);
        for(j=7;j<data_stastics_r-7;j++)//判定右边线有没有向左凸的点
        {
            if(points_r[j][0]<points_r[j+5][0]&&points_r[j][0]<points_r[j-5][0])
            {
                if(points_r[j][0]>50)
                {

                   find_n2=1;

                   n2[0]=(uint8)points_r[j][0];
                   n2[1]=(uint8)points_r[j][1];

                   break;
                 }
            }
        }


        if(find_n2==1)
        {
            calculate(n2,n2_connect,&k,&b);

            for(j=n2[1];j>0;j--)//补线
            {
               r_border[j]=(uint8)limit_a_b((uint8)(k*j+b),3,117);
            }

        }
//        for(j=45;j<55;j++)
//        {
//            if(bin_image[j][110]==0)
//            {
//                left_circle_error=1;
//            }
//
//        }
        if(n2[1]>45)
        {
            gpio_high(P33_10);
            left_circle_into_state=5;
        }


    break;
    case 5:
        l_border[2]=50;
        gpio_low(P33_10);
        l_turn_judge(58,5,10);//找突变点

        n2[0]=118;
        n2[1]=59;

        calculate(n2,n2_connect,&k,&b);

        for(j=n2[1];j>0;j--)//补线
        {
           r_border[j]=(uint8)limit_a_b((uint8)(k*j+b),3,117);
        }
        tft180_show_int                 (100,70, r_lost, 2);
        if(l_turn_max>10&&r_lost<15)
        {
            gpio_high(P33_10);
            left_circle_into_state=6;
        }



    break;
    case 6:
        l_border[2]=60;
        gpio_low(P33_10);
        //v点在左边线
        for(j=10;j<=(data_stastics_l-6);j++)
        {
            if((points_l[j][1]>points_l[j-5][1])&&(points_l[j][1]>points_l[j+5][1]))//找y值比较大的点
            {
                if((points_l[j][0]>10)&&(points_l[j][1]>5))
                {
                    find_v2=1;
                    v2[0]=(uint8)points_l[j][0];//v点
                    v2[1]=(uint8)points_l[j][1];

                    break;
                }
            }
        }
        if(find_v2==1)
        {
            calculate(v2,v2_connet,&k,&b);

            for(j=59;j>=v2[1]-5;j--)//补线
            {
                l_border[j]=(uint8)limit_a_b((uint8)(k*j+b),3,117);
            }

                tft180_show_int                 (45,60, v2[0], 2);
                tft180_show_int                 (45,80, v2[1], 2);
            if(v2[1]>=45)
            {
                gpio_high(P33_10);
                left_circle_into_state=7;
            }
        }

    break;
    case 7:
        l_border[2]=70;
        gpio_low(P33_10);
        //if((l_border[1]>=10)&&(r_border[1]<=110 ))
        if(points_l[1][0]>5)
        {
            aaa=1;
            left_circle_into_state=0;
            finish=1;
            find_n2=0;
            find_v2=0;
            guo=0;
            for(i=0;i<2;i++)
            {
                c1[i]=0;
                v1[i]=0;
                n1[i]=0;
                n2[i]=0;
                v2[i]=0;
            }
        }
    break;
    }
}

int crossroad_into_state=0;
void crossroad_judge()//十字路口判定
{
    int i=0,j=0,find_l=0,find_r=0;
    float k1=0,k2=0,k1_buff=0,k2_buff=0;
    float b1=0,b2=0;
    int8 a=1,l_error=0,r_error=0;
    uint8 point1[2]={0},point2[2]={0};
    float k=0;
    int b=0;
    uint8 black=0;
    static uint8 ten_below_l[2]={0},ten_up_l[2]={0},ten_l_below[2]={0},ten_r_below[2]={0},ten_below_r[2]={0},ten_up_r[2]={0};

    static uint8 x_l,x_r,y_l,y_r;


    //tft180_show_int                 (5,80, l_lost, 2);
    //tft180_show_int                 (5,100, r_lost, 2);
//    tft180_show_int                 (30,80, flag_turn_L, 2);
//    tft180_show_int                 (30,100, flag_turn_R, 2);

    if(crossroad_into_state!=0)//显示屏显示进元素的状态
    {
        //tft180_show_int                 (120,45, 1, 1);
        //tft180_show_int                 (140,45, crossroad_into_state, 1);
        r_border[3]=100;
    }


    switch(crossroad_into_state)
    {
    case 0:
        //tft180_show_int                 (5,60, l_lost, 2);
                   // tft180_show_int                 (5,80, r_lost, 2);
        if(l_lost>=5&&r_lost>=5)//左右丢线
        {
//            l_turn_judge(55,5,10);//找突变点
//            r_turn_judge(55,5,10);
            l_turn_judge_limit(55,5,10,10,0);
            r_turn_judge_limit(55,5,10,10,119);


            if(l_turn_point_t>0||r_turn_point_t>0)
            {//tft_draw_circle(func_limit_ab(l_border[l_turn_point_t], 10, 110), func_limit_ab(l_turn_point_t,10,50), uesr_RED);
            //tft_draw_circle(func_limit_ab(r_border[r_turn_point_t], 10, 110), func_limit_ab(r_turn_point_t,10,50), uesr_RED);
                if(l_border[l_turn_point_t-5]<l_border[l_turn_point_t]&&r_border[r_turn_point_t-5]>r_border[r_turn_point_t])
                {//tft180_show_int                 (50,60, 1, 1);
                    if(func_abs(l_turn_point_t-r_turn_point_t)<20)//2突变点的高度不能超过20
                    {//tft180_show_int                 (50,80, 2, 1);
                        if(l_turn_max_t>5&&r_turn_max_t>5)
                        {//tft180_show_int                 (50,100, 3, 1);
                            if(l_turn_point_t>r_turn_point_t)
                                a=-1;
                            point1[0]=l_border[l_turn_point_t];
                            point1[1]=(uint8)l_turn_point_t;
                            point2[0]=r_border[r_turn_point_t];
                            point2[1]=(uint8)r_turn_point_t;
                            calculate_slope(point1,point2,&k,&b,a);





//                            for(i=20;i<100;i++)
//                            {
//                                y=limit_a_b((uint8)((i-b)/k)-10,5,55);
////                                if(bin_image[y][i]==0)
////                                {
////                                    black=1;
////                                    tft180_show_int                 (70,60, 4, 1);
////                                    k_1=-1/k;
////                                    break;
////                                }
//                               tft180_draw_point(i, y, uesr_RED);
//                            }
                            if(black==0)
                            {
                                crossroad_into_state=1;
                                gpio_high(P33_10);
                            }
                        }
                    }
                }
            }
        }
    break;
    case 1:
        l_border[3]=10;
        gpio_low(P33_10);
//        for(i=60;i>=5;i--)//从y=59，x=60向左右两边寻找白变黑的点
//        {
//            if(bin_image[59][i]-bin_image[59][i-1]==255)
//            {
//                ten_below_l[0]=i-1;
//                ten_below_l[1]=59;
//                //tft_draw_circle(i-1, 59, uesr_RED);
//                //tft180_show_int                 (35,90,  i-1, 2);
//                break;
//            }
//        }
//        for(i=60;i<=115;i++)
//        {
//            if(bin_image[59][i]-bin_image[59][i+1]==255)
//            {
//                ten_below_r[0]=i+1;
//                ten_below_r[1]=59;
//                //tft_draw_circle(i+1, 59, uesr_RED);
//                break;
//            }
//        }
//
//        for(j=55;j>=5;j--)//y=55，x=下左（右）找到的黑点，向上找黑变白的点
//        {
//            if(bin_image[j-1][ten_below_l[0]]-bin_image[j][ten_below_l[0]]==255)
//            {
//                //tft_draw_circle(ten_below_l[0], j-1, uesr_RED);
//                for(i=ten_below_l[0];i<80;i++)//y=找到的点的值-5，x=下左找到的黑点，向右找黑变白的点
//                {
//                    if(bin_image[j+5][i+1]-bin_image[j+5][i]==255)
//                    {
//                        find_l=1;
//                        ten_l_below[0]=i+1;
//                        ten_l_below[1]=j+5;
//                      //  tft_draw_circle(i+1, j+5, uesr_RED);
//                        //tft180_show_int                 (5,90,  ten_l_below[0], 2);
//                        break;
//                    }
//                }
//            }
//
//            if(bin_image[j-1][ten_below_r[0]]-bin_image[j][ten_below_r[0]]==255)
//            {
//                for(i=ten_below_r[0];i>30;i--)//y=找到的点的值-5，x=下右找到的黑点，向左找黑变白的点
//                {
//                    if(bin_image[j+5][i-1]-bin_image[j+5][i]==255)
//                    {
//                        find_r=1;
//                        ten_r_below[0]=i-1;
//                        ten_r_below[1]=j+5;
//                       // tft_draw_circle(i-1, j+5, uesr_RED);
//                        break;
//                    }
//                }
//            }

            l_turn_judge(57,5,10);
            r_turn_judge(57,5,10);

            if(l_turn_max>=10&&r_turn_max>=10)//如果左（右）下点都找到，开始补线
            {
                point1[0]=(uint8)points_l[2][0];
                point1[1]=(uint8)points_l[2][1];
                point2[0]=l_border[l_turn_point];
                point2[1]=(uint8)l_turn_point;
                //tft180_show_int                 (60,80, point2[0], 2);
                               // tft180_show_int                 (60,100, point2[1], 2);
                calculate(point1,point2,&k1,&b1);

                point1[0]=(uint8)points_r[2][0];
                point1[1]=(uint8)points_r[2][1];
                point2[0]=r_border[r_turn_point];
                point2[1]=(uint8)r_turn_point;

                calculate(point1,point2,&k2,&b2);

                if(k1!=k1_buff&&k2!=k2_buff)
                {
                    k1_buff=k1;
                    k2_buff=k2;
                }


                if(k1_buff!=-100&&k2_buff!=-100)
                {
                    //tft180_show_int                 (120,85, 2, 1);
                    for(j=l_turn_point+3;j>10;j--)//补线
                    {
                        l_border[j]=(uint8)limit_a_b((uint8)(k1*j+b1),3,117);
                        //tft180_draw_point(l_border[j], j, uesr_RED);
                    }
                    for(j=r_turn_point+3;j>10;j--)//补线
                    {
                        r_border[j]=(uint8)limit_a_b((uint8)(k2*j+b2),3,117);
                        //tft180_draw_point(r_border[j], j, uesr_RED);
                    }

                }
             }



        if(l_turn_point>=45||r_turn_point>=45)//如果y=5的x=60为白，x=30和90为黑
        {
            crossroad_into_state=2;
        }
    break;
    case 2:
        l_turn_judge(58,5,10);
        r_turn_judge(58,5,10);
        l_border[3]=20;
        for(j=l_turn_point;j>=5;j--)//y=58，x=4和115，向上找白变黑的点
        {
            if(bin_image[j][l_border[l_turn_point]]-bin_image[j-1][l_border[l_turn_point]]==255)
            {
                for(i=l_border[l_turn_point];i<80;i++)
                {
                    if(bin_image[j-3][i+1]-bin_image[j-3][i]==255)//向右找黑变白的点
                    {
                        find_l=1;
                        ten_up_l[0]=i+1;
                        ten_up_l[1]=j-3;
                       // tft180_show_int                 (25,80, ten_up_l[0], 2);
                       // tft180_show_int                 (25,100, ten_up_l[1], 2);


                        break;
                    }
                }
                if(find_l==1)
                    break;
            }
        }

        for(j=r_turn_point;j>=5;j--)//y=58，x=4和115，向上找白变黑的点
        {
            if(bin_image[j][r_border[r_turn_point]]-bin_image[j-1][r_border[r_turn_point]]==255)
            {
                for(i=r_border[r_turn_point];i>40;i--)
                {
                    if(bin_image[j-3][i-1]-bin_image[j-3][i]==255)//向左找黑变白的点
                    {
                        find_r=1;
                        ten_up_r[0]=i+1;
                        ten_up_r[1]=j-3;

                        break;
                    }
                }
                if(find_r==1)
                    break;
            }
        }
            if(find_l==1&&find_r==1)//如果上左（右）都找到，开始补线
            {
                ten_below_l[0]=l_border[l_turn_point];
                ten_below_l[1]=(uint8)l_turn_point;
                ten_below_r[0]=r_border[r_turn_point];
                ten_below_r[1]=(uint8)r_turn_point;
                calculate( ten_below_l,ten_up_l,&k1,&b1 );
                calculate( ten_below_r,ten_up_r,&k2,&b2 );
                for(j=l_turn_point;j>ten_up_l[1];j--)//补线
                {
                    l_border[j]=(uint8)limit_a_b((uint8)(k1*j+b1),3,117);

                }
                for(j=r_turn_point;j>ten_up_r[1];j--)//补线
                {
                    r_border[j]=(uint8)limit_a_b((uint8)(k2*j+b2),3,117);
                }


            }


            for(j=58;j>50;j--)
            {
                if(bin_image[j][10]==0)
                {
                    l_error=1;
                }
                if(bin_image[j][110]==0)
                {
                    r_error=1;
                }
            }
            if(l_turn_point>=50||r_turn_point>=50||l_error==0||r_error==0)
            //if(bin_image[57][20])
            {
                x_l=l_border[l_turn_point];
                x_r=r_border[r_turn_point];
                y_l=(uint8)l_turn_point;
                y_r=(uint8)r_turn_point;
                crossroad_into_state=3;
            }
    break;
    case 3:
        l_border[3]=30;

//        if(points_l[1][0]<5&&points_r[1][0]<115)
//        {
//            find_l=1;
//        }
//        for(i=50;i>20;i--)
//                    {
//
//                    }
        for(j=y_l;j>=5;j--)//y=58，x=4和115，向上找白变黑的点
        {
            if(bin_image[j][x_l]-bin_image[j-1][x_l]==255)
            {
                for(i=x_l;i<80;i++)
                {
                    if(bin_image[j-3][i+1]-bin_image[j-3][i]==255)//向右找黑变白的点
                    {
                        find_l=1;
                        ten_up_l[0]=i+1;
                        ten_up_l[1]=j-3;
                        //tft180_show_int                 (25,80, ten_up_l[0], 2);
                        //tft180_show_int                 (25,100, ten_up_l[1], 2);
                        //tft_draw_circle(ten_up_l[0], ten_up_l[1], uesr_RED);

                        break;
                    }
                }
                if(find_l==1)
                    break;
            }
        }

        for(j=y_r;j>=5;j--)//y=58，x=4和115，向上找白变黑的点
        {
            if(bin_image[j][x_r]-bin_image[j-1][x_r]==255)
            {
                for(i=x_r;i>40;i--)
                {
                    if(bin_image[j-3][i-1]-bin_image[j-3][i]==255)//向左找黑变白的点
                    {
                        find_r=1;
                        ten_up_r[0]=i+1;
                        ten_up_r[1]=j-3;
                        //tft_draw_circle(ten_up_r[0], ten_up_r[1], uesr_RED);
                        //tft180_show_int                 (5,90, i+1, 3);
                        break;
                    }
                }
                if(find_r==1)
                    break;
            }
        }

//            if(bin_image[j][115]-bin_image[j-1][115]==255)
//            {
//                for(i=115;i>30;i--)
//                {
//                    if(bin_image[j-5][i-1]-bin_image[j-5][i]==255)
//                    {
//                        find_r=1;
//                        ten_up_r[0]=i-1;
//                        ten_up_r[1]=j-5;
//                        break;
//                    }
//                }
//            }
            if(find_l==1&&find_r==1)//如果上左（右）都找到，开始补线
            {
                ten_below_l[0]=x_l;
                ten_below_l[1]=58;
                ten_below_r[0]=x_r;
                ten_below_r[1]=58;
                calculate( ten_below_l,ten_up_l,&k1,&b1 );
                calculate( ten_below_r,ten_up_r,&k2,&b2 );
                for(j=58;j>ten_up_l[1];j--)//补线
                {
                    l_border[j]=(uint8)limit_a_b((uint8)(k1*j+b1),10,40);

                }
                for(j=58;j>ten_up_r[1];j--)//补线
                {
                    r_border[j]=(uint8)limit_a_b((uint8)(k2*j+b2),60,110);
                }


            }

            if(ten_up_l[1]>=35||ten_up_r[1]>=35||l_lost<5||r_lost<5)//||l_lost<5||r_lost<5
            {l_border[3]=40;
                crossroad_into_state=0;
                ten_below_l[0]=0;
                ten_below_l[1]=0;
                ten_below_r[0]=0;
                ten_below_r[1]=0;
                ten_up_l[0]=0;
                ten_up_l[1]=0;
                ten_up_r[0]=0;
                ten_up_r[1]=0;
                ten_l_below[0]=0;
                ten_l_below[1]=0;
                ten_r_below[0]=0;
                ten_r_below[1]=0;
                x_l=0;
                x_r=0;
                y_l=0;
                y_r=0;
                finish=1;
                l_border[3]=40;
            }
//        if(bin_image[58][3]==0||bin_image[58][116]==0)//如果y=58，x=3或116其中一点为黑，则过十字路口
//        {
//            crossroad_into_state=0;
//            ten_below_l[0]=0;
//            ten_below_l[1]=0;
//            ten_below_r[0]=0;
//            ten_below_r[1]=0;
//            ten_up_l[0]=0;
//            ten_up_l[1]=0;
//            ten_up_r[0]=0;
//            ten_up_r[1]=0;
//            ten_l_below[0]=0;
//            ten_l_below[1]=0;
//            ten_r_below[0]=0;
//            ten_r_below[1]=0;
//
//        }
    }
}

int judge_flage=0;//跳跃标志位
uint8 hight1=0,hight2=0,middle_point=0;
uint8 jump_flaged=0,start_jump=0;
void leap_judge()//判断是否为需要跳跃的障碍物
{
    uint16 j=0,i=0,judge_error=0,judge1=0,t=0;


    if(judge_flage!=0)//显示屏显示进元素的状态
    {
        tft180_show_int                 (120,45, 5, 1);
        tft180_show_int                 (140,45, judge_flage, 1);
    }
//    tft180_show_int                 (5,60, points_l[data_stastics_l-1][0], 2);
//    tft180_show_int                 (5,80, points_l[data_stastics_l-1][1], 2);
//    tft180_show_int                 (25,60, points_r[data_stastics_r-1][0], 2);
//    tft180_show_int                 (25,80, points_r[data_stastics_r-1][1], 2);

    switch(judge_flage)
    {
    case 0:

            l_turn_judge_limit(55,5,5,points_l[data_stastics_l-1][1]+5,40);
            r_turn_judge_limit(55,5,5,points_r[data_stastics_r-1][1]+5,80);

            if(l_turn_point_t>0||r_turn_point_t>0)
            {
                //tft180_show_int                 (55,60, 1, 2);
                judge_error=1;
                l_border[3]=10;
            }


if(data_stastics_l>0&&data_stastics_r>0)
{
            if(points_l[data_stastics_l-1][1]>=20&&points_r[data_stastics_r-1][1]>=20&&r_lost<5&&l_lost<5)
            {l_border[3]=20;
                for(j=2;j<data_stastics_l-5;j++)//如果有左边线点小于第1个左边线点，错误
                {
                    if(points_l[1][0]>points_l[j][0])
                    {
                        judge_error=1;
                        l_border[3]=20;
                        break;
                    }
                }
                for(j=2;j<data_stastics_r-5;j++)//如果有右边线点大于于第1个右边线点，错误
                {
                    if(points_r[1][0]<points_r[j][0])
                    {
                        l_border[3]=30;
                        judge_error=1;
                        break;
                    }
                }
                if(func_abs(points_l[data_stastics_l-1][1]-points_r[data_stastics_r-1][1])>3)//两边界的最高点基本在同一y值
                {
                    l_border[3]=40;
                    judge_error=1;
                }

                t=(points_l[1][0]+points_r[1][0])/2;
                for(j=points_l[data_stastics_l-1][1];j>8;j--)//黑块上面有3白点
                {
                    if(bin_image[j][t]==255&&bin_image[j-1][t]==255&&bin_image[j-2][t]==255)
                    {
                        l_border[3]=50;
                        t=j-2;
                        break;
                    }

                }

               // tft180_show_int                 (55,60, 8, 2);

                    for(i=55;i<65;i++)
                    {
                        //tft180_draw_point(i,points_l[data_stastics_l-1][1]+10,uesr_RED);
                        //tft180_draw_point(i,points_l[data_stastics_l-1][1]-2,uesr_RED);


                        if(bin_image[points_l[data_stastics_l-1][1]+10][i]==0)
                        {
                            judge_error=1;
                            //tft180_show_int                 (55,80, 2, 1);
                            l_border[3]=60;
                             break;
                        }

                        if(bin_image[points_l[data_stastics_l-1][1]-2][i]==255)
                        {
                            judge_error=1;
                            //tft180_show_int                 (55,100, 3, 1);
                            l_border[3]=70;
                            break;
                        }

                        if(bin_image[t][i]==255)
                        {
                            l_border[3]=80;
                            judge1=1;
                        }


//                    for(i=55;i<65;i++)
//                    {
//                        tft180_draw_point(i,points_l[data_stastics_l-1][1]-15,uesr_RED);
//                        if(bin_image[points_l[data_stastics_l-1][1]-15][i]==0)
//                        {
//                            tft180_show_int                 (75,60, 3, 1);
//                            judge_error=1;
//                            break;
//                        }
//                    }


                    if(judge_error==0&&judge1==1)
                    {
                        gpio_high(P33_10);
                        judge_flage=1;
                    }

                }
            }
}


    break;
    case 1:
        gpio_low(P33_10);
        middle_point=(points_l[1][0]+points_r[1][0])/2;
        if(start_jump==1&&jump_flaged==0)
        {
            jump_flag=1;
            jump_flaged=1;
        }
        if(jump_flaged==2)
        {
            judge_flage=0;
            finish=1;
            hight1=0;
            hight2=0;
            middle_point=0;
            jump_flaged=0;
        }
    }
}



float steer_flag=0;//单边桥标志位
int disantance=0;
uint8 left_bridge_into_state=0;
int bridge_size=0;
extern float ab;
extern int speed;
float abcd=0;
void left_bridge_judge()
{
    int i=0,j=0,find_l=0,find_r=0,l_high_start=0,r_high_start=0,r_bridge_point=0,kk=0,l_bredge_error=0;
    static int high=0,shang_high_point=0,l_buff=0,r_buff=0;
    uint8 black1=0,white1=0,white2=0,zhengqie=0;

    if(left_bridge_into_state!=0)
        {
            disantance += motor_value.receive_left_speed_data-motor_value.receive_right_speed_data;
                     //tft180_show_int                 (100,80, disantance, 5);
                     if(disantance>50000)
                     {
                         high=0;
                         shang_high_point=0;
                         l_buff=0;
                         r_buff=0;
                         left_bridge_into_state=0;
                         disantance=0;
                         abcd=0;
                         speed=hou_speed;
                         finish=1;

                         steer_1.center_num =4300;//4900
                         steer_2.center_num=5200;//4600
                         steer_3.center_num=4900;//3300
                         steer_4.center_num=4200;

                     }
        }
    //tft_draw_circle(func_limit_ab(50, 10, 110), func_limit_ab(49,10,50), uesr_RED);
    //tft_draw_circle(func_limit_ab(50, 10, 110), func_limit_ab(50,10,50), uesr_RED);
    switch(left_bridge_into_state)
    {
    case 0:

        l_turn_judge_limit(55,5,10,8,40);
        r_turn_judge_limit(55,5,8,10,110);

        l_bredge_error=wan_dao();

        if(r_turn_max_t>0)
        {
            if(r_border[r_turn_point_t]<r_border[r_turn_point_t-5])
            {
                l_bredge_error=1;

            }
        }

        if(l_bredge_error==0&&l_turn_max_t>0)
        {
            if(l_border[l_turn_point_t]>10&&l_lost<5&&r_lost<5)
            {
                i=l_border[l_turn_point_t-6]-3;
                for(j=l_turn_point_t-6;j>=5;j--)
                {
                    if(bin_image[j][i]==255)
                    {
                        zhengqie=1;
                        break;
                    }
                }
                if(zhengqie==1)
                {
                    if(l_border[l_turn_point_t-5]-l_border[l_turn_point_t]>0)
                    {
                        for(i=l_border[l_turn_point_t-6];i>=l_border[l_turn_point_t-6]-20;i--)
                        {
                            if(bin_image[l_turn_point_t-6][i]==0)
                                black1++;
                        }
                        for(i=l_border[l_turn_point_t-6];i<=l_border[l_turn_point_t-6]+20;i++)
                        {
                            if(bin_image[l_turn_point_t-6][i]==255)
                                white1++;
                        }
                        if(black1>=10&&white1>=10)//如果第1块单边桥确定，判定第2块单边桥
                        {

                        kk=(l_border[l_turn_point_t-6]+r_border[l_turn_point_t-6])/2;
                                for(j=l_turn_point_t-6;j>4;j--)//func_limit_ab(l_turn_point-16, 5, 25)
                                {
                                    if(bin_image[j][kk]==0)
                                    {
                                        for(i=kk;i>l_border[j];i--)
                                        {
                                            if(bin_image[j][i]==255)
                                            {
                                                if(i>l_border[l_turn_point_t-6]+5)
                                                {
                                                    l_bredge_error=1;
                                                    break;
                                                }
                                                white2++;

                                                if(white2>3)
                                                {
                                                    //tft_draw_circle(func_limit_ab(l_border[l_turn_point_t-5], 10, 110), func_limit_ab(l_turn_point_t-5,10,50), uesr_RED);

                                                    steer_flag=1;
                                                    speed=280;//300
                                                    pwm_set_duty(ATOM1_CH6_P02_6,2800);//1
                                                    pwm_set_duty(ATOM1_CH5_P02_5,6700);//2
                                                    pwm_set_duty(ATOM1_CH4_P02_4,3400);//3500 shenggqo
                                                    pwm_set_duty(ATOM1_CH7_P02_7,5500);//5400 jianshao
                                                    //tft_draw_circle(func_limit_ab(kk, 10, 110), func_limit_ab(j,10,50), uesr_RED);
                                                    r_bridge_point=j;
                                                    gpio_high(P33_10);
                                                    left_bridge_into_state=1;

                                                    break;
                                                }
                                            }
                                        }

                                    }
                                    if(r_bridge_point>0)
                                        break;

                                }
                        }
                    }
                }
            }
        }

    break;
    case 1:
        gpio_low(P33_10);

        for(j=55;j>5;j--)
        {
              if(bin_image[j][50]-bin_image[j-1][50]==255)
              {
                    l_high_start=j;//单边桥的下点
                    for(i=50;i<100;i++)
                    {
                        if(bin_image[j-3][i+1]-bin_image[j-3][i]==255)//单边桥的左右边界点
                        {
                            for(;j>5;j--)
                            {
                                if(bin_image[j-1][50]-bin_image[j][50]==255)//单边桥的上点
                                {
                                    high=l_high_start-(j-1);//单边桥的高度
                                    l_buff=i;//单边桥的右边界点
                                    find_l=1;
                                    origin=75;
                                    break;
                                }
                            }
                        }
                        if(find_l==1)
                        break;
                    }
                    if(find_l==1)
                    break;
              }
        }
        if(l_buff>0)
        {
            for(j=l_high_start-3;j>=l_high_start-high+2;j--)
            {
                l_border[j]=(uint8)limit_a_b(l_border[j]-(r_border[j]-l_buff),3,117);

            }
        }
        if(l_border[30]>50||bin_image[50][45]==0)
        {
            origin=75;
            left_bridge_into_state=2;
        }
    break;
    case 2://左前右后

        if(shang_high_point==0)
        {
            l_turn_judge_limit(58,5,10,20,45);

            if(l_turn_point_t>0)
            {

                i=l_border[l_turn_point_t-5]-5;
                for(j=l_turn_point_t-8;j<58;j++)
                {
                     if(bin_image[j][i]-bin_image[j-1][i]==255)//单边桥的下点
                     {
                           l_high_start=j;
                           break;
                     }
                }

                j=l_turn_point_t-8;
                for(i=l_border[l_turn_point_t-5]-2;i<90;i++)//找左单边桥的右边界点
                {
                    if(bin_image[j][i]-bin_image[j][i-1]==255)
                    {
                        l_buff=i;//单边桥的右边界点

                        break;
                    }
                }

                i=l_border[l_turn_point_t-5]-5;
                if(l_high_start>0)
                {
                    for(j=l_turn_point_t-8;j>10;j--)
                    {
                        if(bin_image[j-1][i]-bin_image[j][i]==255)//单边桥的上点
                        {

                            high=l_high_start-(j-1);//单边桥的高度

                            find_l=1;
                            break;
                        }
                    }
                }

                if(find_l>0)
                {
                    for(j=l_high_start-2;j>=l_high_start-high+1;j--)
                    {
                        l_border[j]=(uint8)limit_a_b(l_border[j]-(r_border[j]-l_buff)-4,3,117);
                    }
                }
            }
        }

        if(l_border[55]>40)//如果左单边桥的下点在图像下
        {
            i=l_border[55];
            for(j=59;j>20;j--)
            {
                if(bin_image[j-1][i-10]-bin_image[j][i-10]==255)
                {
                    shang_high_point=j;//单边桥的上点

                    break;
                }
            }

            for(j=59;j>shang_high_point;j--)
            {
                l_border[j]=(uint8)limit_a_b(l_border[j]-(r_border[j]-i)-5,3,117);
            }
        }

        r_turn_judge_limit(58,5,10,15,80);

        if(r_turn_point_t>0)//找右单边桥的边界
        {

            i=r_border[r_turn_point_t-8]+5;
            for(j=r_turn_point_t-8;j<58;j++)
            {
                 if(bin_image[j][i]-bin_image[j-1][i]==255)//单边桥的下点
                 {
                       r_high_start=j;
                       break;
                 }
            }

            j=r_turn_point_t-8;
            for(i=r_border[r_turn_point_t-5]+2;i>30;i--)//找右单边桥的左边界点
            {
                if(bin_image[j][i-1]-bin_image[j][i]==255)
                {
                    r_buff=i-1;//右单边桥的左边界点

                    break;
                }
            }

            i=r_border[r_turn_point_t-8]+5;
            if(r_high_start>0)
            {
                for(j=r_turn_point_t-5;j>10;j--)
                {
                    if(bin_image[j-1][i]-bin_image[j][i]==255)//单边桥的上点
                    {
                        high=r_high_start-(j-1);//单边桥的高度
                        find_r=1;
                        break;
                    }
                }
            }
            if(find_r>0)
            {
                for(j=r_high_start-1;j>=r_high_start-high+1;j--)
                {
                    r_border[j]=(uint8)limit_a_b(r_border[j]+(r_buff-l_border[j])+3,3,117);
                }
            }
        }

        if(shang_high_point>50&&points_l[0][0]<40&&bin_image[45][80]==0)
        {
            l_turn_judge_limit(58,5,10,10,45);

            if(l_turn_point_t>0&&l_border[l_turn_point_t-5]>l_border[l_turn_point_t])//如果还有左单边桥
            {
                origin=45;
                shang_high_point=0;
                left_bridge_into_state=3;

            }
            else
            {
                origin=45;
                shang_high_point=0;
                left_bridge_into_state=4;
            }
        }

    break;
    case 3://右前左后

        r_border[3]=90;
        if(shang_high_point==0)
        {
            r_turn_judge_limit(58,5,10,20,80);


            if(r_turn_point_t>0)//找右单边桥的边界
            {

                i=r_border[r_turn_point_t-8]+5;
                for(j=r_turn_point_t-8;j<58;j++)
                {
                     if(bin_image[j][i]-bin_image[j-1][i]==255)//单边桥的下点
                     {
                           r_high_start=j;
                           break;
                     }
                }

                j=r_turn_point_t-8;
                for(i=r_border[r_turn_point_t-5]+2;i>30;i--)//找右单边桥的左边界点
                {
                    if(bin_image[j][i-1]-bin_image[j][i]==255)
                    {
                        r_buff=i-1;//右单边桥的左边界点
                        //tft_draw_circle(l_buff, j, uesr_RED);
                        break;
                    }
                }

                i=r_border[r_turn_point_t-8]+5;
                if(r_high_start>0)
                {
                    for(j=r_turn_point_t-5;j>10;j--)
                    {
                        if(bin_image[j-1][i]-bin_image[j][i]==255)//单边桥的上点
                        {
                            high=r_high_start-(j-1);//单边桥的高度
                            find_r=1;
                            break;
                        }
                    }
                }
                if(find_r>0)
                {
                    for(j=r_high_start-1;j>=r_high_start-high+1;j--)
                    {
                        r_border[j]=(uint8)limit_a_b(r_border[j]+(r_buff-l_border[j])+3,3,117);
                    }
                }
            }
        }

        if(r_border[55]<80)//如果右单边桥的下点在图像下
        {
            i=r_border[55];
            for(j=59;j>20;j--)
            {
                if(bin_image[j-1][i+10]-bin_image[j][i+10]==255)
                {
                    shang_high_point=j;//单边桥的上点

                    break;
                }
            }

            for(j=59;j>shang_high_point;j--)
            {
                r_border[j]=(uint8)limit_a_b(r_border[j]+(i-l_border[j])+2,3,117);
            }
        }

        l_turn_judge_limit(58,5,10,20,45);
        if(l_turn_point_t>0)
        {
            i=l_border[l_turn_point_t-5]-5;
            for(j=l_turn_point_t-8;j<58;j++)
            {
                 if(bin_image[j][i]-bin_image[j-1][i]==255)//单边桥的下点
                 {//tft180_show_int                 (45,80,2,1);
                       l_high_start=j;
                       break;
                 }
            }

            j=l_turn_point_t-8;
            for(i=l_border[l_turn_point_t-5]-2;i<90;i++)//找左单边桥的右边界点
            {
                if(bin_image[j][i]-bin_image[j][i-1]==255)
                {
                    l_buff=i;//单边桥的右边界点

                    break;
                }
            }

            i=l_border[l_turn_point_t-5]-5;
            if(l_high_start>0)
            {
                for(j=l_turn_point_t-8;j>10;j--)
                {
                    if(bin_image[j-1][i]-bin_image[j][i]==255)//单边桥的上点
                    {

                        high=l_high_start-(j-1);//单边桥的高度

                        find_l=1;
                        break;
                    }
                }
            }

            if(find_l>0)
            {
                for(j=l_high_start-2;j>=l_high_start-high+1;j--)
                {
                    l_border[j]=(uint8)limit_a_b(l_border[j]-(r_border[j]-l_buff)-6,3,117);
                }
            }
        }

        if(shang_high_point>50&&points_r[0][0]>80&&bin_image[45][40]==0)
        {
            r_turn_judge_limit(58,5,10,10,80);

            if(r_turn_point_t>0&&r_border[r_turn_point_t-5]<r_border[r_turn_point_t])//如果还有右单边桥
            {
                origin=75;
                shang_high_point=0;
                left_bridge_into_state=2;

            }
            else
            {
                origin=75;
                shang_high_point=0;
                left_bridge_into_state=5;
            }
        }
    break;
    case 4:

        if(shang_high_point==0)//最后1个右单边桥
        {
            r_turn_judge_limit(58,5,10,20,80);
//

            if(r_turn_point_t>0)//找右单边桥的边界
            {

                i=r_border[r_turn_point_t-8]+5;
                for(j=r_turn_point_t-8;j<58;j++)
                {
                     if(bin_image[j][i]-bin_image[j-1][i]==255)//单边桥的下点
                     {
                           r_high_start=j;
                           break;
                     }
                }

                j=r_turn_point_t-8;
                for(i=r_border[r_turn_point_t-5]+2;i>30;i--)//找右单边桥的左边界点
                {
                    if(bin_image[j][i-1]-bin_image[j][i]==255)
                    {
                        r_buff=i-1;//右单边桥的左边界点

                        break;
                    }
                }

                i=r_border[r_turn_point_t-8]+5;
                if(r_high_start>0)
                {
                    for(j=r_turn_point_t-5;j>10;j--)
                    {
                        if(bin_image[j-1][i]-bin_image[j][i]==255)//单边桥的上点
                        {
                            high=r_high_start-(j-1);//单边桥的高度
                            find_r=1;
                            break;
                        }
                    }
                }
                if(find_r>0)
                {
                    for(j=r_high_start-1;j>=r_high_start-high+1;j--)
                    {
                        r_border[j]=(uint8)limit_a_b(r_border[j]+(r_buff-l_border[j])+3,3,117);
                    }
                }
            }
        }

        if(r_border[55]<80)//如果右单边桥的下点在图像下
        {
            i=r_border[55];
            for(j=59;j>20;j--)
            {
                if(bin_image[j-1][i+10]-bin_image[j][i+10]==255)
                {
                    shang_high_point=j;//单边桥的上点

                    break;
                }
            }

            for(j=59;j>shang_high_point;j--)
            {
                r_border[j]=(uint8)limit_a_b(r_border[j]+(i-l_border[j])+2,3,117);
            }
        }
        if(shang_high_point>50&&points_r[0][0]>80)
        {
            origin=60;
            shang_high_point=0;
            left_bridge_into_state=6;
        }
     break;
     case 5:
         if(shang_high_point==0)//最后1个左单边桥
         {
             l_turn_judge_limit(58,5,10,20,45);
//
             if(l_turn_point_t>0)
             {

                 i=l_border[l_turn_point_t-5]-5;
                 for(j=l_turn_point_t-8;j<58;j++)
                 {
                      if(bin_image[j][i]-bin_image[j-1][i]==255)//单边桥的下点
                      {
                            l_high_start=j;
                            break;
                      }
                 }

                 j=l_turn_point_t-8;
                 for(i=l_border[l_turn_point_t-5]-2;i<90;i++)//找左单边桥的右边界点
                 {
                     if(bin_image[j][i]-bin_image[j][i-1]==255)
                     {
                         l_buff=i;//单边桥的右边界点

                         break;
                     }
                 }

                 i=l_border[l_turn_point_t-5]-5;
                 if(l_high_start>0)
                 {
                     for(j=l_turn_point_t-8;j>10;j--)
                     {
                         if(bin_image[j-1][i]-bin_image[j][i]==255)//单边桥的上点
                         {

                             high=l_high_start-(j-1);//单边桥的高度

                             find_l=1;
                             break;
                         }
                     }
                 }

                 if(find_l>0)
                 {
                     for(j=l_high_start-2;j>=l_high_start-high+1;j--)
                     {
                         l_border[j]=(uint8)limit_a_b(l_border[j]-(r_border[j]-l_buff)-6,3,117);
                     }
                 }
             }
         }

         if(l_border[55]>40)//如果左单边桥的下点在图像下
         {
             i=l_border[55];
             for(j=59;j>20;j--)
             {
                 if(bin_image[j-1][i-10]-bin_image[j][i-10]==255)
                 {
                     shang_high_point=j;//单边桥的上点
                     //Target_Speed=80;
                     break;
                 }
             }

             for(j=59;j>shang_high_point;j--)
             {
                 l_border[j]=(uint8)limit_a_b(l_border[j]-(r_border[j]-i)-11,3,117);
             }
         }
         if(shang_high_point>50&&points_l[0][0]<40)
         {
             origin=60;
             shang_high_point=0;
             left_bridge_into_state=6;
         }

     break;
     case 6:
         disantance += motor_value.receive_left_speed_data-motor_value.receive_right_speed_data;

         if(disantance>5000)
         {
             high=0;
             shang_high_point=0;
             l_buff=0;
             r_buff=0;
             left_bridge_into_state=0;
             disantance=0;
             abcd=0;
             speed=hou_speed;
             finish=1;
eeee=1;
             steer_1.center_num =4300;//4900
             steer_2.center_num=5200;//4600
             steer_3.center_num=4900;//3300
             steer_4.center_num=4200;

         }
    }
}



void left_bridge_judge3()//3个单边桥
{
    int i=0,j=0,find_l=0,find_r=0,l_high_start=0,r_high_start=0,r_bridge_point=0,kk=0,l_bredge_error=0;
    static int high=0,shang_high_point=0,l_buff=0,r_buff=0;
    uint8 black1=0,white1=0,white2=0,zhengqie=0;


    if(left_bridge_into_state!=0)
    {
        disantance += motor_value.receive_left_speed_data-motor_value.receive_right_speed_data;
                 //tft180_show_int                 (100,80, disantance, 5);
                 if(disantance>50000)
                 {
                     high=0;
                     shang_high_point=0;
                     l_buff=0;
                     r_buff=0;
                     left_bridge_into_state=0;
                     disantance=0;
                     abcd=0;
                     speed=hou_speed;
                     finish=1;

                     steer_1.center_num =4300;//4900
                     steer_2.center_num=5200;//4600
                     steer_3.center_num=4900;//3300
                     steer_4.center_num=4200;

                 }
    }

    switch(left_bridge_into_state)
    {
    case 0:
        l_turn_judge_limit(55,5,10,8,40);
        r_turn_judge_limit(55,5,8,10,110);
        l_bredge_error=wan_dao();
        if(r_turn_max_t>0)
        {
            if(r_border[r_turn_point_t]<r_border[r_turn_point_t-5])
            {
                l_bredge_error=1;
            }
        }

        if(l_bredge_error==0&&l_turn_max_t>0)
        {
            if(l_border[l_turn_point_t]>10&&l_lost<5&&r_lost<5)
            {
                i=l_border[l_turn_point_t-6]-3;
                for(j=l_turn_point_t-6;j>=5;j--)
                {
                    if(bin_image[j][i]==255)
                    {
                        zhengqie=1;
                        break;
                    }
                }
                if(zhengqie==1)
                {
                    if(l_border[l_turn_point_t-5]-l_border[l_turn_point_t]>0)
                    {
                        for(i=l_border[l_turn_point_t-6];i>=l_border[l_turn_point_t-6]-20;i--)
                        {
                            if(bin_image[l_turn_point_t-6][i]==0)
                                black1++;
                        }
                        for(i=l_border[l_turn_point_t-6];i<=l_border[l_turn_point_t-6]+20;i++)
                        {
                            if(bin_image[l_turn_point_t-6][i]==255)
                                white1++;
                        }
                        if(black1>=10&&white1>=10)//如果第1块单边桥确定，判定第2块单边桥
                        {

                        kk=(l_border[l_turn_point_t-6]+r_border[l_turn_point_t-6])/2;
                                for(j=l_turn_point_t-6;j>4;j--)
                                {
                                    if(bin_image[j][kk]==0)
                                    {
                                        for(i=kk;i>l_border[j];i--)
                                        {
                                            if(bin_image[j][i]==255)
                                            {
                                                if(i>l_border[l_turn_point_t-6]+5)
                                                {
                                                    l_bredge_error=1;
                                                    break;
                                                }
                                                white2++;

                                                if(white2>3)
                                                {

                                                    steer_flag=1;
                                                    speed=280;//300
                                                    pwm_set_duty(ATOM1_CH6_P02_6,2800);//1
                                                    pwm_set_duty(ATOM1_CH5_P02_5,6700);//2
                                                    pwm_set_duty(ATOM1_CH4_P02_4,3400);//3500 shenggqo
                                                    pwm_set_duty(ATOM1_CH7_P02_7,5500);//5400 jianshao
                                                    //tft_draw_circle(func_limit_ab(kk, 10, 110), func_limit_ab(j,10,50), uesr_RED);
                                                    r_bridge_point=j;
                                                    gpio_high(P33_10);
                                                    left_bridge_into_state=1;

                                                    break;
                                                }
                                            }
                                        }

                                    }
                                    if(r_bridge_point>0)
                                        break;

                                }
                        }
                    }
                }
            }
        }

    break;
    case 1:
        gpio_low(P33_10);

        for(j=55;j>5;j--)
        {
              if(bin_image[j][50]-bin_image[j-1][50]==255)
              {
                    l_high_start=j;//单边桥的下点
                    for(i=50;i<100;i++)
                    {
                        if(bin_image[j-3][i+1]-bin_image[j-3][i]==255)//单边桥的左右边界点
                        {
                            for(;j>5;j--)
                            {
                                if(bin_image[j-1][50]-bin_image[j][50]==255)//单边桥的上点
                                {
                                    high=l_high_start-(j-1);//单边桥的高度
                                    l_buff=i;//单边桥的右边界点
                                    find_l=1;
                                    origin=75;
                                    break;
                                }
                            }
                        }
                        if(find_l==1)
                        break;
                    }
                    if(find_l==1)
                    break;
              }
        }
        if(l_buff>0)
        {
            for(j=l_high_start-3;j>=l_high_start-high+2;j--)
            {
                l_border[j]=(uint8)limit_a_b(l_border[j]-(r_border[j]-l_buff),3,117);

            }
        }
        if(l_border[30]>50||bin_image[50][45]==0)
        {
            origin=75;
            left_bridge_into_state=2;
        }
    break;
    case 2://左前右后

        if(shang_high_point==0)
                {
                    l_turn_judge_limit(58,5,10,20,45);

                    if(l_turn_point_t>0)
                    {

                        i=l_border[l_turn_point_t-5]-5;
                        for(j=l_turn_point_t-8;j<58;j++)
                        {
                             if(bin_image[j][i]-bin_image[j-1][i]==255)//单边桥的下点
                             {
                                   l_high_start=j;
                                   break;
                             }
                        }

                        j=l_turn_point_t-8;
                        for(i=l_border[l_turn_point_t-5]-2;i<90;i++)//找左单边桥的右边界点
                        {
                            if(bin_image[j][i]-bin_image[j][i-1]==255)
                            {
                                l_buff=i;//单边桥的右边界点

                                break;
                            }
                        }

                        i=l_border[l_turn_point_t-5]-5;
                        if(l_high_start>0)
                        {
                            for(j=l_turn_point_t-8;j>10;j--)
                            {
                                if(bin_image[j-1][i]-bin_image[j][i]==255)//单边桥的上点
                                {

                                    high=l_high_start-(j-1);//单边桥的高度

                                    find_l=1;
                                    break;
                                }
                            }
                        }

                        if(find_l>0)
                        {
                            for(j=l_high_start-2;j>=l_high_start-high+1;j--)
                            {
                                l_border[j]=(uint8)limit_a_b(l_border[j]-(r_border[j]-l_buff)-4,3,117);
                            }
                        }
                    }
                }

                if(l_border[55]>40)//如果左单边桥的下点在图像下
                {
                    i=l_border[55];
                    for(j=59;j>20;j--)
                    {
                        if(bin_image[j-1][i-10]-bin_image[j][i-10]==255)
                        {
                            shang_high_point=j;//单边桥的上点

                            break;
                        }
                    }

                    for(j=59;j>shang_high_point;j--)
                    {
                        l_border[j]=(uint8)limit_a_b(l_border[j]-(r_border[j]-i)-5,3,117);
                    }
                }

                r_turn_judge_limit(58,5,10,15,80);

                if(r_turn_point_t>0)//找右单边桥的边界
                {

                    i=r_border[r_turn_point_t-8]+5;
                    for(j=r_turn_point_t-8;j<58;j++)
                    {
                         if(bin_image[j][i]-bin_image[j-1][i]==255)//单边桥的下点
                         {
                               r_high_start=j;
                               break;
                         }
                    }

                    j=r_turn_point_t-8;
                    for(i=r_border[r_turn_point_t-5]+2;i>30;i--)//找右单边桥的左边界点
                    {
                        if(bin_image[j][i-1]-bin_image[j][i]==255)
                        {
                            r_buff=i-1;//右单边桥的左边界点

                            break;
                        }
                    }

                    i=r_border[r_turn_point_t-8]+5;
                    if(r_high_start>0)
                    {
                        for(j=r_turn_point_t-5;j>10;j--)
                        {
                            if(bin_image[j-1][i]-bin_image[j][i]==255)//单边桥的上点
                            {
                                high=r_high_start-(j-1);//单边桥的高度
                                find_r=1;
                                break;
                            }
                        }
                    }
                    if(find_r>0)
                    {
                        for(j=r_high_start-1;j>=r_high_start-high+1;j--)
                        {
                            r_border[j]=(uint8)limit_a_b(r_border[j]+(r_buff-l_border[j])+3,3,117);
                        }
                    }
                }

                if(shang_high_point>50&&points_l[0][0]<40&&bin_image[45][80]==0)
                {
                    l_turn_judge_limit(58,5,10,10,45);

                    if(l_turn_point_t>0&&l_border[l_turn_point_t-5]>l_border[l_turn_point_t])//如果还有左单边桥
                    {
                        origin=45;
                        shang_high_point=0;
                        left_bridge_into_state=3;

                    }

                }

    break;
    case 3://右前左后


        if(shang_high_point==0)
        {
            r_turn_judge_limit(58,5,10,20,80);


            if(r_turn_point_t>0)//找右单边桥的边界
            {

                i=r_border[r_turn_point_t-8]+5;
                for(j=r_turn_point_t-8;j<58;j++)
                {
                     if(bin_image[j][i]-bin_image[j-1][i]==255)//单边桥的下点
                     {
                           r_high_start=j;
                           break;
                     }
                }

                j=r_turn_point_t-8;
                for(i=r_border[r_turn_point_t-5]+2;i>30;i--)//找右单边桥的左边界点
                {
                    if(bin_image[j][i-1]-bin_image[j][i]==255)
                    {
                        r_buff=i-1;//右单边桥的左边界点
                        //tft_draw_circle(l_buff, j, uesr_RED);
                        break;
                    }
                }

                i=r_border[r_turn_point_t-8]+5;
                if(r_high_start>0)
                {
                    for(j=r_turn_point_t-5;j>10;j--)
                    {
                        if(bin_image[j-1][i]-bin_image[j][i]==255)//单边桥的上点
                        {
                            high=r_high_start-(j-1);//单边桥的高度
                            find_r=1;
                            break;
                        }
                    }
                }
                if(find_r>0)
                {
                    for(j=r_high_start-1;j>=r_high_start-high+1;j--)
                    {
                        r_border[j]=(uint8)limit_a_b(r_border[j]+(r_buff-l_border[j])+3,3,117);
                    }
                }
            }
        }

        if(r_border[55]<80)//如果右单边桥的下点在图像下
        {
            i=r_border[55];
            for(j=59;j>20;j--)
            {
                if(bin_image[j-1][i+10]-bin_image[j][i+10]==255)
                {
                    shang_high_point=j;//单边桥的上点
                    //Target_Speed=80;
                    break;
                }
            }

            for(j=59;j>shang_high_point;j--)
            {
                r_border[j]=(uint8)limit_a_b(r_border[j]+(i-l_border[j])+2,3,117);
            }
        }

        l_turn_judge_limit(58,5,10,20,45);
        if(l_turn_point_t>0)
        {
            i=l_border[l_turn_point_t-5]-5;
            for(j=l_turn_point_t-8;j<58;j++)
            {
                 if(bin_image[j][i]-bin_image[j-1][i]==255)//单边桥的下点
                 {//tft180_show_int                 (45,80,2,1);
                       l_high_start=j;
                       break;
                 }
            }

            j=l_turn_point_t-8;
            for(i=l_border[l_turn_point_t-5]-2;i<90;i++)//找左单边桥的右边界点
            {
                if(bin_image[j][i]-bin_image[j][i-1]==255)
                {
                    l_buff=i;//单边桥的右边界点
                    //tft_draw_circle(l_buff, j, uesr_RED);
                    break;
                }
            }

            i=l_border[l_turn_point_t-5]-5;
            if(l_high_start>0)
            {
                for(j=l_turn_point_t-8;j>10;j--)
                {
                    if(bin_image[j-1][i]-bin_image[j][i]==255)//单边桥的上点
                    {//tft180_show_int                 (65,80,j-1,2);

                        high=l_high_start-(j-1);//单边桥的高度
                        //tft_draw_circle(i, j-1, uesr_RED);
                        find_l=1;
                        break;
                    }
                }
            }

            if(find_l>0)
            {
                for(j=l_high_start-2;j>=l_high_start-high+1;j--)
                {
                    l_border[j]=(uint8)limit_a_b(l_border[j]-(r_border[j]-l_buff)-6,3,117);
                }
            }
        }

        if(shang_high_point>50&&points_r[0][0]>80&&bin_image[45][40]==0)
        {


                origin=75;
                shang_high_point=0;
                left_bridge_into_state=5;

        }

     break;
     case 5:
         if(shang_high_point==0)//最后1个左单边桥
         {
             l_turn_judge_limit(58,5,10,20,45);
             if(l_turn_point_t>0)
             {

                 i=l_border[l_turn_point_t-5]-5;
                 for(j=l_turn_point_t-8;j<58;j++)
                 {
                      if(bin_image[j][i]-bin_image[j-1][i]==255)//单边桥的下点
                      {
                            l_high_start=j;
                            break;
                      }
                 }

                 j=l_turn_point_t-8;
                 for(i=l_border[l_turn_point_t-5]-2;i<90;i++)//找左单边桥的右边界点
                 {
                     if(bin_image[j][i]-bin_image[j][i-1]==255)
                     {
                         l_buff=i;//单边桥的右边界点

                         break;
                     }
                 }

                 i=l_border[l_turn_point_t-5]-5;
                 if(l_high_start>0)
                 {
                     for(j=l_turn_point_t-8;j>10;j--)
                     {
                         if(bin_image[j-1][i]-bin_image[j][i]==255)//单边桥的上点
                         {

                             high=l_high_start-(j-1);//单边桥的高度

                             find_l=1;
                             break;
                         }
                     }
                 }

                 if(find_l>0)
                 {
                     for(j=l_high_start-2;j>=l_high_start-high+1;j--)
                     {
                         l_border[j]=(uint8)limit_a_b(l_border[j]-(r_border[j]-l_buff)-6,3,117);
                     }
                 }
             }
         }

         if(l_border[55]>40)//如果左单边桥的下点在图像下
         {
             i=l_border[55];
             for(j=59;j>20;j--)
             {
                 if(bin_image[j-1][i-10]-bin_image[j][i-10]==255)
                 {
                     shang_high_point=j;//单边桥的上点

                     break;
                 }
             }

             for(j=59;j>shang_high_point;j--)
             {
                 l_border[j]=(uint8)limit_a_b(l_border[j]-(r_border[j]-i)-11,3,117);
             }
         }
         if(shang_high_point>50&&points_l[0][0]<40)
         {
             origin=60;
             shang_high_point=0;
             left_bridge_into_state=6;
         }

     break;
     case 6:
         disantance += motor_value.receive_left_speed_data-motor_value.receive_right_speed_data;
         tft180_show_int                 (100,80, disantance, 5);
         if(disantance>5000)
         {
             high=0;
             shang_high_point=0;
             l_buff=0;
             r_buff=0;
             left_bridge_into_state=0;
             disantance=0;
             abcd=0;
             speed=hou_speed;
             finish=1;
eeee=1;
bbb=1;
             steer_1.center_num =4300;//4900
             steer_2.center_num=5200;//4600
             steer_3.center_num=4900;//3300
             steer_4.center_num=4200;

         }
    }
}


void left_bridge_judge3_di()//3个单边桥
{
    int i=0,j=0,find_l=0,find_r=0,l_high_start=0,r_high_start=0,r_bridge_point=0,kk=0,l_bredge_error=0;
    static int high=0,shang_high_point=0,l_buff=0,r_buff=0;
    uint8 black1=0,white1=0,white2=0,zhengqie=0;


    if(left_bridge_into_state!=0)
    {
        disantance += motor_value.receive_left_speed_data-motor_value.receive_right_speed_data;
                 //tft180_show_int                 (100,80, disantance, 5);
                 if(disantance>50000)
                 {
                     high=0;
                     shang_high_point=0;
                     l_buff=0;
                     r_buff=0;
                     left_bridge_into_state=0;
                     disantance=0;
                     abcd=0;
                     speed=hou_speed;
                     finish=1;



                 }
    }

    switch(left_bridge_into_state)
    {
    case 0:
        l_turn_judge_limit(55,5,10,8,40);
        r_turn_judge_limit(55,5,8,10,110);
        l_bredge_error=wan_dao();
        if(r_turn_max_t>0)
        {
            if(r_border[r_turn_point_t]<r_border[r_turn_point_t-5])
            {
                l_bredge_error=1;
            }
        }

        if(l_bredge_error==0&&l_turn_max_t>0)
        {
            if(l_border[l_turn_point_t]>10&&l_lost<5&&r_lost<5)
            {
                i=l_border[l_turn_point_t-6]-3;
                for(j=l_turn_point_t-6;j>=5;j--)
                {
                    if(bin_image[j][i]==255)
                    {
                        zhengqie=1;
                        break;
                    }
                }
                if(zhengqie==1)
                {
                    if(l_border[l_turn_point_t-5]-l_border[l_turn_point_t]>0)
                    {
                        for(i=l_border[l_turn_point_t-6];i>=l_border[l_turn_point_t-6]-20;i--)
                        {
                            if(bin_image[l_turn_point_t-6][i]==0)
                                black1++;
                        }
                        for(i=l_border[l_turn_point_t-6];i<=l_border[l_turn_point_t-6]+20;i++)
                        {
                            if(bin_image[l_turn_point_t-6][i]==255)
                                white1++;
                        }
                        if(black1>=10&&white1>=10)//如果第1块单边桥确定，判定第2块单边桥
                        {

                        kk=(l_border[l_turn_point_t-6]+r_border[l_turn_point_t-6])/2;
                                for(j=l_turn_point_t-6;j>4;j--)
                                {
                                    if(bin_image[j][kk]==0)
                                    {
                                        for(i=kk;i>l_border[j];i--)
                                        {
                                            if(bin_image[j][i]==255)
                                            {
                                                if(i>l_border[l_turn_point_t-6]+5)
                                                {
                                                    l_bredge_error=1;
                                                    break;
                                                }
                                                white2++;

                                                if(white2>3)
                                                {


                                                    //tft_draw_circle(func_limit_ab(kk, 10, 110), func_limit_ab(j,10,50), uesr_RED);
                                                    r_bridge_point=j;
                                                    gpio_high(P33_10);
                                                    left_bridge_into_state=1;

                                                    break;
                                                }
                                            }
                                        }

                                    }
                                    if(r_bridge_point>0)
                                        break;

                                }
                        }
                    }
                }
            }
        }

    break;
    case 1:
        gpio_low(P33_10);

        for(j=55;j>5;j--)
        {
              if(bin_image[j][50]-bin_image[j-1][50]==255)
              {
                    l_high_start=j;//单边桥的下点
                    for(i=50;i<100;i++)
                    {
                        if(bin_image[j-3][i+1]-bin_image[j-3][i]==255)//单边桥的左右边界点
                        {
                            for(;j>5;j--)
                            {
                                if(bin_image[j-1][50]-bin_image[j][50]==255)//单边桥的上点
                                {
                                    high=l_high_start-(j-1);//单边桥的高度
                                    l_buff=i;//单边桥的右边界点
                                    find_l=1;
                                    origin=75;
                                    break;
                                }
                            }
                        }
                        if(find_l==1)
                        break;
                    }
                    if(find_l==1)
                    break;
              }
        }
        if(l_buff>0)
        {
            for(j=l_high_start-3;j>=l_high_start-high+2;j--)
            {
                l_border[j]=(uint8)limit_a_b(l_border[j]-(r_border[j]-l_buff),3,117);

            }
        }
        if(l_border[30]>50||bin_image[50][45]==0)
        {
            origin=75;
            left_bridge_into_state=2;
        }
    break;
    case 2://左前右后

        if(shang_high_point==0)
                {
                    l_turn_judge_limit(58,5,10,20,45);

                    if(l_turn_point_t>0)
                    {

                        i=l_border[l_turn_point_t-5]-5;
                        for(j=l_turn_point_t-8;j<58;j++)
                        {
                             if(bin_image[j][i]-bin_image[j-1][i]==255)//单边桥的下点
                             {
                                   l_high_start=j;
                                   break;
                             }
                        }

                        j=l_turn_point_t-8;
                        for(i=l_border[l_turn_point_t-5]-2;i<90;i++)//找左单边桥的右边界点
                        {
                            if(bin_image[j][i]-bin_image[j][i-1]==255)
                            {
                                l_buff=i;//单边桥的右边界点

                                break;
                            }
                        }

                        i=l_border[l_turn_point_t-5]-5;
                        if(l_high_start>0)
                        {
                            for(j=l_turn_point_t-8;j>10;j--)
                            {
                                if(bin_image[j-1][i]-bin_image[j][i]==255)//单边桥的上点
                                {

                                    high=l_high_start-(j-1);//单边桥的高度

                                    find_l=1;
                                    break;
                                }
                            }
                        }

                        if(find_l>0)
                        {
                            for(j=l_high_start-2;j>=l_high_start-high+1;j--)
                            {
                                l_border[j]=(uint8)limit_a_b(l_border[j]-(r_border[j]-l_buff)-4,3,117);
                            }
                        }
                    }
                }

                if(l_border[55]>40)//如果左单边桥的下点在图像下
                {
                    i=l_border[55];
                    for(j=59;j>20;j--)
                    {
                        if(bin_image[j-1][i-10]-bin_image[j][i-10]==255)
                        {
                            shang_high_point=j;//单边桥的上点

                            break;
                        }
                    }

                    for(j=59;j>shang_high_point;j--)
                    {
                        l_border[j]=(uint8)limit_a_b(l_border[j]-(r_border[j]-i)-5,3,117);
                    }
                }

                r_turn_judge_limit(58,5,10,15,80);

                if(r_turn_point_t>0)//找右单边桥的边界
                {

                    i=r_border[r_turn_point_t-8]+5;
                    for(j=r_turn_point_t-8;j<58;j++)
                    {
                         if(bin_image[j][i]-bin_image[j-1][i]==255)//单边桥的下点
                         {
                               r_high_start=j;
                               break;
                         }
                    }

                    j=r_turn_point_t-8;
                    for(i=r_border[r_turn_point_t-5]+2;i>30;i--)//找右单边桥的左边界点
                    {
                        if(bin_image[j][i-1]-bin_image[j][i]==255)
                        {
                            r_buff=i-1;//右单边桥的左边界点

                            break;
                        }
                    }

                    i=r_border[r_turn_point_t-8]+5;
                    if(r_high_start>0)
                    {
                        for(j=r_turn_point_t-5;j>10;j--)
                        {
                            if(bin_image[j-1][i]-bin_image[j][i]==255)//单边桥的上点
                            {
                                high=r_high_start-(j-1);//单边桥的高度
                                find_r=1;
                                break;
                            }
                        }
                    }
                    if(find_r>0)
                    {
                        for(j=r_high_start-1;j>=r_high_start-high+1;j--)
                        {
                            r_border[j]=(uint8)limit_a_b(r_border[j]+(r_buff-l_border[j])+3,3,117);
                        }
                    }
                }

                if(shang_high_point>50&&points_l[0][0]<40&&bin_image[45][80]==0)
                {
                    l_turn_judge_limit(58,5,10,10,45);

                    if(l_turn_point_t>0&&l_border[l_turn_point_t-5]>l_border[l_turn_point_t])//如果还有左单边桥
                    {
                        origin=45;
                        shang_high_point=0;
                        left_bridge_into_state=3;

                    }

                }

    break;
    case 3://右前左后


        if(shang_high_point==0)
        {
            r_turn_judge_limit(58,5,10,20,80);


            if(r_turn_point_t>0)//找右单边桥的边界
            {

                i=r_border[r_turn_point_t-8]+5;
                for(j=r_turn_point_t-8;j<58;j++)
                {
                     if(bin_image[j][i]-bin_image[j-1][i]==255)//单边桥的下点
                     {
                           r_high_start=j;
                           break;
                     }
                }

                j=r_turn_point_t-8;
                for(i=r_border[r_turn_point_t-5]+2;i>30;i--)//找右单边桥的左边界点
                {
                    if(bin_image[j][i-1]-bin_image[j][i]==255)
                    {
                        r_buff=i-1;//右单边桥的左边界点
                        //tft_draw_circle(l_buff, j, uesr_RED);
                        break;
                    }
                }

                i=r_border[r_turn_point_t-8]+5;
                if(r_high_start>0)
                {
                    for(j=r_turn_point_t-5;j>10;j--)
                    {
                        if(bin_image[j-1][i]-bin_image[j][i]==255)//单边桥的上点
                        {
                            high=r_high_start-(j-1);//单边桥的高度
                            find_r=1;
                            break;
                        }
                    }
                }
                if(find_r>0)
                {
                    for(j=r_high_start-1;j>=r_high_start-high+1;j--)
                    {
                        r_border[j]=(uint8)limit_a_b(r_border[j]+(r_buff-l_border[j])+3,3,117);
                    }
                }
            }
        }

        if(r_border[55]<80)//如果右单边桥的下点在图像下
        {
            i=r_border[55];
            for(j=59;j>20;j--)
            {
                if(bin_image[j-1][i+10]-bin_image[j][i+10]==255)
                {
                    shang_high_point=j;//单边桥的上点
                    //Target_Speed=80;
                    break;
                }
            }

            for(j=59;j>shang_high_point;j--)
            {
                r_border[j]=(uint8)limit_a_b(r_border[j]+(i-l_border[j])+2,3,117);
            }
        }

        l_turn_judge_limit(58,5,10,20,45);
        if(l_turn_point_t>0)
        {
            i=l_border[l_turn_point_t-5]-5;
            for(j=l_turn_point_t-8;j<58;j++)
            {
                 if(bin_image[j][i]-bin_image[j-1][i]==255)//单边桥的下点
                 {//tft180_show_int                 (45,80,2,1);
                       l_high_start=j;
                       break;
                 }
            }

            j=l_turn_point_t-8;
            for(i=l_border[l_turn_point_t-5]-2;i<90;i++)//找左单边桥的右边界点
            {
                if(bin_image[j][i]-bin_image[j][i-1]==255)
                {
                    l_buff=i;//单边桥的右边界点
                    //tft_draw_circle(l_buff, j, uesr_RED);
                    break;
                }
            }

            i=l_border[l_turn_point_t-5]-5;
            if(l_high_start>0)
            {
                for(j=l_turn_point_t-8;j>10;j--)
                {
                    if(bin_image[j-1][i]-bin_image[j][i]==255)//单边桥的上点
                    {//tft180_show_int                 (65,80,j-1,2);

                        high=l_high_start-(j-1);//单边桥的高度
                        //tft_draw_circle(i, j-1, uesr_RED);
                        find_l=1;
                        break;
                    }
                }
            }

            if(find_l>0)
            {
                for(j=l_high_start-2;j>=l_high_start-high+1;j--)
                {
                    l_border[j]=(uint8)limit_a_b(l_border[j]-(r_border[j]-l_buff)-6,3,117);
                }
            }
        }

        if(shang_high_point>50&&points_r[0][0]>80&&bin_image[45][40]==0)
        {


                origin=75;
                shang_high_point=0;
                left_bridge_into_state=5;

        }

     break;
     case 5:
         if(shang_high_point==0)//最后1个左单边桥
         {
             l_turn_judge_limit(58,5,10,20,45);
             if(l_turn_point_t>0)
             {

                 i=l_border[l_turn_point_t-5]-5;
                 for(j=l_turn_point_t-8;j<58;j++)
                 {
                      if(bin_image[j][i]-bin_image[j-1][i]==255)//单边桥的下点
                      {
                            l_high_start=j;
                            break;
                      }
                 }

                 j=l_turn_point_t-8;
                 for(i=l_border[l_turn_point_t-5]-2;i<90;i++)//找左单边桥的右边界点
                 {
                     if(bin_image[j][i]-bin_image[j][i-1]==255)
                     {
                         l_buff=i;//单边桥的右边界点

                         break;
                     }
                 }

                 i=l_border[l_turn_point_t-5]-5;
                 if(l_high_start>0)
                 {
                     for(j=l_turn_point_t-8;j>10;j--)
                     {
                         if(bin_image[j-1][i]-bin_image[j][i]==255)//单边桥的上点
                         {

                             high=l_high_start-(j-1);//单边桥的高度

                             find_l=1;
                             break;
                         }
                     }
                 }

                 if(find_l>0)
                 {
                     for(j=l_high_start-2;j>=l_high_start-high+1;j--)
                     {
                         l_border[j]=(uint8)limit_a_b(l_border[j]-(r_border[j]-l_buff)-6,3,117);
                     }
                 }
             }
         }

         if(l_border[55]>40)//如果左单边桥的下点在图像下
         {
             i=l_border[55];
             for(j=59;j>20;j--)
             {
                 if(bin_image[j-1][i-10]-bin_image[j][i-10]==255)
                 {
                     shang_high_point=j;//单边桥的上点

                     break;
                 }
             }

             for(j=59;j>shang_high_point;j--)
             {
                 l_border[j]=(uint8)limit_a_b(l_border[j]-(r_border[j]-i)-11,3,117);
             }
         }
         if(shang_high_point>50&&points_l[0][0]<40)
         {
             origin=60;
             shang_high_point=0;
             left_bridge_into_state=6;
         }

     break;
     case 6:
         disantance += motor_value.receive_left_speed_data-motor_value.receive_right_speed_data;
         tft180_show_int                 (100,80, disantance, 5);
         if(disantance>5000)
         {
             high=0;
             shang_high_point=0;
             l_buff=0;
             r_buff=0;
             left_bridge_into_state=0;
             disantance=0;
             abcd=0;
             speed=hou_speed;
             finish=1;
eeee=1;
bbb=1;


         }
    }
}


//右单边桥函数
uint8 right_bridge_into_state=0;
void right_bridge_judge()
{
    int i=0,j=0,find_l=0,find_r=0,l_high_start=0,r_high_start=0,l_bridge_point=0,kk=0,r_bredge_error=0;
    static int8 high=0,shang_high_point=0,l_buff=0,r_buff=0;
    uint8 black1=0,white1=0,black2=0,white2=0,zhengqie=0;


    if(right_bridge_into_state!=0)
            {
                disantance += motor_value.receive_left_speed_data-motor_value.receive_right_speed_data;

                         if(disantance>50000)
                         {

                                          high=0;
                                          shang_high_point=0;
                                          l_buff=0;
                                          r_buff=0;
                                          right_bridge_into_state=0;
                                          disantance=0;
                                          abcd=0;
                                          speed=hou_speed;
                                          finish=1;
                                          steer_1.center_num =4300;//4900
                                       steer_2.center_num=5200;//4600
                                       steer_3.center_num=4900;//3300
                                       steer_4.center_num=4200;

                         }
            }



    switch(right_bridge_into_state)
    {
    case 0:

        l_turn_judge_limit(55,5,8,10,10);
        r_turn_judge_limit(55,5,10,8,80);

        r_bredge_error=wan_dao();
        if(l_turn_max_t>0)
        {
            if(l_border[l_turn_point_t]>l_border[l_turn_point_t-5])
            {
                r_bredge_error=1;
            }
        }

        if(r_bredge_error==0&&r_turn_max_t>0)
        {
            if(r_border[r_turn_point_t]<110&&l_lost<5&&r_lost<5)
            {
                i=r_border[r_turn_point_t-6]+3;
                for(j=r_turn_point_t-6;j>=5;j--)
                {
                    if(bin_image[j][i]==255)
                    {
                        zhengqie=1;
                        break;
                    }
                }
                if(zhengqie==1)
                {
                    if(r_border[r_turn_point_t]-r_border[r_turn_point_t-5]>0)
                    {
                        for(i=r_border[r_turn_point_t-6];i<=r_border[r_turn_point_t-6]+15;i++)
                        {
                            if(bin_image[r_turn_point_t-6][i]==0)
                                black1++;
                        }
                        for(i=r_border[r_turn_point_t-6];i>=r_border[r_turn_point_t-6]-10;i--)
                        {
                            if(bin_image[r_turn_point_t-6][i]==255)
                                white1++;
                        }
                        if(black1>=10&&white1>=10)//如果第1块单边桥确定，判定第2块单边桥
                        {

                            kk=(l_border[r_turn_point_t-6]+r_border[r_turn_point_t-6])/2;
                            for(j=r_turn_point_t-6;j>4;j--)
                            {
                                if(bin_image[j][kk]==0)
                                {
                                    for(i=kk;i<r_border[j];i++)
                                    {
                                        if(bin_image[j][i]==255)
                                        {
                                            if(i<r_border[r_turn_point_t-6]-5)
                                            {
                                                r_bredge_error=1;
                                                break;
                                            }

                                                white2++;
                                                //tft180_show_int                 (75,80, 5, 1);
                                                if(white2>3)
                                                {
                                                    tft_draw_circle(func_limit_ab(l_border[l_turn_point_t-5], 10, 110), func_limit_ab(l_turn_point_t-5,10,50), uesr_RED);
                                                    gpio_high(P33_10);
                                                    steer_flag=1;
                                                    speed=280;//300
                                                    pwm_set_duty(ATOM1_CH6_P02_6,2800);//1
                                                    pwm_set_duty(ATOM1_CH5_P02_5,6700);//2
                                                    pwm_set_duty(ATOM1_CH4_P02_4,3400);//3500 shenggqo
                                                    pwm_set_duty(ATOM1_CH7_P02_7,5500);//5400 jianshao
                                                    //tft_draw_circle(func_limit_ab(kk, 10, 110), func_limit_ab(j,10,50), uesr_RED);
                                                    l_bridge_point=j;
                                                    gpio_high(P33_10);
                                                    right_bridge_into_state=1;
                                                    break;
                                                }
                                            }
                                        }

                                    }
                                    if(l_bridge_point>0)
                                        break;
                                }
                        }
                    }
                }
            }
        }

    break;
    case 1:
        gpio_low(P33_10);

        for(j=55;j>5;j--)
        {
              if(bin_image[j][70]-bin_image[j-1][70]==255)
              {
                    r_high_start=j;//单边桥的下点
                    for(i=70;i>20;i--)
                    {
                        if(bin_image[j-3][i-1]-bin_image[j-3][i]==255)//单边桥的左右边界点
                        {
                            for(;j>5;j--)
                            {
                                if(bin_image[j-1][70]-bin_image[j][70]==255)//单边桥的上点
                                {
                                    high=r_high_start-(j-1);//单边桥的高度
                                    r_buff=i;//单边桥的右边界点
                                    find_r=1;

                                    origin=45;
                                    break;
                                }
                            }
                        }
                        if(find_r==1)
                        break;
                    }
                    if(find_r==1)
                    break;
              }
        }
        if(r_buff>0)
        {
            for(j=r_high_start-3;j>=r_high_start-high+2;j--)
            {
                r_border[j]=limit_a_b(r_border[j]+(r_buff-l_border[j]),3,117);
            }
        }
        if(r_border[30]<70||bin_image[70][45]==0)
        {
            origin=45;
            right_bridge_into_state=3;
        }
    break;
    case 2://左前右后
        if(shang_high_point==0)
        {
            l_turn_judge_limit(58,5,10,20,45);

            if(l_turn_point_t>0)
            {

                i=l_border[l_turn_point_t-5]-5;
                for(j=l_turn_point_t-8;j<58;j++)
                {
                     if(bin_image[j][i]-bin_image[j-1][i]==255)//单边桥的下点
                     {
                           l_high_start=j;
                           break;
                     }
                }

                j=l_turn_point_t-8;
                for(i=l_border[l_turn_point_t-5]-2;i<90;i++)//找左单边桥的右边界点
                {
                    if(bin_image[j][i]-bin_image[j][i-1]==255)
                    {
                        l_buff=i;//单边桥的右边界点

                        break;
                    }
                }

                i=l_border[l_turn_point_t-5]-5;
                if(l_high_start>0)
                {
                    for(j=l_turn_point_t-8;j>10;j--)
                    {
                        if(bin_image[j-1][i]-bin_image[j][i]==255)//单边桥的上点
                        {//tft180_show_int                 (65,80,j-1,2);

                            high=l_high_start-(j-1);//单边桥的高度
                            tft_draw_circle(i, j-1, uesr_RED);
                            find_l=1;
                            break;
                        }
                    }
                }

                if(find_l>0)
                {
                    for(j=l_high_start-2;j>=l_high_start-high+1;j--)
                    {
                        l_border[j]=limit_a_b(l_border[j]-(r_border[j]-l_buff)-4,3,117);
                    }
                }
            }
        }

        if(l_border[55]>40)//如果左单边桥的下点在图像下
        {
            i=l_border[55];
            for(j=59;j>20;j--)
            {
                if(bin_image[j-1][i-10]-bin_image[j][i-10]==255)
                {
                    shang_high_point=j;//单边桥的上点
                    //Target_Speed=80;
                    break;
                }
            }

            for(j=59;j>shang_high_point;j--)
            {
                l_border[j]=limit_a_b(l_border[j]-(r_border[j]-i)-5,3,117);
            }
        }

        r_turn_judge_limit(58,5,10,15,80);
        //tft180_show_int                 (5,100,r_turn_point_t,2);
        if(r_turn_point_t>0)//找右单边桥的边界
        {
            //tft_draw_circle(r_border[r_turn_point_t-5], r_turn_point_t-5, uesr_RED);
            i=r_border[r_turn_point_t-8]+5;
            for(j=r_turn_point_t-8;j<58;j++)
            {
                 if(bin_image[j][i]-bin_image[j-1][i]==255)//单边桥的下点
                 {
                       r_high_start=j;
                       break;
                 }
            }

            j=r_turn_point_t-8;
            for(i=r_border[r_turn_point_t-5]+2;i>30;i--)//找右单边桥的左边界点
            {
                if(bin_image[j][i-1]-bin_image[j][i]==255)
                {
                    r_buff=i-1;//右单边桥的左边界点
                    //tft_draw_circle(l_buff, j, uesr_RED);
                    break;
                }
            }

            i=r_border[r_turn_point_t-8]+5;
            if(r_high_start>0)
            {
                for(j=r_turn_point_t-5;j>10;j--)
                {
                    if(bin_image[j-1][i]-bin_image[j][i]==255)//单边桥的上点
                    {
                        high=r_high_start-(j-1);//单边桥的高度
                        find_r=1;
                        break;
                    }
                }
            }
            if(find_r>0)
            {
                for(j=r_high_start-1;j>=r_high_start-high+1;j--)
                {
                    r_border[j]=limit_a_b(r_border[j]+(r_buff-l_border[j])+3,3,117);
                }
            }
        }

        if(shang_high_point>50&&points_l[0][0]<40&&bin_image[45][80]==0)
        {
            l_turn_judge_limit(58,5,10,10,45);
            if(l_turn_point_t>0&&l_border[l_turn_point_t-5]>l_border[l_turn_point_t])//如果还有左单边桥
            {
                origin=45;
                shang_high_point=0;
                right_bridge_into_state=3;

            }
            else
            {
                origin=45;
                shang_high_point=0;
                right_bridge_into_state=4;
            }
        }

    break;
    case 3://右前左后

        if(shang_high_point==0)
        {
            r_turn_judge_limit(58,5,10,20,80);


            if(r_turn_point_t>0)//找右单边桥的边界
            {

                i=r_border[r_turn_point_t-8]+5;
                for(j=r_turn_point_t-8;j<58;j++)
                {
                     if(bin_image[j][i]-bin_image[j-1][i]==255)//单边桥的下点
                     {
                           r_high_start=j;
                           break;
                     }
                }

                j=r_turn_point_t-8;
                for(i=r_border[r_turn_point_t-5]+2;i>30;i--)//找右单边桥的左边界点
                {
                    if(bin_image[j][i-1]-bin_image[j][i]==255)
                    {
                        r_buff=i-1;//右单边桥的左边界点
                        //tft_draw_circle(l_buff, j, uesr_RED);
                        break;
                    }
                }

                i=r_border[r_turn_point_t-8]+5;
                if(r_high_start>0)
                {
                    for(j=r_turn_point_t-5;j>10;j--)
                    {
                        if(bin_image[j-1][i]-bin_image[j][i]==255)//单边桥的上点
                        {
                            high=r_high_start-(j-1);//单边桥的高度
                            find_r=1;
                            break;
                        }
                    }
                }
                if(find_r>0)
                {
                    for(j=r_high_start-1;j>=r_high_start-high+1;j--)
                    {
                        r_border[j]=limit_a_b(r_border[j]+(r_buff-l_border[j])+3,3,117);
                    }
                }
            }
        }

        if(r_border[55]<80)//如果右单边桥的下点在图像下
        {
            i=r_border[55];
            for(j=59;j>20;j--)
            {
                if(bin_image[j-1][i+10]-bin_image[j][i+10]==255)
                {
                    shang_high_point=j;//单边桥的上点
                    //Target_Speed=80;
                    break;
                }
            }

            for(j=59;j>shang_high_point;j--)
            {
                r_border[j]=limit_a_b(r_border[j]+(i-l_border[j])+2,3,117);
            }
        }

        l_turn_judge_limit(58,5,10,20,45);
        if(l_turn_point_t>0)
        {
            i=l_border[l_turn_point_t-5]-5;
            for(j=l_turn_point_t-8;j<58;j++)
            {
                 if(bin_image[j][i]-bin_image[j-1][i]==255)//单边桥的下点
                 {tft180_show_int                 (45,80,2,1);
                       l_high_start=j;
                       break;
                 }
            }

            j=l_turn_point_t-8;
            for(i=l_border[l_turn_point_t-5]-2;i<90;i++)//找左单边桥的右边界点
            {
                if(bin_image[j][i]-bin_image[j][i-1]==255)
                {
                    l_buff=i;//单边桥的右边界点
                    //tft_draw_circle(l_buff, j, uesr_RED);
                    break;
                }
            }

            i=l_border[l_turn_point_t-5]-5;
            if(l_high_start>0)
            {
                for(j=l_turn_point_t-8;j>10;j--)
                {
                    if(bin_image[j-1][i]-bin_image[j][i]==255)//单边桥的上点
                    {tft180_show_int                 (65,80,j-1,2);

                        high=l_high_start-(j-1);//单边桥的高度
                       // tft_draw_circle(i, j-1, uesr_RED);
                        find_l=1;
                        break;
                    }
                }
            }

            if(find_l>0)
            {
                for(j=l_high_start-2;j>=l_high_start-high+1;j--)
                {
                    l_border[j]=limit_a_b(l_border[j]-(r_border[j]-l_buff)-6,3,117);
                }
            }
        }

        if(shang_high_point>50&&points_r[0][0]>80&&bin_image[45][40]==0)
        {
            r_turn_judge_limit(58,5,10,10,80);

            if(r_turn_point_t>0&&r_border[r_turn_point_t-5]<r_border[r_turn_point_t])//如果还有右单边桥
            {
                origin=75;
                shang_high_point=0;
                right_bridge_into_state=2;

            }
            else
            {
                origin=75;
                shang_high_point=0;
                right_bridge_into_state=5;
            }
        }
    break;
    case 4:
        if(shang_high_point==0)//最后1个右单边桥
        {
            r_turn_judge_limit(58,5,10,20,80);
            tft180_show_int                 (5,80,l_turn_point_t,2);
            tft180_show_int                 (45,80,0,1);
            tft180_show_int                 (45,100,l_border[l_turn_point_t-5],2);

            if(r_turn_point_t>0)//找右单边桥的边界
            {
                //tft_draw_circle(r_border[r_turn_point_t-5], r_turn_point_t-5, uesr_RED);
                i=r_border[r_turn_point_t-8]+5;
                for(j=r_turn_point_t-8;j<58;j++)
                {
                     if(bin_image[j][i]-bin_image[j-1][i]==255)//单边桥的下点
                     {
                           r_high_start=j;
                           break;
                     }
                }

                j=r_turn_point_t-8;
                for(i=r_border[r_turn_point_t-5]+2;i>30;i--)//找右单边桥的左边界点
                {
                    if(bin_image[j][i-1]-bin_image[j][i]==255)
                    {
                        r_buff=i-1;//右单边桥的左边界点
                       // tft_draw_circle(l_buff, j, uesr_RED);
                        break;
                    }
                }

                i=r_border[r_turn_point_t-8]+5;
                if(r_high_start>0)
                {
                    for(j=r_turn_point_t-5;j>10;j--)
                    {
                        if(bin_image[j-1][i]-bin_image[j][i]==255)//单边桥的上点
                        {
                            high=r_high_start-(j-1);//单边桥的高度
                            find_r=1;
                            break;
                        }
                    }
                }
                if(find_r>0)
                {
                    for(j=r_high_start-1;j>=r_high_start-high+1;j--)
                    {
                        r_border[j]=limit_a_b(r_border[j]+(r_buff-l_border[j])+3,3,117);
                    }
                }
            }
        }

        if(r_border[55]<80)//如果右单边桥的下点在图像下
        {
            i=r_border[55];
            for(j=59;j>20;j--)
            {
                if(bin_image[j-1][i+10]-bin_image[j][i+10]==255)
                {
                    shang_high_point=j;//单边桥的上点
                    //Target_Speed=80;
                    break;
                }
            }

            for(j=59;j>shang_high_point;j--)
            {
                r_border[j]=limit_a_b(r_border[j]+(i-l_border[j])+2,3,117);
            }
        }
        if(shang_high_point>50&&points_r[0][0]>80)
        {
            origin=60;
            shang_high_point=0;
            right_bridge_into_state=6;
        }
     break;
     case 5:
         if(shang_high_point==0)//最后1个左单边桥
         {
             l_turn_judge_limit(58,5,10,20,45);
             tft180_show_int                 (5,80,l_turn_point_t,2);
             tft180_show_int                 (45,80,0,1);
             tft180_show_int                 (45,100,l_border[l_turn_point_t-5],2);
             if(l_turn_point_t>0)
             {//tft_draw_circle(l_border[l_turn_point_t-5], l_turn_point_t-5, uesr_RED);

                 i=l_border[l_turn_point_t-5]-5;
                 for(j=l_turn_point_t-8;j<58;j++)
                 {
                      if(bin_image[j][i]-bin_image[j-1][i]==255)//单边桥的下点
                      {tft180_show_int                 (45,80,2,1);
                            l_high_start=j;
                            break;
                      }
                 }

                 j=l_turn_point_t-8;
                 for(i=l_border[l_turn_point_t-5]-2;i<90;i++)//找左单边桥的右边界点
                 {
                     if(bin_image[j][i]-bin_image[j][i-1]==255)
                     {
                         l_buff=i;//单边桥的右边界点
                         //tft_draw_circle(l_buff, j, uesr_RED);
                         break;
                     }
                 }

                 i=l_border[l_turn_point_t-5]-5;
                 if(l_high_start>0)
                 {
                     for(j=l_turn_point_t-8;j>10;j--)
                     {
                         if(bin_image[j-1][i]-bin_image[j][i]==255)//单边桥的上点
                         {tft180_show_int                 (65,80,j-1,2);

                             high=l_high_start-(j-1);//单边桥的高度
                             //tft_draw_circle(i, j-1, uesr_RED);
                             find_l=1;
                             break;
                         }
                     }
                 }

                 if(find_l>0)
                 {
                     for(j=l_high_start-2;j>=l_high_start-high+1;j--)
                     {
                         l_border[j]=limit_a_b(l_border[j]-(r_border[j]-l_buff)-6,3,117);
                     }
                 }
             }
         }

         if(l_border[55]>40)//如果左单边桥的下点在图像下
         {
             i=l_border[55];
             for(j=59;j>20;j--)
             {
                 if(bin_image[j-1][i-10]-bin_image[j][i-10]==255)
                 {
                     shang_high_point=j;//单边桥的上点
                     //Target_Speed=80;
                     break;
                 }
             }

             for(j=59;j>shang_high_point;j--)
             {
                 l_border[j]=limit_a_b(l_border[j]-(r_border[j]-i)-11,3,117);
             }
         }
         if(shang_high_point>50&&points_l[0][0]<40)
         {
             origin=60;
             shang_high_point=0;
             right_bridge_into_state=6;
         }

     break;
     case 6:
         disantance += motor_value.receive_left_speed_data-motor_value.receive_right_speed_data;
         //tft180_show_int                 (100,80, disantance, 5);
         if(disantance>5000)
         {tft180_show_int                 (100,100, 8, 1);
             high=0;
             shang_high_point=0;
             l_buff=0;
             r_buff=0;
             right_bridge_into_state=0;
             disantance=0;
             abcd=0;
             speed=hou_speed;
             finish=1;
             eeee=1;
             steer_1.center_num =4300;//4900
                          steer_2.center_num=5200;//4600
                          steer_3.center_num=4900;//3300
                          steer_4.center_num=4200;
         }
    }
}


void right_bridge_judge3()//3个右单边桥
{
    int i=0,j=0,find_l=0,find_r=0,l_high_start=0,r_high_start=0,l_bridge_point=0,kk=0,r_bredge_error=0;
    static int8 high=0,shang_high_point=0,l_buff=0,r_buff=0;
    uint8 black1=0,white1=0,black2=0,white2=0,zhengqie=0;



    if(right_bridge_into_state!=0)
        {
            disantance += motor_value.receive_left_speed_data-motor_value.receive_right_speed_data;

                     if(disantance>50000)
                     {

                                      high=0;
                                      shang_high_point=0;
                                      l_buff=0;
                                      r_buff=0;
                                      right_bridge_into_state=0;
                                      disantance=0;
                                      abcd=0;
                                      speed=hou_speed;
                                      finish=1;
                                      steer_1.center_num =4300;//4900
                                   steer_2.center_num=5200;//4600
                                   steer_3.center_num=4900;//3300
                                   steer_4.center_num=4200;

                     }
        }
    switch(right_bridge_into_state)
    {
    case 0:

        l_turn_judge_limit(55,5,8,10,10);
        r_turn_judge_limit(55,5,10,8,80);

        r_bredge_error=wan_dao();
        if(l_turn_max_t>0)
        {
            if(l_border[l_turn_point_t]>l_border[l_turn_point_t-5])
            {
                r_bredge_error=1;
            }
        }

        if(r_bredge_error==0&&r_turn_max_t>0)
        {
            if(r_border[r_turn_point_t]<110&&l_lost<5&&r_lost<5)
            {
                i=r_border[r_turn_point_t-6]+3;
                for(j=r_turn_point_t-6;j>=5;j--)
                {
                    if(bin_image[j][i]==255)
                    {
                        zhengqie=1;
                        break;
                    }
                }
                if(zhengqie==1)
                {
                    if(r_border[r_turn_point_t]-r_border[r_turn_point_t-5]>0)
                    {
                        for(i=r_border[r_turn_point_t-6];i<=r_border[r_turn_point_t-6]+15;i++)
                        {
                            if(bin_image[r_turn_point_t-6][i]==0)
                                black1++;
                        }
                        for(i=r_border[r_turn_point_t-6];i>=r_border[r_turn_point_t-6]-10;i--)
                        {
                            if(bin_image[r_turn_point_t-6][i]==255)
                                white1++;
                        }
                        if(black1>=10&&white1>=10)//如果第1块单边桥确定，判定第2块单边桥
                        {

                            kk=(l_border[r_turn_point_t-6]+r_border[r_turn_point_t-6])/2;
                            for(j=r_turn_point_t-6;j>4;j--)
                            {
                                if(bin_image[j][kk]==0)
                                {
                                    for(i=kk;i<r_border[j];i++)
                                    {
                                        if(bin_image[j][i]==255)
                                        {
                                            if(i<r_border[r_turn_point_t-6]-5)
                                            {
                                                r_bredge_error=1;
                                                break;
                                            }

                                                white2++;

                                                if(white2>3)
                                                {
                                                    tft_draw_circle(func_limit_ab(l_border[l_turn_point_t-5], 10, 110), func_limit_ab(l_turn_point_t-5,10,50), uesr_RED);
                                                    gpio_high(P33_10);
                                                    steer_flag=1;
                                                    speed=280;//300
                                                    pwm_set_duty(ATOM1_CH6_P02_6,2800);//1
                                                    pwm_set_duty(ATOM1_CH5_P02_5,6700);//2
                                                    pwm_set_duty(ATOM1_CH4_P02_4,3400);//3500 shenggqo
                                                    pwm_set_duty(ATOM1_CH7_P02_7,5500);//5400 jianshao

                                                    l_bridge_point=j;
                                                    gpio_high(P33_10);
                                                    right_bridge_into_state=1;
                                                    break;
                                                }
                                            }
                                        }

                                    }
                                    if(l_bridge_point>0)
                                        break;
                                }
                        }
                    }
                }
            }
        }

    break;
    case 1:
        gpio_low(P33_10);

        for(j=55;j>5;j--)
        {
              if(bin_image[j][70]-bin_image[j-1][70]==255)
              {
                    r_high_start=j;//单边桥的下点
                    for(i=70;i>20;i--)
                    {
                        if(bin_image[j-3][i-1]-bin_image[j-3][i]==255)//单边桥的左右边界点
                        {
                            for(;j>5;j--)
                            {
                                if(bin_image[j-1][70]-bin_image[j][70]==255)//单边桥的上点
                                {
                                    high=r_high_start-(j-1);//单边桥的高度
                                    r_buff=i;//单边桥的右边界点
                                    find_r=1;

                                    origin=45;
                                    break;
                                }
                            }
                        }
                        if(find_r==1)
                        break;
                    }
                    if(find_r==1)
                    break;
              }
        }
        if(r_buff>0)
        {
            for(j=r_high_start-3;j>=r_high_start-high+2;j--)
            {
                r_border[j]=limit_a_b(r_border[j]+(r_buff-l_border[j]),3,117);
            }
        }
        if(r_border[30]<70||bin_image[70][45]==0)
        {
            origin=45;
            right_bridge_into_state=3;
        }
    break;
    case 2:
        if(shang_high_point==0)
                {
                    l_turn_judge_limit(58,5,10,20,45);

                    if(l_turn_point_t>0)
                    {

                        i=l_border[l_turn_point_t-5]-5;
                        for(j=l_turn_point_t-8;j<58;j++)
                        {
                             if(bin_image[j][i]-bin_image[j-1][i]==255)//单边桥的下点
                             {
                                   l_high_start=j;
                                   break;
                             }
                        }

                        j=l_turn_point_t-8;
                        for(i=l_border[l_turn_point_t-5]-2;i<90;i++)//找左单边桥的右边界点
                        {
                            if(bin_image[j][i]-bin_image[j][i-1]==255)
                            {
                                l_buff=i;//单边桥的右边界点

                                break;
                            }
                        }

                        i=l_border[l_turn_point_t-5]-5;
                        if(l_high_start>0)
                        {
                            for(j=l_turn_point_t-8;j>10;j--)
                            {
                                if(bin_image[j-1][i]-bin_image[j][i]==255)//单边桥的上点
                                {//tft180_show_int                 (65,80,j-1,2);

                                    high=l_high_start-(j-1);//单边桥的高度
                                    tft_draw_circle(i, j-1, uesr_RED);
                                    find_l=1;
                                    break;
                                }
                            }
                        }

                        if(find_l>0)
                        {
                            for(j=l_high_start-2;j>=l_high_start-high+1;j--)
                            {
                                l_border[j]=limit_a_b(l_border[j]-(r_border[j]-l_buff)-4,3,117);
                            }
                        }
                    }
                }

                if(l_border[55]>40)//如果左单边桥的下点在图像下
                {
                    i=l_border[55];
                    for(j=59;j>20;j--)
                    {
                        if(bin_image[j-1][i-10]-bin_image[j][i-10]==255)
                        {
                            shang_high_point=j;//单边桥的上点

                            break;
                        }
                    }

                    for(j=59;j>shang_high_point;j--)
                    {
                        l_border[j]=limit_a_b(l_border[j]-(r_border[j]-i)-5,3,117);
                    }
                }

                r_turn_judge_limit(58,5,10,15,80);
                //tft180_show_int                 (5,100,r_turn_point_t,2);
                if(r_turn_point_t>0)//找右单边桥的边界
                {
                    //tft_draw_circle(r_border[r_turn_point_t-5], r_turn_point_t-5, uesr_RED);
                    i=r_border[r_turn_point_t-8]+5;
                    for(j=r_turn_point_t-8;j<58;j++)
                    {
                         if(bin_image[j][i]-bin_image[j-1][i]==255)//单边桥的下点
                         {
                               r_high_start=j;
                               break;
                         }
                    }

                    j=r_turn_point_t-8;
                    for(i=r_border[r_turn_point_t-5]+2;i>30;i--)//找右单边桥的左边界点
                    {
                        if(bin_image[j][i-1]-bin_image[j][i]==255)
                        {
                            r_buff=i-1;//右单边桥的左边界点
                            //tft_draw_circle(l_buff, j, uesr_RED);
                            break;
                        }
                    }

                    i=r_border[r_turn_point_t-8]+5;
                    if(r_high_start>0)
                    {
                        for(j=r_turn_point_t-5;j>10;j--)
                        {
                            if(bin_image[j-1][i]-bin_image[j][i]==255)//单边桥的上点
                            {
                                high=r_high_start-(j-1);//单边桥的高度
                                find_r=1;
                                break;
                            }
                        }
                    }
                    if(find_r>0)
                    {
                        for(j=r_high_start-1;j>=r_high_start-high+1;j--)
                        {
                            r_border[j]=limit_a_b(r_border[j]+(r_buff-l_border[j])+3,3,117);
                        }
                    }
                }

                if(shang_high_point>50&&points_l[0][0]<40&&bin_image[45][80]==0)
                {

                        origin=45;
                        shang_high_point=0;
                        right_bridge_into_state=4;

                }
        break;
        case 3:

            if(shang_high_point==0)
            {
                r_turn_judge_limit(58,5,10,20,80);


                if(r_turn_point_t>0)//找右单边桥的边界
                {
                    //tft_draw_circle(r_border[r_turn_point_t-5], r_turn_point_t-5, uesr_RED);
                    i=r_border[r_turn_point_t-8]+5;
                    for(j=r_turn_point_t-8;j<58;j++)
                    {
                         if(bin_image[j][i]-bin_image[j-1][i]==255)//单边桥的下点
                         {
                               r_high_start=j;
                               break;
                         }
                    }

                    j=r_turn_point_t-8;
                    for(i=r_border[r_turn_point_t-5]+2;i>30;i--)//找右单边桥的左边界点
                    {
                        if(bin_image[j][i-1]-bin_image[j][i]==255)
                        {
                            r_buff=i-1;//右单边桥的左边界点
                            //tft_draw_circle(l_buff, j, uesr_RED);
                            break;
                        }
                    }

                    i=r_border[r_turn_point_t-8]+5;
                    if(r_high_start>0)
                    {
                        for(j=r_turn_point_t-5;j>10;j--)
                        {
                            if(bin_image[j-1][i]-bin_image[j][i]==255)//单边桥的上点
                            {
                                high=r_high_start-(j-1);//单边桥的高度
                                find_r=1;
                                break;
                            }
                        }
                    }
                    if(find_r>0)
                    {
                        for(j=r_high_start-1;j>=r_high_start-high+1;j--)
                        {
                            r_border[j]=limit_a_b(r_border[j]+(r_buff-l_border[j])+3,3,117);
                        }
                    }
                }
            }

            if(r_border[55]<80)//如果右单边桥的下点在图像下
            {
                i=r_border[55];
                for(j=59;j>20;j--)
                {
                    if(bin_image[j-1][i+10]-bin_image[j][i+10]==255)
                    {
                        shang_high_point=j;//单边桥的上点
                        //Target_Speed=80;
                        break;
                    }
                }

                for(j=59;j>shang_high_point;j--)
                {
                    r_border[j]=limit_a_b(r_border[j]+(i-l_border[j])+2,3,117);
                }
            }

            l_turn_judge_limit(58,5,10,20,45);
            if(l_turn_point_t>0)
            {
                i=l_border[l_turn_point_t-5]-5;
                for(j=l_turn_point_t-8;j<58;j++)
                {
                     if(bin_image[j][i]-bin_image[j-1][i]==255)//单边桥的下点
                     {tft180_show_int                 (45,80,2,1);
                           l_high_start=j;
                           break;
                     }
                }

                j=l_turn_point_t-8;
                for(i=l_border[l_turn_point_t-5]-2;i<90;i++)//找左单边桥的右边界点
                {
                    if(bin_image[j][i]-bin_image[j][i-1]==255)
                    {
                        l_buff=i;//单边桥的右边界点
                        //tft_draw_circle(l_buff, j, uesr_RED);
                        break;
                    }
                }

                i=l_border[l_turn_point_t-5]-5;
                if(l_high_start>0)
                {
                    for(j=l_turn_point_t-8;j>10;j--)
                    {
                        if(bin_image[j-1][i]-bin_image[j][i]==255)//单边桥的上点
                        {tft180_show_int                 (65,80,j-1,2);

                            high=l_high_start-(j-1);//单边桥的高度
                           // tft_draw_circle(i, j-1, uesr_RED);
                            find_l=1;
                            break;
                        }
                    }
                }

                if(find_l>0)
                {
                    for(j=l_high_start-2;j>=l_high_start-high+1;j--)
                    {
                        l_border[j]=limit_a_b(l_border[j]-(r_border[j]-l_buff)-6,3,117);
                    }
                }
            }

            if(shang_high_point>50&&points_r[0][0]>80&&bin_image[45][40]==0)
            {
                r_turn_judge_limit(58,5,10,10,80);

                if(r_turn_point_t>0&&r_border[r_turn_point_t-5]<r_border[r_turn_point_t])//如果还有右单边桥
                {
                    origin=75;
                    shang_high_point=0;
                    right_bridge_into_state=2;

                }

            }
    break;
    case 4:
        if(shang_high_point==0)//最后1个右单边桥
        {
            r_turn_judge_limit(58,5,10,20,80);


            if(r_turn_point_t>0)//找右单边桥的边界
            {

                i=r_border[r_turn_point_t-8]+5;
                for(j=r_turn_point_t-8;j<58;j++)
                {
                     if(bin_image[j][i]-bin_image[j-1][i]==255)//单边桥的下点
                     {
                           r_high_start=j;
                           break;
                     }
                }

                j=r_turn_point_t-8;
                for(i=r_border[r_turn_point_t-5]+2;i>30;i--)//找右单边桥的左边界点
                {
                    if(bin_image[j][i-1]-bin_image[j][i]==255)
                    {
                        r_buff=i-1;//右单边桥的左边界点

                        break;
                    }
                }

                i=r_border[r_turn_point_t-8]+5;
                if(r_high_start>0)
                {
                    for(j=r_turn_point_t-5;j>10;j--)
                    {
                        if(bin_image[j-1][i]-bin_image[j][i]==255)//单边桥的上点
                        {
                            high=r_high_start-(j-1);//单边桥的高度
                            find_r=1;
                            break;
                        }
                    }
                }
                if(find_r>0)
                {
                    for(j=r_high_start-1;j>=r_high_start-high+1;j--)
                    {
                        r_border[j]=limit_a_b(r_border[j]+(r_buff-l_border[j])+3,3,117);
                    }
                }
            }
        }

        if(r_border[55]<80)//如果右单边桥的下点在图像下
        {
            i=r_border[55];
            for(j=59;j>20;j--)
            {
                if(bin_image[j-1][i+10]-bin_image[j][i+10]==255)
                {
                    shang_high_point=j;//单边桥的上点

                    break;
                }
            }

            for(j=59;j>shang_high_point;j--)
            {
                r_border[j]=limit_a_b(r_border[j]+(i-l_border[j])+2,3,117);
            }
        }
        if(shang_high_point>50&&points_r[0][0]>80)
        {
            origin=60;
            shang_high_point=0;
            right_bridge_into_state=6;
        }
     break;
     case 6:
         disantance += motor_value.receive_left_speed_data-motor_value.receive_right_speed_data;

         if(disantance>5000)
         {
             high=0;
             shang_high_point=0;
             l_buff=0;
             r_buff=0;
             right_bridge_into_state=0;
             disantance=0;
             abcd=0;
             speed=hou_speed;
             finish=1;
             eeee=1;
             bbb=1;
             steer_1.center_num =4300;//4900
          steer_2.center_num=5200;//4600
          steer_3.center_num=4900;//3300
          steer_4.center_num=4200;
         }
    }
}

void right_bridge_judge3_di()//3个右单边桥
{
    int i=0,j=0,find_l=0,find_r=0,l_high_start=0,r_high_start=0,l_bridge_point=0,kk=0,r_bredge_error=0;
    static int8 high=0,shang_high_point=0,l_buff=0,r_buff=0;
    uint8 black1=0,white1=0,black2=0,white2=0,zhengqie=0;



    if(right_bridge_into_state!=0)
        {
            disantance += motor_value.receive_left_speed_data-motor_value.receive_right_speed_data;

                     if(disantance>50000)
                     {

                                      high=0;
                                      shang_high_point=0;
                                      l_buff=0;
                                      r_buff=0;
                                      right_bridge_into_state=0;
                                      disantance=0;
                                      abcd=0;
                                      speed=hou_speed;
                                      finish=1;


                     }
        }
    switch(right_bridge_into_state)
    {
    case 0:

        l_turn_judge_limit(55,5,8,10,10);
        r_turn_judge_limit(55,5,10,8,80);

        r_bredge_error=wan_dao();
        if(l_turn_max_t>0)
        {
            if(l_border[l_turn_point_t]>l_border[l_turn_point_t-5])
            {
                r_bredge_error=1;
            }
        }

        if(r_bredge_error==0&&r_turn_max_t>0)
        {
            if(r_border[r_turn_point_t]<110&&l_lost<5&&r_lost<5)
            {
                i=r_border[r_turn_point_t-6]+3;
                for(j=r_turn_point_t-6;j>=5;j--)
                {
                    if(bin_image[j][i]==255)
                    {
                        zhengqie=1;
                        break;
                    }
                }
                if(zhengqie==1)
                {
                    if(r_border[r_turn_point_t]-r_border[r_turn_point_t-5]>0)
                    {
                        for(i=r_border[r_turn_point_t-6];i<=r_border[r_turn_point_t-6]+15;i++)
                        {
                            if(bin_image[r_turn_point_t-6][i]==0)
                                black1++;
                        }
                        for(i=r_border[r_turn_point_t-6];i>=r_border[r_turn_point_t-6]-10;i--)
                        {
                            if(bin_image[r_turn_point_t-6][i]==255)
                                white1++;
                        }
                        if(black1>=10&&white1>=10)//如果第1块单边桥确定，判定第2块单边桥
                        {

                            kk=(l_border[r_turn_point_t-6]+r_border[r_turn_point_t-6])/2;
                            for(j=r_turn_point_t-6;j>4;j--)
                            {
                                if(bin_image[j][kk]==0)
                                {
                                    for(i=kk;i<r_border[j];i++)
                                    {
                                        if(bin_image[j][i]==255)
                                        {
                                            if(i<r_border[r_turn_point_t-6]-5)
                                            {
                                                r_bredge_error=1;
                                                break;
                                            }

                                                white2++;

                                                if(white2>3)
                                                {
                                                    tft_draw_circle(func_limit_ab(l_border[l_turn_point_t-5], 10, 110), func_limit_ab(l_turn_point_t-5,10,50), uesr_RED);
                                                    gpio_high(P33_10);




                                                    l_bridge_point=j;
                                                    gpio_high(P33_10);
                                                    right_bridge_into_state=1;
                                                    break;
                                                }
                                            }
                                        }

                                    }
                                    if(l_bridge_point>0)
                                        break;
                                }
                        }
                    }
                }
            }
        }

    break;
    case 1:
        gpio_low(P33_10);

        for(j=55;j>5;j--)
        {
              if(bin_image[j][70]-bin_image[j-1][70]==255)
              {
                    r_high_start=j;//单边桥的下点
                    for(i=70;i>20;i--)
                    {
                        if(bin_image[j-3][i-1]-bin_image[j-3][i]==255)//单边桥的左右边界点
                        {
                            for(;j>5;j--)
                            {
                                if(bin_image[j-1][70]-bin_image[j][70]==255)//单边桥的上点
                                {
                                    high=r_high_start-(j-1);//单边桥的高度
                                    r_buff=i;//单边桥的右边界点
                                    find_r=1;

                                    origin=45;
                                    break;
                                }
                            }
                        }
                        if(find_r==1)
                        break;
                    }
                    if(find_r==1)
                    break;
              }
        }
        if(r_buff>0)
        {
            for(j=r_high_start-3;j>=r_high_start-high+2;j--)
            {
                r_border[j]=limit_a_b(r_border[j]+(r_buff-l_border[j]),3,117);
            }
        }
        if(r_border[30]<70||bin_image[70][45]==0)
        {
            origin=45;
            right_bridge_into_state=3;
        }
    break;
    case 2:
        if(shang_high_point==0)
                {
                    l_turn_judge_limit(58,5,10,20,45);

                    if(l_turn_point_t>0)
                    {

                        i=l_border[l_turn_point_t-5]-5;
                        for(j=l_turn_point_t-8;j<58;j++)
                        {
                             if(bin_image[j][i]-bin_image[j-1][i]==255)//单边桥的下点
                             {
                                   l_high_start=j;
                                   break;
                             }
                        }

                        j=l_turn_point_t-8;
                        for(i=l_border[l_turn_point_t-5]-2;i<90;i++)//找左单边桥的右边界点
                        {
                            if(bin_image[j][i]-bin_image[j][i-1]==255)
                            {
                                l_buff=i;//单边桥的右边界点

                                break;
                            }
                        }

                        i=l_border[l_turn_point_t-5]-5;
                        if(l_high_start>0)
                        {
                            for(j=l_turn_point_t-8;j>10;j--)
                            {
                                if(bin_image[j-1][i]-bin_image[j][i]==255)//单边桥的上点
                                {//tft180_show_int                 (65,80,j-1,2);

                                    high=l_high_start-(j-1);//单边桥的高度
                                    //tft_draw_circle(i, j-1, uesr_RED);
                                    find_l=1;
                                    break;
                                }
                            }
                        }

                        if(find_l>0)
                        {
                            for(j=l_high_start-2;j>=l_high_start-high+1;j--)
                            {
                                l_border[j]=limit_a_b(l_border[j]-(r_border[j]-l_buff)-4,3,117);
                            }
                        }
                    }
                }

                if(l_border[55]>40)//如果左单边桥的下点在图像下
                {
                    i=l_border[55];
                    for(j=59;j>20;j--)
                    {
                        if(bin_image[j-1][i-10]-bin_image[j][i-10]==255)
                        {
                            shang_high_point=j;//单边桥的上点

                            break;
                        }
                    }

                    for(j=59;j>shang_high_point;j--)
                    {
                        l_border[j]=limit_a_b(l_border[j]-(r_border[j]-i)-5,3,117);
                    }
                }

                r_turn_judge_limit(58,5,10,15,80);
                //tft180_show_int                 (5,100,r_turn_point_t,2);
                if(r_turn_point_t>0)//找右单边桥的边界
                {
                    //tft_draw_circle(r_border[r_turn_point_t-5], r_turn_point_t-5, uesr_RED);
                    i=r_border[r_turn_point_t-8]+5;
                    for(j=r_turn_point_t-8;j<58;j++)
                    {
                         if(bin_image[j][i]-bin_image[j-1][i]==255)//单边桥的下点
                         {
                               r_high_start=j;
                               break;
                         }
                    }

                    j=r_turn_point_t-8;
                    for(i=r_border[r_turn_point_t-5]+2;i>30;i--)//找右单边桥的左边界点
                    {
                        if(bin_image[j][i-1]-bin_image[j][i]==255)
                        {
                            r_buff=i-1;//右单边桥的左边界点
                            //tft_draw_circle(l_buff, j, uesr_RED);
                            break;
                        }
                    }

                    i=r_border[r_turn_point_t-8]+5;
                    if(r_high_start>0)
                    {
                        for(j=r_turn_point_t-5;j>10;j--)
                        {
                            if(bin_image[j-1][i]-bin_image[j][i]==255)//单边桥的上点
                            {
                                high=r_high_start-(j-1);//单边桥的高度
                                find_r=1;
                                break;
                            }
                        }
                    }
                    if(find_r>0)
                    {
                        for(j=r_high_start-1;j>=r_high_start-high+1;j--)
                        {
                            r_border[j]=limit_a_b(r_border[j]+(r_buff-l_border[j])+3,3,117);
                        }
                    }
                }

                if(shang_high_point>50&&points_l[0][0]<40&&bin_image[45][80]==0)
                {

                        origin=45;
                        shang_high_point=0;
                        right_bridge_into_state=4;

                }
        break;
        case 3:

            if(shang_high_point==0)
            {
                r_turn_judge_limit(58,5,10,20,80);


                if(r_turn_point_t>0)//找右单边桥的边界
                {
                    //tft_draw_circle(r_border[r_turn_point_t-5], r_turn_point_t-5, uesr_RED);
                    i=r_border[r_turn_point_t-8]+5;
                    for(j=r_turn_point_t-8;j<58;j++)
                    {
                         if(bin_image[j][i]-bin_image[j-1][i]==255)//单边桥的下点
                         {
                               r_high_start=j;
                               break;
                         }
                    }

                    j=r_turn_point_t-8;
                    for(i=r_border[r_turn_point_t-5]+2;i>30;i--)//找右单边桥的左边界点
                    {
                        if(bin_image[j][i-1]-bin_image[j][i]==255)
                        {
                            r_buff=i-1;//右单边桥的左边界点
                            //tft_draw_circle(l_buff, j, uesr_RED);
                            break;
                        }
                    }

                    i=r_border[r_turn_point_t-8]+5;
                    if(r_high_start>0)
                    {
                        for(j=r_turn_point_t-5;j>10;j--)
                        {
                            if(bin_image[j-1][i]-bin_image[j][i]==255)//单边桥的上点
                            {
                                high=r_high_start-(j-1);//单边桥的高度
                                find_r=1;
                                break;
                            }
                        }
                    }
                    if(find_r>0)
                    {
                        for(j=r_high_start-1;j>=r_high_start-high+1;j--)
                        {
                            r_border[j]=limit_a_b(r_border[j]+(r_buff-l_border[j])+3,3,117);
                        }
                    }
                }
            }

            if(r_border[55]<80)//如果右单边桥的下点在图像下
            {
                i=r_border[55];
                for(j=59;j>20;j--)
                {
                    if(bin_image[j-1][i+10]-bin_image[j][i+10]==255)
                    {
                        shang_high_point=j;//单边桥的上点
                        //Target_Speed=80;
                        break;
                    }
                }

                for(j=59;j>shang_high_point;j--)
                {
                    r_border[j]=limit_a_b(r_border[j]+(i-l_border[j])+2,3,117);
                }
            }

            l_turn_judge_limit(58,5,10,20,45);
            if(l_turn_point_t>0)
            {
                i=l_border[l_turn_point_t-5]-5;
                for(j=l_turn_point_t-8;j<58;j++)
                {
                     if(bin_image[j][i]-bin_image[j-1][i]==255)//单边桥的下点
                     {tft180_show_int                 (45,80,2,1);
                           l_high_start=j;
                           break;
                     }
                }

                j=l_turn_point_t-8;
                for(i=l_border[l_turn_point_t-5]-2;i<90;i++)//找左单边桥的右边界点
                {
                    if(bin_image[j][i]-bin_image[j][i-1]==255)
                    {
                        l_buff=i;//单边桥的右边界点
                        //tft_draw_circle(l_buff, j, uesr_RED);
                        break;
                    }
                }

                i=l_border[l_turn_point_t-5]-5;
                if(l_high_start>0)
                {
                    for(j=l_turn_point_t-8;j>10;j--)
                    {
                        if(bin_image[j-1][i]-bin_image[j][i]==255)//单边桥的上点
                        {tft180_show_int                 (65,80,j-1,2);

                            high=l_high_start-(j-1);//单边桥的高度
                           // tft_draw_circle(i, j-1, uesr_RED);
                            find_l=1;
                            break;
                        }
                    }
                }

                if(find_l>0)
                {
                    for(j=l_high_start-2;j>=l_high_start-high+1;j--)
                    {
                        l_border[j]=limit_a_b(l_border[j]-(r_border[j]-l_buff)-6,3,117);
                    }
                }
            }

            if(shang_high_point>50&&points_r[0][0]>80&&bin_image[45][40]==0)
            {
                r_turn_judge_limit(58,5,10,10,80);

                if(r_turn_point_t>0&&r_border[r_turn_point_t-5]<r_border[r_turn_point_t])//如果还有右单边桥
                {
                    origin=75;
                    shang_high_point=0;
                    right_bridge_into_state=2;

                }

            }
    break;
    case 4:
        if(shang_high_point==0)//最后1个右单边桥
        {
            r_turn_judge_limit(58,5,10,20,80);


            if(r_turn_point_t>0)//找右单边桥的边界
            {

                i=r_border[r_turn_point_t-8]+5;
                for(j=r_turn_point_t-8;j<58;j++)
                {
                     if(bin_image[j][i]-bin_image[j-1][i]==255)//单边桥的下点
                     {
                           r_high_start=j;
                           break;
                     }
                }

                j=r_turn_point_t-8;
                for(i=r_border[r_turn_point_t-5]+2;i>30;i--)//找右单边桥的左边界点
                {
                    if(bin_image[j][i-1]-bin_image[j][i]==255)
                    {
                        r_buff=i-1;//右单边桥的左边界点

                        break;
                    }
                }

                i=r_border[r_turn_point_t-8]+5;
                if(r_high_start>0)
                {
                    for(j=r_turn_point_t-5;j>10;j--)
                    {
                        if(bin_image[j-1][i]-bin_image[j][i]==255)//单边桥的上点
                        {
                            high=r_high_start-(j-1);//单边桥的高度
                            find_r=1;
                            break;
                        }
                    }
                }
                if(find_r>0)
                {
                    for(j=r_high_start-1;j>=r_high_start-high+1;j--)
                    {
                        r_border[j]=limit_a_b(r_border[j]+(r_buff-l_border[j])+3,3,117);
                    }
                }
            }
        }

        if(r_border[55]<80)//如果右单边桥的下点在图像下
        {
            i=r_border[55];
            for(j=59;j>20;j--)
            {
                if(bin_image[j-1][i+10]-bin_image[j][i+10]==255)
                {
                    shang_high_point=j;//单边桥的上点

                    break;
                }
            }

            for(j=59;j>shang_high_point;j--)
            {
                r_border[j]=limit_a_b(r_border[j]+(i-l_border[j])+2,3,117);
            }
        }
        if(shang_high_point>50&&points_r[0][0]>80)
        {
            origin=60;
            shang_high_point=0;
            right_bridge_into_state=6;
        }
     break;
     case 6:
         disantance += motor_value.receive_left_speed_data-motor_value.receive_right_speed_data;

         if(disantance>5000)
         {
             high=0;
             shang_high_point=0;
             l_buff=0;
             r_buff=0;
             right_bridge_into_state=0;
             disantance=0;
             abcd=0;
             speed=hou_speed;
             finish=1;
             eeee=1;
             bbb=1;

         }
    }
}

uint8 rapm_flage=0;
//void ramp_judge()//坡道识别
//{
//    uint8 j,count=0,l_count=0,r_count=0;
//    static uint8 go_down=0;
//
//    //tft180_show_int                 (50,65,disantance,5);
//    //tft180_show_int                 (50,100,go_down,1);
//    switch(rapm_flage)
//    {
//    case 0:
//        if(dl1b_distance_mm<=100)
//        {
//            for(j=10;j<50;j++)
//            {
//                if(bin_image[j][40] ==255)  //判断白点
//                {
//                    count++;
//                }
//                if(bin_image[j][60] ==255)  //判断白点
//                {
//                    count++;
//                }
//                if(bin_image[j][80] ==255)  //判断白点
//                {
//                    count++;
//                }
//            }
//           if(count>=115)
//           {
//               for(j=59;j>40;j--)
//               {
//                    if(bin_image[j][2] ==255)  //判断白点
//                    {
//                        l_count++;
//                    }
//                    if(bin_image[j][117] ==255)  //判断白点
//                    {
//                        r_count++;
//                    }
//               }
//               if(l_count>=10||r_count>=10)
//               {
//                   rapm_flage=1;
//                   Target_Speed=400;
//               }
//           }
//        }
//    break;
//    case 1:
//        if(dl1b_distance_mm>5000)
//        {
//            go_down=1;
//        }
//        if(go_down==1)
//        {
//            Target_Speed=240;
//            if(disantance<18888)disantance += motor_value.receive_left_speed_data-motor_value.receive_right_speed_data;
//            if(disantance>18000)
//            {
//                Target_Speed++;
//                if(Target_Speed>=400)
//                    {
//                    go_down=0;
//                    rapm_flage=0;
//                    }
//            }
//        }
//
//    }
//}


extern uint8 stop;
uint8 start_distance_count=0;

void zebra_crossing()//斑马线识别
{
    uint8 i=0,j=0,count=0;
    for(j=45;j<48;j++)//y
    {
        for(i=45;i<75;i++)//x
        {
            if((bin_image[j][i-1]==255&&bin_image[j][i]==0)||(bin_image[j][i-1]==0&&bin_image[j][i]==255))//黑白交界点
            {
                count++;
            }
        }
    }

    if(count>10&&finish==1)
    {
        start_distance_count=1;

    }
    if(start_distance_count==1)
    {
        disantance += motor_value.receive_left_speed_data-motor_value.receive_right_speed_data;
        if(disantance>7000)
        {
            speed=0;
            disantance=0;
        }
    }


}

void leap_image()
{
    uint8 i=0,j=0;
    uint8 find=0,r_judge=0,l_judge=0;

    if(bin_image[20][60]==0||bin_image[20][50]==0||bin_image[20][70]==0)//bin_image[30][60]==0||bin_image[25][60]==0||bin_image[20][60]==0
    {
        start_jump=1;
    }
    if(jump_flaged==1)
    {
        if(bin_image[59][60]==255)
            jump_flaged=2;
    }
    if(bin_image[58][60]==0)
    {
        for(j=58;j>20;j--)
        {
            if(bin_image[j-1][60]-bin_image[j][60]==255)
            {
                for(i=60;i<110;i++)
                {
                    if(bin_image[j-5][i]-bin_image[j-5][i+1]==255)
                    {
                        r_judge=i;
                        break;
                    }

                }
                for(i=60;i>10;i--)
                {
                    if(bin_image[j-5][i]-bin_image[j-5][i-1]==255)
                    {
                        l_judge=i;
                        break;
                    }
                }
                hight1=59;
                hight2=j-1;
                find=1;
                middle_point=(r_judge+l_judge)/2;
                break;
            }
        }
    }
    else
    {
        for(j=58;j>10;j--)
        {
            if(bin_image[j][middle_point]-bin_image[j-1][middle_point]==255)
            {
                hight1=j;
                for(j=hight1-1;j>10;j--)
                {
                    if(bin_image[j-1][middle_point]-bin_image[j][middle_point]==255)
                    {
                        hight2=j-1;
                        find=1;
                        break;
                    }
                }
                if(find==1)
                    break;
            }

        }
    }
    if(find==1)
    {

        for(j=func_limit_ab(hight1+2,10,59);j>hight2-9;j--)
        {
            for(i=middle_point-20;i<middle_point+20;i++)
            {
                 bin_image[j][i]=255;
            }
        }

    }


}

uint8 left_barrier_into_state=0;
void left_barrier_judge()//左路障
{
    uint8 i=0,j=0,error=0,wide=0;

    static int disantance=0;
//    tft180_show_int                 (5,60, l_turn_point, 2);
//    tft180_show_int                 (5,80, l_turn_max, 2);
//    tft180_show_int                 (25,60, r_turn_point, 2);
//    tft180_show_int                 (25,80, r_turn_max, 2);
//    tft180_show_int                 (45,60, points_l[data_stastics_l-1][1], 2);


    if(left_barrier_into_state!=0)//显示屏显示进元素的状态
    {
        tft180_show_int                 (120,45, 7, 1);
        tft180_show_int                 (140,45, left_barrier_into_state, 1);
    }
        switch(left_barrier_into_state)
        {
        case 0:
            l_turn_judge_limit(55,5,10,10,30);
            r_turn_judge_limit(55,5,5,10,115);
//            tft180_show_int                 (50,60, l_turn_max_t, 2);
//            tft180_show_int                 (50,80, l_turn_point_t, 2);

            if(r_turn_max_t==0&&l_turn_max_t>0)
            {
            if(l_turn_point_t>20&&l_border[l_turn_point_t]<l_border[l_turn_point_t-5])//l_turn_point_t>10    !!!!
            {//tft180_show_int                 (65,60, 1, 1);

//                for(j=l_turn_point_t-6;j>func_limit_ab(l_turn_point_t-13,5,55);j--)
//                {
//                    if(func_abs(l_border[j]-l_border[l_turn_point_t-6])>3)
//                    {
//                        error=1;
//                        break;
//                    }
//                }



                if(data_stastics_r>0)//加了这个
                            {
                                for(j=55;j>func_limit_ab(r_turn_point_t-5, 10, 55);j--)
                                {
                                    if(r_border[j]<r_border[j-5])
                                    {
                                        error=1;
                                        break;
                                    }
                                }
                            }
                if(l_lost<5&&r_lost<5&&l_border[l_turn_point_t-5]<55)
                {
                    for(j=l_turn_point_t-5;j>func_limit_ab(l_turn_point_t-25,8,50);j--)
                    {
                        if(bin_image[j][l_border[l_turn_point_t-5]+5]==0)
                        {
                            //tft180_show_int                 (85,60, 4, 1);
                            error=1;
                            break;
                        }
                    }
    //
                    if(error==0)
                    {
                        gpio_high(P33_10);
                        left_barrier_into_state=1;
                    }
                }
            }

            }
       break;
       case 1:
           r_border[4]=60;
           gpio_low(P33_10);
           l_turn_judge(58,5,10);

           if(l_turn_max>0)
           {
               j=l_turn_point-7;
               for(i=l_border[l_turn_point-5]-2;i<90;i++)
               {
                   if(bin_image[j][i+1]-bin_image[j][i]==255)
                   {
                       wide=i-1;
                       break;
                   }
               }

               if(wide>0)
               {
                   for(j=(uint8)limit_a_b(l_turn_point+10,5,55);j>l_turn_point-15;j--)
                   {
                       l_border[j]=(uint8)limit_a_b(wide,3,117);
                   }
                   for(j=l_turn_point-15;j>10;j--)
                   {
                       l_border[j]=(uint8)limit_a_b(wide+5,3,117);
                   }

               }
               if(l_turn_point-5>50)
               {
                   left_barrier_into_state=2;
               }
           }
        break;
        case 2:
            r_border[4]=70;
            l_turn_judge_limit(55,5,8,25,10);
            if(l_turn_max_t>0)
            {
                if(l_border[l_turn_point_t-5]>l_border[l_turn_point_t])
                {
                    l_turn_max_t=0;
                }
            }
            for(j=58;j>10;j--)
            {
               l_border[j]=r_border[50]-40;
            }

           if(l_turn_max_t==0)
               left_barrier_into_state=3;
        break;
        case 3:
            r_border[4]=80;
           for(j=58;j>10;j--)
           {
               l_border[j]=r_border[50]-40;
           }
           disantance += motor_value.receive_left_speed_data-motor_value.receive_right_speed_data;
           tft180_show_int                 (100,80, disantance, 5);
           tft180_show_int                 (5,60, motor_value.receive_left_speed_data, 4);
           if(disantance>7000)
           {
               llll=1;
               left_barrier_into_state=0;
               disantance=0;
               finish=1;
               ccc=1;
           }


        }
}

uint8 right_barrier_into_state=0;
void right_barrier_judge()//右路障
{
    uint8 i=0,j=0,error=0,wide=0;

    static int disantance=0;




        switch(right_barrier_into_state)
        {
        case 0:
            l_turn_judge_limit(55,5,5,10,5);
            r_turn_judge_limit(55,5,10,10,90);


            if(data_stastics_l>0)//加了这个
            {
                for(j=55;j>func_limit_ab(l_turn_point_t-5, 10, 55);j--)
                {
                    if(l_border[j]>l_border[j-5])
                    {
                        error=1;
                        break;
                    }
                }
            }
            if(l_turn_max_t==0&&r_turn_max_t>0)
            {//tft180_show_int                 (100,45, 1, 1);
                if(r_turn_point_t>20&&r_border[r_turn_point_t-5]<r_border[r_turn_point_t])//r_turn_point_t>10  !!!
                {
                    if(l_lost<5&&r_lost<5&&r_border[r_turn_point_t-5]>65)
                    {
                        for(j=r_turn_point_t-5;j>func_limit_ab(r_turn_point_t-25,5,50);j--)
                        {
                            if(bin_image[j][r_border[r_turn_point_t-5]-5]==0)
                            {
                                tft180_show_int                 (85,60, 4, 1);
                                error=1;
                                break;
                            }
                        }
                        if(error==0)
                        {
                            gpio_high(P33_10);
                            right_barrier_into_state=1;
                        }

                    }
                }
            }
       break;
       case 1:
           l_border[4]=10;
           gpio_low(P33_10);

           r_turn_judge_limit(58,5,10,10,110);
           if(r_turn_max_t>0)
           {
               j=r_turn_point_t-7;
               for(i=r_border[r_turn_point_t-5]+2;i>30;i--)
               {
                   if(bin_image[j][i]-bin_image[j][i+1]==255)
                   {
                       wide=i+1;
                       break;
                   }
               }

               if(wide>0)
               {
                   for(j=limit_a_b(r_turn_point_t+10,5,55);j>limit_a_b(r_turn_point_t-15,5,55);j--)
                   {
                       r_border[j]=limit_a_b(wide,3,117);
                   }
                   for(j=limit_a_b(r_turn_point_t-15,5,55);j>10;j--)
                   {
                       r_border[j]=limit_a_b(wide-5,3,117);
                   }

               }
               if(r_turn_point_t-5>50)
               {
                   right_barrier_into_state=2;
               }
           }
        break;
        case 2:
            l_border[4]=20;
           r_turn_judge_limit(55,5,8,25,110);


           if(r_turn_max_t>0)
           {
               if(r_border[r_turn_point_t-5]<r_border[r_turn_point_t])
               {
                   r_turn_max_t=0;
               }
           }
           for(j=58;j>10;j--)
           {
               r_border[j]=l_border[50]+40;
           }

           if(r_turn_max_t==0)
           right_barrier_into_state=3;
        break;
        case 3:
            l_border[4]=30;
           for(j=58;j>10;j--)
           {
               r_border[j]=l_border[50]+40;
           }
           disantance += motor_value.receive_left_speed_data-motor_value.receive_right_speed_data;
           tft180_show_int                 (100,80, disantance, 5);
           if(disantance>7000)
           {
               llll=1;
               ccc=1;
               right_barrier_into_state=0;
               disantance=0;
               finish=1;
           }


        }
}

/*
函数名称：void image_process(void)
功能说明：最终处理函数
参数说明：无
函数返回：无
修改时间：2022年9月8日
备    注：
example： image_process();
 */

extern int Target_turn;

int max_turn=0;
extern int l_bridge,l_barrier;
void image_process(void)
{
uint8 Into_element=0;
uint16 i;
uint8 hightest = 0;//定义一个最高行，tip：这里的最高指的是y值的最小

/*这是离线调试用的*/
Get_image(mt9v03x_image);
turn_to_bin();
/*提取赛道边界*/
image_filter(bin_image);//滤波
image_draw_rectan(bin_image);//预处理
data_stastics_l = 0;//清零
data_stastics_r = 0;

//if(judge_flage!=0)//跳跃图像变化
//leap_image();



if (get_start_point(image_h - 2))//找到起点了，再执行八领域，没找到就一直找
{
    gpio_low(P33_10);
    //printf("正在开始八领域\n");
    search_l_r((uint16)USE_num, bin_image, &data_stastics_l, &data_stastics_r, start_point_l[0], start_point_l[1], start_point_r[0], start_point_r[1], &hightest);
    //printf("八邻域已结束\n");
    // 从爬取的边界线内提取边线 ， 这个才是最终有用的边线
    get_left(data_stastics_l);
    get_right(data_stastics_r);

    //处理函数放这里，不要放到if外面去了，不要放到if外面去了，不要放到if外面去了，重要的事说三遍
    /**********处理部分*******/

        lost_num();    //丢线数量

        if(left_circle_into_state!=0||right_circle_into_state!=0||crossroad_into_state!=0||left_bridge_into_state!=0||right_bridge_into_state!=0||rapm_flage!=0||judge_flage!=0||left_barrier_into_state!=0||right_barrier_into_state!=0)
        {
            Into_element=1;

        }
        if(Into_element==0)
        {
            r_border[3]=110;
        }
//if(Into_element==0||judge_flage!=0)
//        leap_judge();       //跳跃识别

//if(Into_element==0)
//{
//    l_turn_judge_limit(55,5,10,5,30);
//    r_turn_judge_limit(55,5,10,5,90);
//    if(l_turn_point_t>0||r_turn_point_t>0)
//    {
//        speed=400;
//    }
//    else speed=500;
//}

        if(Into_element==0||crossroad_into_state!=0)
        crossroad_judge();//十字路口判定







if(l_bridge==1)
{
        if(Into_element==0||left_circle_into_state!=0&&aaa==0)
        left_circle_judge();     //左圆环识别并处理

        if(Into_element==0||right_bridge_into_state!=0&&bbb==0)
                //right_bridge_judge();//右单边桥识别并处理
            //right_bridge_judge3();//3个右单边桥识别并处理
        right_bridge_judge3_di();//3个右单边桥
        if(Into_element==0||right_barrier_into_state!=0&&ccc==0)//&&eeee==1
        right_barrier_judge();//右路障识别并处理


}
else
{
    if(Into_element==0||right_circle_into_state!=0&&aaa==0)
    right_circle_judge();     //右圆环识别并处理



    if(Into_element==0||left_bridge_into_state!=0&&bbb==0)
            //left_bridge_judge3();//3个左单边桥识别并处理  //left_bridge_judge();//左单边桥识别并处理
    left_bridge_judge3_di();
    if(Into_element==0||left_barrier_into_state!=0&&ccc==0)//&&eeee==1
    left_barrier_judge();//左路障识别并处理

}






//        if(Into_element==0||rapm_flage!=0)
//        ramp_judge();//坡道识别并处理
       // tft180_show_int                 (5,100,rapm_flage,1);
        //tft180_show_int                 (80,100,crossroad_into_state,1);
//        if(Into_element==0&&finish==1)
//        zebra_crossing();//斑马线识别并处理



}
else
{
    //goOutside=1;
//    speed=0;
    gpio_high(P33_10);
}

    //显示图像   改成你自己的就行 等后期足够自信了，显示关掉，显示屏挺占资源的
    tft180_displayimage03x(bin_image[0], 120,60);
    //根据最终循环次数画出边界点
//    for (i = 0; i < data_stastics_l; i++)
//    {
//        tft180_draw_point(points_l[i][0]+2, points_l[i][1], uesr_GREEN);//tft180_show_int(10,90,points_l[data_stastics_l-1][0],3);tft180_show_int(50,90,points_l[data_stastics_l-1][1],3);
//    }
//    for (i = 0; i < data_stastics_r; i++)
//    {
//        tft180_draw_point(points_r[i][0]-2, points_r[i][1], uesr_RED);//tft180_show_int(125,5,points_r[data_stastics_r-1][0],3);tft180_show_int(125,35,points_r[data_stastics_r-1][1],3);
//    }
//    for (i = 58; i > 50; i--)
//    {
//        tft180_draw_point(40, i, uesr_RED);
//        tft180_draw_point(80, i, uesr_RED);
//    }

    for (i = hightest; i < image_h-1; i++)
    {
        center_line[i] = (l_border[i] + r_border[i]) >> 1;//求中线
        //求中线最好最后求，不管是补线还是做状态机，全程最好使用一组边线，中线最后求出，不能干扰最后的输出
//        tft180_draw_point(center_line[i], i, uesr_GREEN);
//        tft180_draw_point(l_border[i], i, uesr_RED);
//        tft180_draw_point(r_border[i], i, uesr_RED);
    }

//Target_turn=find_mid_line_count();
        Target_turn=func_limit(find_mid_line_count(),30);//根据中线求出目标转向
if(func_abs(Target_turn)>max_turn)
    max_turn=func_abs(Target_turn);
        tft180_show_int                 (120,5,image_thereshold,3);
        tft180_show_int                 (120,25,Threshold_add,3);
        //tft180_show_int                 (5,100,max_turn,3);
        //tft180_show_int                 (55,100,Target_turn,3);
//        tft180_show_int                 (5,100,motor_value.receive_right_speed_data,4);
//
        //tft180_show_int                 (60,80,imu660ra_gyro_x,4);
        //tft180_show_int                 (100,35,r_x,2);
        //tft180_show_int                 (100,65,pitching_angle,2);
        //tft180_show_int                 (80,80,Target_turn,3);
//        tft180_show_int                 (110,80,out_Turn,3);
        //tft180_show_int                 (5,100,dl1b_distance_mm, 4);
}

//加权求中线
uint8 hight_list[60]=
{
       1,1,1,1,1,1,1,1,1,1,
       1,1,1,1,1,1,1,1,1,1,
       6,6,6,6,6,6,6,6,6,6,
       12,12,12,12,12,20,20,20,20,20,
       12,12,12,12,12,6,6,6,6,6,
       1,1,1,1,1,1,1,1,1,1,

};

uint8 final_mid_line=mid_w;
uint8 last_mid_line=mid_w;
int find_mid_line_count(void)
{
    uint8 mid_line_value=mid_w;
    uint8 mid_line=mid_w;
    uint32 mid_line_sum=0;
    uint32 hight_list_sum=0;
    for(uint8 i=(image_h-1);i>0;i--)
    {
        mid_line_sum+=hight_list[i]*center_line[i];
        hight_list_sum+=hight_list[i];
    }
    mid_line=(uint8)(mid_line_sum/hight_list_sum);
    mid_line_value=last_mid_line*0.2+mid_line*0.8;
    last_mid_line=mid_line_value;

    return mid_line_value-60;
}
/*

这里是起点（0.0）***************——>*************x值最大
************************************************************
************************************************************
************************************************************
************************************************************
******************假如这是一副图像*************************
***********************************************************
***********************************************************
***********************************************************
***********************************************************
***********************************************************
***********************************************************
y值最大*******************************************(188.120)

*/





