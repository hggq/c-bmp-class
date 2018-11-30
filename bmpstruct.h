#ifndef _BMPSTRUCT_H_
#define _BMPSTRUCT_H_ 

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <string>
namespace bmp
{
typedef struct tagBMPHEADER{
    unsigned short bfType;//文件类型，必须是0x424D，即字符“BM”
    unsigned int bfSize;//文件大小
    unsigned short bfReserved1;//保留字
    unsigned short bfReserved2;//保留字
    unsigned int bfOffBits;//从文件头到实际位图数据的偏移字节数
}BMPHEADER;
typedef struct tagBMPINFOHEADER{
    unsigned int biSize;//信息头大小
    unsigned int biWidth;//图像宽度
    unsigned int biHeight;//图像高度
    unsigned short biPlanes;//位平面数，必须为1
    unsigned short biBitCount;//每像素位数
    unsigned int  biCompression; //压缩类型
    unsigned int  biSizeImage; //压缩图像大小字节数
    unsigned int  biXPelsPerMeter; //水平分辨率
    unsigned int  biYPelsPerMeter; //垂直分辨率
    unsigned int  biClrUsed; //位图实际用到的色彩数
    unsigned int  biClrImportant; //本位图中重要的色彩数
}BMPINFOHEADER;

typedef struct tagRGBAPALE{
    unsigned char Red; //该颜色的蓝色分量
    unsigned char Green; //该颜色的绿色分量
    unsigned char Blue; //该颜色的红色分量
    unsigned char Alpha; //保留值
}RGBAPALE;//调色板定义

typedef struct tagRGBDATA{
    unsigned char Red; //该颜色的蓝色分量
    unsigned char Green; //该颜色的绿色分量
    unsigned char Blue; //该颜色的红色分量
   // BYTE Alpha; //保留值
}RGBA;//调色板定义

class bitbmp{

public:
    bitbmp(){
    	init();
    }
    bitbmp(std::string filename){
        file=filename;
        init();
    }
    bool read(std::string filename){
        if(!filename.empty()){
             file=filename;
        }
        if(file.empty()) return false;
        
        std::ifstream in(file, std::ios::binary | std::ios::in);
        
        if (in.bad()) {
            return false;
        }
        
        if (in.is_open() == false) {
            return false;
        }
        memset(__header,0,sizeof(char)*54);
        in.read((char *)__header,sizeof(char)*54);
        
        in.seekg(0, std::ios::beg);
       
        in.read((char *)&head.bfType,sizeof(short));
        in.read((char *)&head.bfSize,sizeof(int));
        in.read((char *)&head.bfReserved1,sizeof(short));
        in.read((char *)&head.bfReserved2,sizeof(short));
        in.read((char *)&head.bfOffBits,sizeof(int));

        in.read((char *)&info.biSize,sizeof(int));
        in.read((char *)&info.biWidth,sizeof(int));
        in.read((char *)&info.biHeight,sizeof(int));
        in.read((char *)&info.biPlanes,sizeof(short));
        in.read((char *)&info.biBitCount,sizeof(short));
        
        in.read((char *)&info.biCompression,sizeof(int));
        in.read((char *)&info.biSizeImage,sizeof(int));
        in.read((char *)&info.biXPelsPerMeter,sizeof(int));
        in.read((char *)&info.biYPelsPerMeter,sizeof(int));
        in.read((char *)&info.biClrUsed,sizeof(int));
        in.read((char *)&info.biClrImportant,sizeof(int));
        
        
        assert(info.biSize =40);
        
        assert(info.biSizeImage < 102400000);
        assert(info.biWidth < 10240);
        assert(info.biHeight < 10240);
        width=info.biWidth;
        height=info.biHeight;
        
        unsigned int ColorTableSize = 0;
        
        if (info.biBitCount == 1) {
            ColorTableSize = 2;
        } else if (info.biBitCount== 4) {
            ColorTableSize = 16;
        } else if (info.biBitCount == 8) {
            ColorTableSize = 256;
        }
        
        RGBAPALE* ColorTable = new RGBAPALE[ColorTableSize];
        
        in.read((char *)ColorTable, sizeof(RGBAPALE)*info.biClrUsed);
        size = (int)info.biWidth*(int)info.biHeight;
        __data = new RGBA[size];
        
        unsigned int LineWidth = ((info.biWidth * info.biBitCount / 8) + 3) & ~3;
        uint8_t *Line = new uint8_t[LineWidth];
        
        in.seekg(head.bfOffBits, std::ios::beg);
        int Index = 0;
        
        if (info.biCompression == 0) {
            for (unsigned int i = 0; i < info.biHeight; i++) {
                in.read((char*) Line, LineWidth);
                uint8_t *LinePtr = Line;
                
                for (unsigned int j = 0; j < info.biWidth; j++) {
                    if (info.biBitCount == 1) {
                        uint32_t Color = *((uint8_t*) LinePtr);
                        for (int k = 0; k < 8; k++) {
                            __data[Index].Red = ColorTable[Color & 0x80 ? 1 : 0].Red;
                            __data[Index].Green = ColorTable[Color & 0x80 ? 1 : 0].Green;
                            __data[Index].Blue = ColorTable[Color & 0x80 ? 1 : 0].Blue;
                           // __data[Index].Alpha = ColorTable[Color & 0x80 ? 1 : 0].Alpha;
                            Index++;
                            Color <<= 1;
                        }
                        LinePtr++;
                        j += 7;
                    } else if (info.biBitCount == 4) {
                        uint32_t Color = *((uint8_t*) LinePtr);
                        __data[Index].Blue = ColorTable[(Color >> 4) & 0x0f].Red;
                        __data[Index].Green = ColorTable[(Color >> 4) & 0x0f].Green;
                        __data[Index].Red = ColorTable[(Color >> 4) & 0x0f].Blue;
                        //__data[Index].Alpha = ColorTable[(Color >> 4) & 0x0f].Alpha;
                        Index++;
                        __data[Index].Blue = ColorTable[Color & 0x0f].Red;
                        __data[Index].Green = ColorTable[Color & 0x0f].Green;
                        __data[Index].Red = ColorTable[Color & 0x0f].Blue;
                        //__data[Index].Alpha = ColorTable[Color & 0x0f].Alpha;
                        Index++;
                        LinePtr++;
                        j++;
                    } else if (info.biBitCount == 8) {
                        uint32_t Color = *((uint8_t*) LinePtr);
                        __data[Index].Blue = ColorTable[Color].Red;
                        __data[Index].Green = ColorTable[Color].Green;
                        __data[Index].Red = ColorTable[Color].Blue;
                        //__data[Index].Alpha = ColorTable[Color].Alpha;
                        Index++;
                        LinePtr++;
                    } else if (info.biBitCount == 16) {
                        uint32_t Color = *((uint16_t*) LinePtr);
                        __data[Index].Blue = ((Color >> 10) & 0x1f) << 3;
                        __data[Index].Green = ((Color >> 5) & 0x1f) << 3;
                        __data[Index].Red = (Color & 0x1f) << 3;
                        //__data[Index].Alpha = 255;
                        Index++;
                        LinePtr += 2;
                    } else if (info.biBitCount == 24) {
                        uint32_t Color = *((uint32_t*) LinePtr);
                        __data[Index].Blue = Color & 0xff;
                        __data[Index].Green = (Color >> 8) & 0xff;
                        __data[Index].Red = (Color >> 16) & 0xff;
                        //__data[Index].Alpha = 255;
                        Index++;
                        LinePtr += 3;
                    } else if (info.biBitCount == 32) {
                        uint32_t Color = *((uint32_t*) LinePtr);
                        __data[Index].Blue = Color & 0xff;
                        __data[Index].Green = (Color >> 8) & 0xff;
                        __data[Index].Red = (Color >> 16) & 0xff;
                        //__data[Index].Alpha = Color >> 24;
                        Index++;
                        LinePtr += 4;
                    }
                }
            }
        }
        delete[] ColorTable;
        return true;
    }
    bool save(std::string filename){
        if(!filename.empty()){
            file=filename;
        }
        if(file.empty()) return false;
       
        std::ofstream ou(file,std::ios::out | std::ios::binary);
        
        if (ou.bad()) {
            return false;
        }
        
        if (ou.is_open() == false) {
            return false;
        }

        head.bfType=0x4d42;
        head.bfOffBits=54;//固定偏移54
        head.bfReserved1=0;
        head.bfReserved2=0;
        info.biBitCount=24;
        info.biWidth=width;
        info.biHeight=height;
        info.biClrUsed=0;
        info.biClrImportant=0;
        info.biPlanes=1;
        info.biCompression=0;
        info.biXPelsPerMeter=0x15FD;
        info.biYPelsPerMeter=0x15FD;
        
        unsigned int LineWidth = ((info.biWidth * info.biBitCount / 8) + 3) & ~3;
        info.biSizeImage=LineWidth*height;
        head.bfSize=info.biSizeImage+54;
        info.biSize=40;//信息部40长度固定

        
        memcpy(__header,&head.bfType,sizeof(short));
        memcpy(__header+2,&head.bfSize,sizeof(int));
        memcpy(__header+6,&head.bfReserved1,sizeof(short));
        memcpy(__header+8,&head.bfReserved2,sizeof(short));
        memcpy(__header+10,&head.bfOffBits,sizeof(int));
 
        
        memcpy(__header+14,&info.biSize,sizeof(int));
        memcpy(__header+18,&info.biWidth,sizeof(int));
        memcpy(__header+22,&info.biHeight,sizeof(int));
        memcpy(__header+26,&info.biPlanes,sizeof(short));
        memcpy(__header+28,&info.biBitCount,sizeof(short));
        
        memcpy(__header+30,&info.biCompression,sizeof(int));
        memcpy(__header+34,&info.biSizeImage,sizeof(int));
        memcpy(__header+38,&info.biXPelsPerMeter,sizeof(int));
        memcpy(__header+42,&info.biYPelsPerMeter,sizeof(int));
        memcpy(__header+46,&info.biClrUsed,sizeof(int));
        memcpy(__header+50,&info.biClrImportant,sizeof(int));
 
        ou.write((char *)__header,sizeof(char)*54);
        
        uint8_t *Line = new uint8_t[LineWidth];
        
        uint8_t *ptr = Line;
        RGBA *lineptr = __data;
        int w=0;
        for(int j=0;j<info.biHeight;j++){

            ptr = Line;
            //逐行写入
            for(int k=0;k<info.biWidth;k++){
                *(ptr++)=lineptr->Blue;
                *(ptr++)=lineptr->Green;
                *(ptr++)=lineptr->Red;
                lineptr++;
                w++;
            }
            //补足一行
            for(int m=info.biWidth;m<LineWidth;m++){
                *(ptr++)=0x00;
            }
           
            ou.write((char *)Line,LineWidth);

        }
        ou.close();
        delete[] Line;
        return true;
    }
    bitbmp* setWidth(int w){
        width=w;
        return this;
    }
    RGBA* getData(){
        
        return __data;
    }
    bitbmp* fillColor(RGBA *data,int x=0,int y=0,int w=0,int h=0){
        
        if(size<1) return this;
        if(x==0&&y==0&&w==0&&h==0){
            for(int i=0;i<size;i++){
                __data[i].Red= data->Red;
                __data[i].Green= data->Green;
                __data[i].Blue= data->Blue;
            }
           return this;
        }
        //处理数据范围
        if(x<0) x=0;
        if(y<0) y=0;
        
        int hline=y+h;
        if(hline>height)
            hline=height;
        int wline=x+w;
        if(wline>width)
            wline=width;
        
        for(int i=y;i<hline;i++){
            for(int j=x;j<wline;j++){
                int temp=width*i+j;
                __data[temp].Red= data->Red;
                __data[temp].Green= data->Green;
                __data[temp].Blue= data->Blue;
            }
        }
        
        return this;
    }
    bitbmp* setData(unsigned int index, RGBA *data){
        if(index>size) return this;
        __data[index].Red= data->Red;
        __data[index].Green= data->Green;
        __data[index].Blue= data->Blue;
        return this;
    }
    bitbmp* setPixel(RGBA *data,int x,int y){
        
        int index=width*y+x;
        if(index>size) return this;
        __data[index].Red= data->Red;
        __data[index].Green= data->Green;
        __data[index].Blue= data->Blue;
        return this;
    }
    /*
    *创建数据空间
    */
    bitbmp* malloc(){
        
        if(size>0){
            delete[] __data;
        }
        
        size=width*height;
        __data = new RGBA[size];
        return this;
    }
    bitbmp* setHeight(int h){
        height=h;
        return this;
    }
    ~bitbmp(){
        std::free(__header);
        if(size>0){
          delete[] __data;
        }
        
    }
    /*
    *初始化变量
    */
    bitbmp* init(){
        height=0;
        width=0;
        bits=24;
        size=0;
        __header =  (unsigned char*) std::malloc(sizeof(char)*54);
        return this;
    }
    unsigned int width,height;
    unsigned char bits;
private:
	//文件头部
    unsigned char* __header;
    //颜色数据
    RGBA *__data;
    unsigned char* __palette;
    //文件名
    std::string file;
    unsigned int size;
    //表头信息
    BMPHEADER head;
    BMPINFOHEADER info;
    
};
	
}
#endif 