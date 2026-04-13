#include "window.h"
#include <dwmapi.h>

namespace zenith_desk {

Window::Window(const std::wstring& title, int width, int height)
    : title_(title), width_(width), height_(height), hwnd_(nullptr) {
    hInstance_ = GetModuleHandle(nullptr);
}

Window::~Window() {
    if (hwnd_) {
        DestroyWindow(hwnd_);
    }
}

bool Window::create() {
    // 注册窗口类
    WNDCLASSEXW wc = {};
    wc.cbSize = sizeof(WNDCLASSEXW);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = windowProc;
    wc.hInstance = hInstance_;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wc.lpszClassName = L"ZenithDeskWindow";

    if (!RegisterClassExW(&wc)) {
        return false;
    }

    // 创建普通分层窗口
    hwnd_ = CreateWindowExW(
        WS_EX_LAYERED | WS_EX_NOACTIVATE | WS_EX_TOOLWINDOW,  // 分层窗口 + 不激活 + 工具窗口（隐藏任务栏图标）
        L"ZenithDeskWindow",
        title_.c_str(),
        WS_POPUP,
        100, 100,
        width_, height_,
        nullptr, nullptr,
        hInstance_,
        this
    );

    if (!hwnd_) {
        return false;
    }

    // 设置透明度
    SetLayeredWindowAttributes(hwnd_, 0, 128, LWA_ALPHA);

    // 显示窗口
    ShowWindow(hwnd_, SW_SHOWNOACTIVATE);
    
    // 设置为桌面级别（在所有窗口下方）
    SetWindowPos(hwnd_, HWND_BOTTOM, 0, 0, 0, 0, 
                 SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);

    return true;
}

int Window::run() {
    MSG msg = {};
    while (GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return static_cast<int>(msg.wParam);
}

LRESULT CALLBACK Window::windowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    Window* window = nullptr;

    if (msg == WM_CREATE) {
        // 从 CreateWindowEx 传入的参数获取 this 指针
        CREATESTRUCT* cs = reinterpret_cast<CREATESTRUCT*>(lParam);
        window = reinterpret_cast<Window*>(cs->lpCreateParams);
        SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(window));
    } else {
        window = reinterpret_cast<Window*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
    }

    if (window) {
        return window->handleMessage(msg, wParam, lParam);
    }

    return DefWindowProc(hwnd, msg, wParam, lParam);
}

LRESULT Window::handleMessage(UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_WINDOWPOSCHANGING: {
            // 阻止窗口被最小化（Win+D 时保持可见）
            WINDOWPOS* pos = reinterpret_cast<WINDOWPOS*>(lParam);
            pos->flags &= ~SWP_HIDEWINDOW;
            return 0;
        }

        case WM_KEYDOWN:
            // 按 ESC 退出
            if (wParam == VK_ESCAPE) {
                PostQuitMessage(0);
                return 0;
            }
            break;

        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;

        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd_, &ps);
            
            // 绘制白色边框
            HPEN pen = CreatePen(PS_SOLID, 3, RGB(255, 255, 255));
            HPEN oldPen = (HPEN)SelectObject(hdc, pen);
            HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, GetStockObject(NULL_BRUSH));
            
            Rectangle(hdc, 0, 0, width_, height_);
            
            SelectObject(hdc, oldPen);
            SelectObject(hdc, oldBrush);
            DeleteObject(pen);
            
            EndPaint(hwnd_, &ps);
            return 0;
        }
    }

    return DefWindowProc(hwnd_, msg, wParam, lParam);
}

} // namespace zenith_desk

// 程序入口
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    zenith_desk::Window window(L"天顶 - 测试窗口", 800, 600);
    
    if (!window.create()) {
        MessageBoxW(nullptr, L"窗口创建失败", L"错误", MB_ICONERROR);
        return 1;
    }

    return window.run();
}
