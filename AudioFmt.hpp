//
// Created by wangyuelin on 2022/10/14.
//

#ifndef IAUDIOPLAYER_AUDIOFMT_HPP
#define IAUDIOPLAYER_AUDIOFMT_HPP

#include "libavutil/samplefmt.h"

// AudioFmt 存储音频相关信息
class AudioFmt
{
private:
    int sample_rate; //采样率
    int channels; //通道数
    AVSampleFormat sample_fmt; //采样格式
    int channel_layout; //通道布局
public:
    AudioFmt(int sample_rate, int channels, AVSampleFormat sample_fmt, int channel_layout) {
        this->sample_rate = sample_rate;
        this->channels = channels;
        this->sample_fmt = sample_fmt;
        this->channel_layout = channel_layout;
    }

    AudioFmt() = delete;

    ~AudioFmt() = default;

    int get_sample_rate() const {
        return sample_rate;
    }

    int get_channels() const {
        return channels;
    }

    AVSampleFormat get_sample_fmt() const {
        return sample_fmt;
    }

    int get_channel_layout() const {
        return channel_layout;
    }


};
#endif //IAUDIOPLAYER_AUDIOFMT_HPP
