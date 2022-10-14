#include <iostream>

// 引用使用C编写的库需要加上 extern "C" 修饰符，否则会报错
#ifdef __cplusplus
extern "C" {
#endif

#include "libavcodec/avcodec.h"
#include "libavutil/avutil.h"
#include "libavformat/avformat.h"

#ifdef __cplusplus
}
#endif

int main() {
    std::cout << "ffmpeg配置如下：" << std::endl;
    std::cout << "avcodec_configuration:" << avcodec_configuration() << std::endl;

    return 0;
}
