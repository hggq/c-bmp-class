#include <stdio.h>
#include "bmpstruct.h"
#include "stdlib.h"
#include "math.h"
#include "font.h"
#include <iostream>

using namespace std;
void showchar(unsigned short x,unsigned short y,unsigned char num,unsigned char size,bmp::bitbmp *bmp,bmp::RGBA color)
{
    unsigned char temp,t1,t;
    unsigned short y0=y;
    
   unsigned char csize=(size/8+((size%8)?1:0))*(size/2);        //得到字体一个字符对应点阵集所占的字节数
    num=num-' ';//得到偏移后的值（ASCII字库是从空格开始取模，所以-' '就是对应字符的字库）

    for(t=0;t<csize;t++)
    {
        if(size==12)temp=asc2_1206[num][t];          //调用1206字体
        else if(size==16)temp=asc2_1608[num][t];    //调用1608字体
        else if(size==24)temp=asc2_2412[num][t];    //调用2412字体
        else return;                                //没有的字库
        for(t1=0;t1<8;t1++)
        {
            if(temp&0x80) bmp->setPixel(&color,x,y);

            temp<<=1;
            y--;
            if(y<0)return;        //超区域了
            if((y0-y)==size)
            {
                y=y0;
                x++;
                if(x>=bmp->width)return;    //超区域了
                break;
            }
        }
    }
}
void showString(unsigned short x,unsigned short y,unsigned short width,unsigned char size,unsigned char *p,bmp::bitbmp *bmp,bmp::RGBA color)
{
    unsigned short x0=x;
    width=x+width;
    while((*p<='~')&&(*p>=' '))//判断是不是非法字符!
    {
        if(x>=width){x=x0;y-=size;}
        if(y>=bmp->height)break;//退出
        if(y<0)break;//退出
        showchar(x,y,*p,size,bmp,color);
        x+=size/2;
        p++;
    }
}
void numtohex(unsigned char num,unsigned char *p){
    unsigned char t=0;
    unsigned char str[]={'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
    t=num&0xF;
    p[0]='0';
    p[1]='x';
    p[3]=str[t];
    num=num>>4;
    t=num&0xF;
    p[2]=str[t];
    p[4]='\0';
}
int main()
{


    bmp::bitbmp *bmp=new bmp::bitbmp();
    //  bmp->read("./140.bmp");
    //cout<<bmp->width;
    //保存bmp图片
    //  bmp->save("./b2.bmp");
 
    bmp->setWidth(480)->setHeight(320)->malloc();
    

    // y=2x+10 E2A925
    bmp::RGBA color;
    //color.Red=0xE2;
    //color.Green=0xA9;
    //color.Blue=0x25;
    color={0xFF,0xFF,0xFF};
    bmp->fillColor(&color);
    color={0xFF,0x00,0x00};
    cout<<bmp->width;
    unsigned char str[]="!1234567890ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    //showchar(40,50,'C',24,bmp,color);
    showString(40,60,100,12,str,bmp,color);
    bmp->save("./bfont2.bmp");
    return 0;
    color.Red=0xff;
    color.Green=0x00;
    color.Blue=0x00;
    
    bmp->fillColor(&color,40,40,300,200);
    
    bmp->fillColor(&color,340,240,10,10);
    
    for(int n=20;n<280;n++){
        bmp->setPixel(&color,20,n);
        bmp->setPixel(&color,21,n);
        bmp->setPixel(&color,n,20);
        bmp->setPixel(&color,n,21);
    }
    
    for(int i=20;i<120;i++){
        int y=2*i+10;
        
        bmp->setPixel(&color,i,y);
        
    }
    
    bmp->save("./b2.bmp");
    return 0;
}
