//
// Created by wangyuelin on 2022/10/20.
//

#ifndef IAUDIOPLAYER_PCMPLAYER_HPP
#define IAUDIOPLAYER_PCMPLAYER_HPP

#include "../AudioFmt.hpp"
#include "log.hpp"
#include "PcmFileHelper.hpp"


extern "C"
{
#include "SDL.h"
}

// 播放pcm
class PcmPlayer
{
private:
    AudioFmt &fmt; // 音频的格式
public:

    explicit PcmPlayer(AudioFmt &fmt) : fmt(fmt) {}

    virtual ~PcmPlayer() {
        // 退出SDL
        SDL_Quit();
    }

    /**
     * 初始化
     * @return
     */
    bool init() {

        if (SDL_WasInit(SDL_INIT_AUDIO) != 0)
        {
            SDL_QuitSubSystem(SDL_INIT_AUDIO);
        }

        if (SDL_Init(SDL_INIT_AUDIO) != 0)
        {
            LOGE("SDL_Init not init: %s\n", SDL_GetError());
            return false;
        }

        SDL_AudioSpec playSpec;
        playSpec.freq = fmt.get_sample_rate(); //采用率
        playSpec.format = sdl_sample_fmt(fmt.get_sample_fmt());//采样格式
        playSpec.channels = fmt.get_channels();
        playSpec.silence = 0;     //静音的数值
        playSpec.userdata = nullptr;
        playSpec.samples = 2048;// 样本数
        //设置音频软件回调
        playSpec.callback = fill_audio;//sound_callback;

        if (SDL_OpenAudio(&playSpec, nullptr) < 0)
        {
            LOGE("Unable to open SDL_OpenAudio: %s \n", SDL_GetError());
            return false;
        }
        LOGD("PcmPlayer init sucess ! \n");
        return true;
    }

    /**
     * 开始播放
     */
    void start_play() {
        SDL_PauseAudio(0);
    }

private:
    /**
     *  将ffmpeg的采样格式转为sdl的采样格式
     * @param sample_fmt
     * @return
     */
    Uint16 sdl_sample_fmt(AVSampleFormat sample_fmt) {
        switch (sample_fmt)
        {
            case AV_SAMPLE_FMT_U8:
            {
                return AUDIO_U8;
            }
            case AV_SAMPLE_FMT_S16:
            {
                return AUDIO_S16SYS;
            }
            case AV_SAMPLE_FMT_S32:
            {
                return AUDIO_S32SYS;
            }
            default:
            {
                return AUDIO_S16SYS;
            }
        }
    }

    /**
     * 填充播放的数据
     * @param udata
     * @param stream
     * @param len
     */
    static void fill_audio(void *udata, Uint8 *stream, int len) {
        static PcmFileHelper *pcm_file_helper;
        if (pcm_file_helper == nullptr)
        {
            pcm_file_helper = new PcmFileHelper("/Users/yuelinwang/Documents/C++工程/IAudioPlayer/assets/target.pcm");
            if (!pcm_file_helper->open())
            {

                LOGE("PcmFileHelper::open() failed");
                if (pcm_file_helper != nullptr)
                {
                    delete pcm_file_helper;
                    pcm_file_helper = nullptr;
                }
                return;
            }
        }
        SDL_memset(stream, 0, len);
        int ret = pcm_file_helper->fill(stream, len);
        LOGD("填充数据的结果：%d \n", ret);
    }

};






#endif //IAUDIOPLAYER_PCMPLAYER_HPP
