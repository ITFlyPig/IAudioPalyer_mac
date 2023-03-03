//
// Created by wangyuelin on 2023/3/3.
//

#ifndef IAUDIOPLAYER_OPENGLRENDER_HPP
#define IAUDIOPLAYER_OPENGLRENDER_HPP

#include <cstdint>
#include <OpenGL/gl3.h>
#include <GLUT/glut.h>

// 使用OpenGL进行渲染的渲染器
class OpenGLRender
{
private:
    /**
     * 生成一张红色的RGBA图片
     * @param width
     * @param height
     * @return
     */
    unsigned char * generateRGBAImage(int width, int height)
    {
        auto *rgba = new unsigned char[width * height * 4];
        for (int i = 0; i < width * height; i++)
        {
            rgba[i * 4] = 255;
            rgba[i * 4 + 1] = 0;
            rgba[i * 4 + 2] = 0;
            rgba[i * 4 + 3] = 255;
        }
        return rgba;
    }
public:
    OpenGLRender() = default;

    ~OpenGLRender() = default;

    /**
     * 初始化
     * @param width
     * @param height
     * @return
     */
    bool init(int width, int height);

    /**
     * 渲染
     * @param data
     * @param width
     * @param height
     */
    void render(uint8_t *data, int width, int height)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    }

    /**
     * 释放资源
     */
    void release();

};


#endif //IAUDIOPLAYER_OPENGLRENDER_HPP
