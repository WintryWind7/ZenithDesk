#ifndef ZENITH_WINDOW_H
#define ZENITH_WINDOW_H

#include <windows.h>
#include <string>

namespace zenith_desk {

// 窗口类
class Window {
public:
    Window(const std::wstring& title, int width, int height);
    ~Window();

    // 创建窗口
    bool create();
    
    // 运行消息循环
    int run();
    
    // 获取窗口句柄
    HWND getHandle() const { return hwnd_; }

private:
    // 窗口过程（静态）
    static LRESULT CALLBACK windowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
    
    // 实例窗口过程
    LRESULT handleMessage(UINT msg, WPARAM wParam, LPARAM lParam);

private:
    std::wstring title_;
    int width_;
    int height_;
    HWND hwnd_;
    HINSTANCE hInstance_;
};

} // namespace zenith_desk

#endif // ZENITH_WINDOW_H
