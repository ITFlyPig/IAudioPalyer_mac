#include <iostream>
#include <thread>
#include <GLUT/glut.h>

// 引用使用C编写的库需要加上 extern "C" 修饰符，否则会报错
#ifdef __cplusplus
extern "C" {
#endif

#include "libavcodec/avcodec.h"
#include "libavutil/avutil.h"
#include "libavutil/channel_layout.h"
#include "libavformat/avformat.h"

#include "SDL.h"

#ifdef __cplusplus
}
#endif

#include "src/log.hpp"
#include "src/AudioDecoder.hpp"
#include "src/AudioDecodeCallBack.hpp"
#include "ResampleCallBack.hpp"
#include "spdlog/spdlog.h"
#include "src/BlockingQueue.hpp"
#include "src/PcmPlayer.hpp"

void audio_decode();

void test_block_queue();

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

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
    glutInitWindowSize(800, 600);
    glutCreateWindow("OpenGL Texture");
    glutDisplayFunc([]() {
        glClearColor(0.5f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        glutSwapBuffers();
    });
    glutMainLoop();

//    AudioFmt dst_audio_fmt = AudioFmt(44100, av_get_channel_layout_nb_channels(
//            AV_CH_LAYOUT_STEREO), AV_SAMPLE_FMT_S16, AV_CH_LAYOUT_STEREO);
//    PcmPlayer pcmPlayer(dst_audio_fmt);
//    if (!pcmPlayer.init())
//    {
//        LOGE("PcmPlayer初始化失败!");
//        return -1;
//    }
//    pcmPlayer.start_play();
//    LOGD("PcmPlayer初始化成功");
//
//    // 休眠60s
//    std::this_thread::sleep_for(std::chrono::seconds(60));



    return 0;
}

// 测试阻塞队列
void test_block_queue() {
    BlockingQueue<int> blocking_queue(2);
    bool is_stop = false;
    // 开启一个线程放数据到blockingQueue
    std::thread produce_thread([&blocking_queue, &is_stop]() {
        for (int i = 0; i< 20; i++)
        {
            blocking_queue.put(const_cast<int &&>(i));
            LOGD("生产线程将数据 %d 放入队列中, 当前队列size：%zu \n", i, blocking_queue.size());
        }
        is_stop = true;
    });

    std::thread consume_thread([&blocking_queue, &is_stop]() {
        while (true)
        {
            auto item = blocking_queue.take();
            LOGD("消费线程从队列中取走了数据 %d, 当前队列size：%zu \n", item, blocking_queue.size());
            // 休眠1s
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }

    });

    produce_thread.join();
    consume_thread.join();
}

void audio_decode() {
    std::__1::shared_ptr<AudioFmt> dst_audio_fmt = std::__1::make_shared<AudioFmt>(44100, av_get_channel_layout_nb_channels(
            AV_CH_LAYOUT_STEREO), AV_SAMPLE_FMT_S16, AV_CH_LAYOUT_STEREO);
    LOGD("输出音频信息：\n");
    print(dst_audio_fmt.get());

    ResampleCallBack callBack(dst_audio_fmt.get());
    AudioDecoder audioDecoder("/Users/yuelinwang/Documents/C++工程/IAudioPlayer/assets/test.mp3", callBack);
    audioDecoder.startDecode();
};