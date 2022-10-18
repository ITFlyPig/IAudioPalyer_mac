//
// Created by wangyuelin on 2022/10/18.
//

#ifndef IAUDIOPLAYER_AUDIODECODECALLBACK_HPP
#define IAUDIOPLAYER_AUDIODECODECALLBACK_HPP

#include "../AudioFmt.hpp"

// 音频解码的回调
class AudioDecodeCallBack
{
public:
    /**
     * 音频解码回调
     * @param p_fmt 音频格式
     * @param data  音频数据
     */
    virtual void on_decode_frame(AudioFmt *p_fmt, uint8_t **data, int nb_samples) = 0;
};


#endif //IAUDIOPLAYER_AUDIODECODECALLBACK_HPP
