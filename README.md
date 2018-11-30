# c-bmp-class
mac os c++ bmp class
网上的bmp类也可以要改下long为int类型
64位系统下long为8个字节

编译方式 g++ main.cpp -std=c++14
如果不用c++14改下color 初始赋值方式 支持c++11

192:bmp hzq$ g++ main.cpp
main.cpp:84:11: error: expected expression
    color={0xFF,0xFF,0xFF};
          ^
main.cpp:86:11: error: expected expression
    color={0xFF,0x00,0x00};
          ^
2 errors generated.
192:bmp hzq$ g++ main.cpp -std=c++14
