cmake_minimum_required(VERSION 3.19)
project(IAudioPlayer)

set(CMAKE_CXX_STANDARD 11)
set(SOURCE_DIR ${CMAKE_SOURCE_DIR})

#引入conan
include(${CMAKE_SOURCE_DIR}/libs/conanbuildinfo.cmake)
conan_basic_setup()


# 源文件
file(GLOB SOURCE_FILES
        ${CMAKE_SOURCE_DIR}/*.hpp
        ${CMAKE_SOURCE_DIR}/src/*.hpp
        )


#添加头文件搜索路径
include_directories(/usr/local/Cellar/ffmpeg/5.1.2/include)
#添加库文件搜索路径
link_directories(/usr/local/Cellar/ffmpeg/5.1.2/lib)

add_executable(IAudioPlayer main.cpp ${SOURCE_FILES} src/AudioDecoder.hpp ResampleCallBack.hpp)

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
)

conan_target_link_libraries(${PROJECT_NAME} ${CONAN_LIBS})