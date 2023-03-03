//
// Created by wangyuelin on 2022/10/14.
//

#ifndef IAUDIOPLAYER_AUDIODECODER_HPP
#define IAUDIOPLAYER_AUDIODECODER_HPP

#include <utility>

#include "../AudioFmt.hpp"
#include "log.hpp"
#include "AudioDecodeCallBack.hpp"

#ifdef __cplusplus
extern "C" {
#endif

#include "libavcodec/avcodec.h"
#include "libavutil/avutil.h"
#include "libavformat/avformat.h"
#include "libavutil/channel_layout.h"

#ifdef __cplusplus
}
#endif

// AudioDecoder 音频解码器
class AudioDecoder
{
private:
    using string = std::string;

private:
    string m_audio_path; //音频文件路径
    AVFormatContext *p_fmt_ctx = nullptr; //封装格式上下文
    AudioDecodeCallBack &p_callBack;


public:
    AudioDecoder(string mAudioPath, AudioDecodeCallBack &pCallBack) : m_audio_path(std::move(
            mAudioPath)), p_callBack(pCallBack) {}

    ~AudioDecoder()
    {
        if (p_fmt_ctx != nullptr)
        {
            avformat_close_input(&p_fmt_ctx);
            avformat_free_context(p_fmt_ctx);
        }
    }
    // 开始解码
    int startDecode()
    {
        //判断m_audio_path是否为空
        if (m_audio_path.empty())
        {
            LOGE("AudioDecoder::startDecode() m_audio_path is empty");
            return -1;
        }

        p_fmt_ctx = avformat_alloc_context();
        int ret = avformat_open_input(&p_fmt_ctx, m_audio_path.c_str(), nullptr, nullptr);
        if (ret < 0)
        {
            LOGE("AudioDecoder::startDecode() avformat_open_input() failed");
            return -1;
        }
        ret = avformat_find_stream_info(p_fmt_ctx, nullptr);
        if (ret < 0)
        {
            LOGE("AudioDecoder::startDecode() avformat_find_stream_info() failed");
            return -1;
        }

        //获取音频流的索引
        int audioStreamIndex = -1;
        for (int i = 0; i < p_fmt_ctx->nb_streams; ++i) {
            auto stream = p_fmt_ctx->streams[i];
            if (stream->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
                audioStreamIndex = i;
                break;
            }
        }
        if (audioStreamIndex == -1) {
            LOGE("AudioDecoder::startDecode() 没有找到音频流");
            return -1;
        }

        // 打开音频解码器
        /**
         * 老的获取AVCodecContext方式：
         * AVCodecContext *p_codec_ctx = p_fmt_ctx->streams[audioStreamIndex]->codec;（old）
         *
         *另一个获取思路：
         * avformat_find_stream_info(p_fmt_ctx, nullptr);
         */
        AVCodecContext *p_codec_ctx = avcodec_alloc_context3(nullptr);
        ret = avcodec_parameters_to_context(p_codec_ctx, p_fmt_ctx->streams[audioStreamIndex]->codecpar);
        if (ret < 0) {
            LOGE("AudioDecoder::startDecode() avcodec_parameters_to_context() failed");
            return -1;
        }

        const AVCodec *p_codec = avcodec_find_decoder(p_codec_ctx->codec_id);
        if (p_codec == nullptr) {
            LOGE("AudioDecoder::startDecode() 没有找到音频解码器");
            return -1;
        }
        ret = avcodec_open2(p_codec_ctx, p_codec, nullptr);
        if (ret < 0) {
            LOGE("AudioDecoder::startDecode() 打开解码器错误：%s", av_err2str(ret));
            return -1;
        }

        //  设置输入音频信息
        auto m_src_audio_fmt = new AudioFmt(p_codec_ctx->sample_rate, p_codec_ctx->channels, p_codec_ctx->sample_fmt, p_codec_ctx->channel_layout);


        LOGD("输入音频信息：sample_rate=%d, channels=%d, sample_fmt=%s, channel_layout=%s, 格式=%s \n",
             p_codec_ctx->sample_rate, p_codec_ctx->channels, av_get_sample_fmt_name(p_codec_ctx->sample_fmt),
             av_get_channel_name(p_codec_ctx->channel_layout), av_sample_fmt_is_planar(p_codec_ctx->sample_fmt) ? "planar" : "packed");

        // 开始解码音频
         AVPacket packet;
        av_init_packet(&packet);
        const AVPacket *p_packet = &packet;
        while (av_read_frame(p_fmt_ctx, &packet) == 0) {

            if (packet.stream_index == audioStreamIndex) {
                // 发送
                do {
                    ret = avcodec_send_packet(p_codec_ctx, p_packet);
                    if (ret < 0 && ret != AVERROR(EAGAIN)) {
                        if (ret == AVERROR_EOF) {
                            LOGE("avcodec_send_packet failed: EOF");
                        } else if (ret == AVERROR(EINVAL)) {
                            LOGE("avcodec_send_packet failed: EINVAL");
                        } else if (ret == AVERROR(ENOMEM)) {
                            LOGE("avcodec_send_packet failed: ENOMEM");
                        } else if (ret == AVERROR_INVALIDDATA) {
                            LOGE("avcodec_send_packet failed: AVERROR_INVALIDDATA");
                        } else if (ret == AVERROR_PATCHWELCOME) {
                            LOGE("avcodec_send_packet failed: AVERROR_PATCHWELCOME");
                        } else if (ret == AVERROR_BUG) {
                            LOGE("avcodec_send_packet failed: AVERROR_BUG");
                        } else {
                            LOGE("avcodec_send_packet failed: unknown code: %d ", ret);
                        }
                        return -1;
                    }
                } while (ret == AVERROR(EAGAIN));

                // 接收
                AVFrame *p_frame = av_frame_alloc();
                do {
                    ret = avcodec_receive_frame(p_codec_ctx, p_frame);
                    if (ret < 0 && ret != AVERROR(EAGAIN)) {
                        if (ret == AVERROR_EOF) {
                            LOGE("avcodec_receive_frame failed: EOF");
                        } else if (ret == AVERROR(EINVAL)) {
                            LOGE("avcodec_receive_frame failed: EINVAL");
                        } else if (ret == AVERROR(ENOMEM)) {
                            LOGE("avcodec_receive_frame failed: ENOMEM");
                        } else if (ret == AVERROR_INPUT_CHANGED) {
                            LOGE("avcodec_receive_frame failed: AVERROR_INPUT_CHANGED");
                        } else {
                            LOGE("avcodec_receive_frame failed:%s " , av_err2str(ret));
                        }
                        return -1;
                    } else if (ret == 0) {
                        // 回调出去
                        p_callBack.on_decode_frame(m_src_audio_fmt, p_frame->data, p_frame->nb_samples);
                    } else {
                        LOGE("avcodec_receive_frame 未知行为：%d", ret);
                    }
                } while (ret == AVERROR(EAGAIN));
                // 释放p_frame
//                av_frame_free(&p_frame);
            }
            av_packet_unref(&packet);
        }

        // 释放AVCodecContext
        avcodec_free_context(&p_codec_ctx);
        delete m_src_audio_fmt;

        return 0;
    }

    // 暂停解码
    void pauseDecode();

    // 停止解码
    void stopDecode();

};


#endif //IAUDIOPLAYER_AUDIODECODER_HPP
