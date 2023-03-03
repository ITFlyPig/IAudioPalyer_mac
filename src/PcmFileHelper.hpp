//
// Created by wangyuelin on 2022/10/20.
//

#ifndef IAUDIOPLAYER_PCMFILEHELPER_HPP
#define IAUDIOPLAYER_PCMFILEHELPER_HPP

#include <string>
#include <utility>

extern "C"
{

// pcm文件的帮助类
class PcmFileHelper
{
private:
    using string = std::string;
    string m_pcm_path; //pcm文件路径
    FILE *p_pcm_file = nullptr; //pcm文件

public:
    explicit PcmFileHelper(string mPcmPath) : m_pcm_path(std::move(mPcmPath)) {}

    ~PcmFileHelper() {
        if (p_pcm_file != nullptr)
        {
            fclose(p_pcm_file);
            p_pcm_file = nullptr;
        }
    }

    /**
     * 打开文件
     * @return
     */
    bool open() {
        if (m_pcm_path.empty())
        {
            LOGE("PcmFileHelper::open() m_pcm_path is empty");
            return false;
        }
        p_pcm_file = fopen(m_pcm_path.c_str(), "r");
        if (p_pcm_file == nullptr)
        {
            LOGE("PcmFileHelper::open() fopen failed:%s", errno);
            return false;
        }
        return true;
    }

    /**
     * 读取len字节填充到data
     * @param data
     * @param len
     * @return 读取的字节数
     */
    int fill(void *data, int len) {
        if (p_pcm_file == nullptr)
        {
            LOGE("PcmFileHelper::fill() p_pcm_file is null");
            return -1;
        }
        if (data == nullptr)
        {
            LOGE("PcmFileHelper::fill() data is null");
            return -1;
        }
        if (len <= 0)
        {
            LOGE("PcmFileHelper::fill() len is invalid");
            return -1;
        }
        int size = fread(data, 1, len, p_pcm_file);
        return size;

    }

};
}

#endif //IAUDIOPLAYER_PCMFILEHELPER_HPP
