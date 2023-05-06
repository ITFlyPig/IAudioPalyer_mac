//
// Created by wangyuelin on 2023/3/3.
//

#ifndef IAUDIOPLAYER_OPENGLRENDER_HPP
#define IAUDIOPLAYER_OPENGLRENDER_HPP

#include <cstdint>
#include <OpenGL/gl3.h>
#include <GLUT/glut.h>
#include <GLFW/glfw3.h>

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
    unsigned char *generateRGBAImage(int width, int height) {
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
    bool init(int width, int height) {
        //初始化OpenGL
        if (!glfwInit())
        {
            return false;
        }
        //创建窗口
        GLFWwindow *window = glfwCreateWindow(width, height, "OpenGLRender", nullptr, nullptr);
        if (!window)
        {
            glfwTerminate();
            return false;
        }
        //设置为当前线程的上下文
        glfwMakeContextCurrent(window);
        //设置视口
        glViewport(0, 0, width, height);
        //设置顶点坐标
        GLfloat vertices[] = {
                -1.0f, -1.0f,
                1.0f, -1.0f,
                -1.0f, 1.0f,
                1.0f, 1.0f,
        };
        //设置纹理坐标
        GLfloat textureCoords[] = {
                0.0f, 0.0f,
                1.0f, 0.0f,
                0.0f, 1.0f,
                1.0f, 1.0f,
        };
        //设置顶点缓冲
        GLuint vbo;
        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        //设置纹理缓冲
        GLuint textureVbo;
        glGenBuffers(1, &textureVbo);
        glBindBuffer(GL_ARRAY_BUFFER, textureVbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(textureCoords), textureCoords, GL_STATIC_DRAW);
        //设置顶点着色器
        const char *vertexShaderSource = "#version 330 core\n"
                                         "layout (location = 0) in vec4 aPos;\n"
                                         "layout (location = 1) in vec2 aTexCoord;\n"
                                         "out vec2 TexCoord;\n"
                                         "void main()\n"
                                         "{\n"
                                         "    gl_Position = aPos;\n"
                                         "    TexCoord = aTexCoord;\n"
                                         "}\0";
        GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    }

    /**
     * 渲染
     * @param data
     * @param width
     * @param height
     */
    void render(uint8_t *data, int width, int height) {


    }

    /**
     * 释放资源
     */
    void release();

};


#endif //IAUDIOPLAYER_OPENGLRENDER_HPP
