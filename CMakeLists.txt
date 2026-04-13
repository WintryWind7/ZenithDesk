cmake_minimum_required(VERSION 3.17)
project(ZenithDesk VERSION 0.1.0 LANGUAGES CXX)

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 包含头文件目录
include_directories(include)

# 源文件
set(SOURCES
    src/main.cpp
)

# 创建可执行文件
add_executable(zenith_desk ${SOURCES})

# 链接 Windows 库
target_link_libraries(zenith_desk
    user32      # 窗口管理
    gdi32       # 图形设备接口
    dwmapi      # Desktop Window Manager
)

# Windows 子系统设置（GUI 程序，不显示控制台）
if(WIN32)
    set_target_properties(zenith_desk PROPERTIES
        WIN32_EXECUTABLE TRUE
    )
endif()
