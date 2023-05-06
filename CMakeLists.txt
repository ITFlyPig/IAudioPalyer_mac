cmake_minimum_required(VERSION 3.19)
project(IAudioPlayer)

set(CMAKE_CXX_STANDARD 11)
set(SOURCE_DIR ${CMAKE_SOURCE_DIR})
# 获取项目的根目录
set(PROJECT_DIR ${CMAKE_SOURCE_DIR})

#引入conan
include(${CMAKE_SOURCE_DIR}/libs/conanbuildinfo.cmake)
conan_basic_setup()

# 源文件
file(
        GLOB SOURCE_FILES
        ${CMAKE_SOURCE_DIR}/*.hpp
        ${CMAKE_SOURCE_DIR}/src/*.hpp
        ${PROJECT_DIR}/local_libs/glad/src/glad.c # glad源文件
)


#添加头文件搜索路径
include_directories(
        /usr/local/Cellar/ffmpeg/5.1.2_6/include
        /usr/local/Cellar/glfw/3.3.8/include
        ${PROJECT_DIR}/local_libs/glad/include # glad头文件
)
#添加库文件搜索路径
link_directories(/usr/local/Cellar/ffmpeg/5.1.2_6/lib
        /usr/local/Cellar/glfw/3.3.8/lib
        )

add_executable(IAudioPlayer main.cpp ${SOURCE_FILES} src/AudioDecoder.hpp ResampleCallBack.hpp src/BlockingQueue.hpp src/PcmPlayer.hpp src/PcmFileHelper.hpp OpenGlRender.hpp)

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)

#添加链接库
target_link_libraries(
        IAudioPlayer
        libavformat.a
        libavfilter.a
        libavcodec.a
        libpostproc.a
        libavutil.a
        libswresample.a
        libswscale.a
        libpostproc.a
        glfw
)

conan_target_link_libraries(${PROJECT_NAME} ${CONAN_LIBS})