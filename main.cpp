#include <iostream>

// 引用使用C编写的库需要加上 extern "C" 修饰符，否则会报错
#ifdef __cplusplus
extern "C" {
#endif

#include "libavcodec/avcodec.h"
#include "libavutil/avutil.h"
#include "libavutil/channel_layout.h"
#include "libavformat/avformat.h"

#ifdef __cplusplus
}
#endif

#include "src/log.hpp"
#include "src/AudioDecoder.hpp"
#include "src/AudioDecodeCallBack.hpp"
#include "ResampleCallBack.hpp"
#include "spdlog/spdlog.h"

void print(AudioFmt *fmt) {
    if (fmt == nullptr)
    {
        LOGE("print fmt is null");
        return;
    }
    LOGD("音频信息：sample_rate=%d, channels=%d, sample_fmt=%s, channel_layout=%s, 格式=%s \n",
         fmt->get_sample_rate(), fmt->get_channels(), av_get_sample_fmt_name(fmt->get_sample_fmt()),
         av_get_channel_name(fmt->get_channel_layout()),
         av_sample_fmt_is_planar(fmt->get_sample_fmt()) ? "planar" : "packed");
}

int main() {
    // 需要的音频信息，智能指针
//    int len = 100;
//    char ch_name[len];
//    av_channel_name(ch_name, len, dst_channel_layout);
// 获取声道数

    spdlog::info("spdlog version {}.{}.{}", SPDLOG_VER_MAJOR, SPDLOG_VER_MINOR, SPDLOG_VER_PATCH);
//    std::shared_ptr<AudioFmt> dst_audio_fmt = std::make_shared<AudioFmt>(44100, av_get_channel_layout_nb_channels(
//            AV_CH_LAYOUT_STEREO), AV_SAMPLE_FMT_S16, AV_CH_LAYOUT_STEREO);
//    LOGD("输出音频信息：\n");
//    print(dst_audio_fmt.get());
//
//    ResampleCallBack callBack(dst_audio_fmt.get());
//    AudioDecoder audioDecoder("/Users/yuelinwang/Documents/C++工程/IAudioPlayer/assets/test.mp3", callBack);
//    audioDecoder.startDecode();

    return 0;
};