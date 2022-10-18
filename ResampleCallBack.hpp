//
// Created by wangyuelin on 2022/10/18.
//

#ifndef IAUDIOPLAYER_RESAMPLECALLBACK_HPP
#define IAUDIOPLAYER_RESAMPLECALLBACK_HPP

#include "src/AudioDecodeCallBack.hpp"
#include "AudioFmt.hpp"
#include "src/log.hpp"
#include "Resampler.hpp"

class ResampleCallBack : public AudioDecodeCallBack {
private:
    // 重采样器
    Resampler *m_resampler = nullptr;
    // 目标音频格式
    AudioFmt *m_dst_fmt = nullptr;
    // 目标文件
    FILE *fp = nullptr;
public:

    ResampleCallBack( AudioFmt *mDstFmt) :m_dst_fmt(mDstFmt) {
        fp = fopen("target.pcm", "wb");
    }

    ~ResampleCallBack() {
        if (fp != nullptr) {
            fclose(fp);
        }
    }

    void on_decode_frame(AudioFmt *p_fmt, uint8_t **data, int nb_samples) override{
        if (m_dst_fmt == nullptr) {
            LOGE("ResampleCallBack::on_decode_frame m_dst_fmt is null");
            return;
        }
        if (p_fmt == nullptr) {
            LOGE("ResampleCallBack::on_decode_frame p_fmt is null");
            return;
        }
        if (data == nullptr) {
            LOGE("ResampleCallBack::on_decode_frame data is null");
            return;
        }
        if (nb_samples < 0) {
            LOGE("ResampleCallBack::on_decode_frame nb_samples is invalid");
            return;
        }
        if (m_resampler == nullptr){
            m_resampler = new Resampler(p_fmt, m_dst_fmt);
            if (!m_resampler->prepare()){
                LOGE("ResampleCallBack::on_decode_frame prepare failed");
                return;
            }
        }
        // 重采样
        uint8_t **dst_data = nullptr;
        // data 转为const
        const auto **src_data = (const uint8_t **) data;
        int size = m_resampler->resample(src_data, nb_samples, &dst_data);
        if (size < 0){
            LOGE("ResampleCallBack::on_decode_frame resample failed");
            return;
        }

        // 将重采样后的数据写到文件中
        // 创建target.pcm文件
        if (fp == nullptr) {
            LOGE("ResampleCallBack::on_decode_frame open target.pcm failed");
            return;
        }
        // 写入文件
        int count = fwrite(dst_data[0], 1, size, fp);
        if (count < 0) {
            LOGE("ResampleCallBack::on_decode_frame fwrite failed");
            return;
        }
    };

};
#endif //IAUDIOPLAYER_RESAMPLECALLBACK_HPP
