//
// Created by wangyuelin on 2022/10/14.
//

#ifndef IAUDIOPLAYER_RESAMPLER_H
#define IAUDIOPLAYER_RESAMPLER_H

#ifdef __cplusplus
extern "C" {
#endif

#include "libswresample/swresample.h"

#ifdef __cplusplus
}
#endif


#include "AudioFmt.hpp"
#include "src/log.hpp"

// Resampler 对音频重采样
class Resampler
{
private:
    // 借助其实现重采样
    SwrContext *swr_context = nullptr;
    // 输入音频格式
    AudioFmt *m_src_fmt;
    // 输出音频格式
    AudioFmt *m_dst_fmt;
    // 标记是否准备成功
    bool m_is_prepare = false;

public:
    Resampler(AudioFmt *src_fmt, AudioFmt *dst_fmt)
    {
        this->m_src_fmt = src_fmt;
        this->m_dst_fmt = dst_fmt;
    }

    ~Resampler()
    {
        if (swr_context != nullptr)
        {
            swr_free(&swr_context);
        }
    }

    // 采样之前的准备
    bool prepare()
    {
        if (!check_param())
        {
            LOGE("Resampler::prepare check_param failed");
            m_is_prepare = false;
            return false;
        }
        swr_context = swr_alloc_set_opts(nullptr,
                                         m_dst_fmt->get_channel_layout(),
                                         m_dst_fmt->get_sample_fmt(),
                                         m_dst_fmt->get_sample_rate(),
                                         m_src_fmt->get_channel_layout(),
                                         m_src_fmt->get_sample_fmt(),
                                         m_src_fmt->get_sample_rate(),
                                         0, nullptr);
        if (swr_context == nullptr) {
            LOGE("Resampler::prepare() swr_alloc_set_opts failed");
            m_is_prepare = false;
            return false;
        }
        int ret = swr_init(swr_context);
        if (ret < 0) {
            LOGE("Resampler::prepare() swr_init failed, error: %s", av_err2str(ret));
            m_is_prepare = false;
            return false;
        }
        m_is_prepare = true;
        return true;
    }

    // 参数检查
    bool check_param()
    {
        if (m_src_fmt == nullptr || m_dst_fmt == nullptr)
        {
            LOGE("Resampler::check_param() m_src_fmt == nullptr || m_dst_fmt == nullptr");
            return false;
        }
        return true;
    }

    // 重采样
    int resample(const uint8_t **src_data, int src_nb_samples, uint8_t ***dst_data)
    {
        // 检查是否准备好
        if (!m_is_prepare)
        {
            LOGE("Resampler::resample() m_is_prepare == false");
            return -1;
        }

        // 检查参数
        if (src_data == nullptr || src_nb_samples <= 0 )
        {
            LOGE("Resampler::resample() src_data == nullptr || src_nb_samples <= 0 ");
            return -1;
        }

        int src_sample_rate = m_src_fmt->get_sample_rate();
        int dst_sample_rate = m_dst_fmt->get_sample_rate();
        int dst_channels = m_dst_fmt->get_channels();
        AVSampleFormat dst_sample_fmt = m_dst_fmt->get_sample_fmt();

        int align = 1;
        // 据被转换的音频帧，计算转换后的音频帧的样本数
        int dst_nb_samples = av_rescale_rnd(
                swr_get_delay(swr_context, src_sample_rate) + src_nb_samples,
                dst_sample_rate, src_sample_rate, AV_ROUND_UP);

        // 申请输出内存
        int dst_linesize = 0;
        int ret = av_samples_alloc_array_and_samples(dst_data, &dst_linesize, dst_channels,
                                                     dst_nb_samples, dst_sample_fmt, align);
        if (ret < 0) {
            LOGE("Resampler::resample() 申请输出内存失败：%s", av_err2str(ret));
            return -1;
        }

        // 重采样
        int nb = swr_convert(swr_context, *dst_data, dst_nb_samples, src_data, src_nb_samples);
        LOGD("采样前样本数：%d，采样后样本数：%d \n", src_nb_samples, nb);
        if (nb < 0) {
            LOGE("Resampler::resample() 重采样失败：%s", av_err2str(nb));
            return -1;
        }

        // 重采样后计算实际的缓冲区大小
        int dst_buff_size = av_samples_get_buffer_size(&dst_linesize, dst_channels, nb,
                                                       dst_sample_fmt, align);
        LOGD("采样后缓冲区大小：%d \n", dst_buff_size);
        if (dst_buff_size < 0) {
            LOGE("Resampler::resample() 获取重采样后实际的缓冲区大小失败：%s", av_err2str(dst_buff_size));
            return -1;
        }
        return dst_buff_size;
    }
};


#endif //IAUDIOPLAYER_RESAMPLER_H
