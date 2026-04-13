# Fences 技术实现分析

## 核心技术架构

### 1. 分层窗口（Layered Window）

Fences 使用 Windows 的分层窗口技术实现透明/半透明的栅栏框：

- **窗口样式**：`WS_EX_LAYERED` 扩展样式
- **透明控制**：`SetLayeredWindowAttributes(hwnd, colorKey, alpha, LWA_ALPHA | LWA_COLORKEY)`
  - `LWA_ALPHA`：控制整体透明度（0-255）
  - `LWA_COLORKEY`：指定某个颜色完全透明
- **DWM 集成**：`DwmExtendFrameIntoClientArea` 实现与桌面窗口管理器的深度集成

### 2. 鼠标事件处理

实现"无感"交互的关键：

- **透明区域穿透**：`WS_EX_TRANSPARENT` 样式让透明区域的鼠标事件穿透到下层
- **选择性响应**：
  - 栅栏边框和标题栏区域：响应鼠标事件（拖动、调整大小）
  - 栅栏内部透明区域：穿透到桌面图标
- **双窗口方案**（可能）：
  - 一个窗口负责绘制边框
  - 另一个窗口负责拦截鼠标点击，防止误选桌面图标

### 3. 桌面图标管理

Fences 通过 Shell API 操作桌面图标：

- **枚举图标**：
  - 使用 `IShellWindows` 获取桌面窗口
  - 通过 `IFolderView` 接口枚举桌面图标
  - 获取图标位置、名称、类型等信息

- **图标重定位**：
  - 拦截图标的默认位置
  - 将图标移动到指定的栅栏区域内
  - 保存图标布局配置

- **自动分组**：
  - 根据文件类型（文档、应用、文件夹）自动分类
  - 使用规则引擎匹配图标属性

### 4. 系统集成

- **启动注册**：注册为 Windows 启动项，开机自动运行
- **Shell 扩展**：可能注册为 Shell Extension，深度集成资源管理器
- **Hook 技术**：
  - `SetWindowsHookEx` 监听系统事件
  - 拦截桌面图标的创建、移动、删除事件
  - 实时同步栅栏内容

### 5. 渲染优化

保证"无感延迟"的性能优化：

- **硬件加速**：利用 DWM 的 GPU 合成能力
- **最小化重绘**：
  - 只重绘变化的区域
  - 使用 `InvalidateRect` 精确控制刷新范围
- **双缓冲**：避免闪烁
- **原生 GDI/GDI+**：直接使用 Windows 图形 API，无中间层开销

### 6. 关键 Windows API

```cpp
// 创建分层窗口
CreateWindowEx(
    WS_EX_LAYERED | WS_EX_TOPMOST | WS_EX_TOOLWINDOW,
    className, windowName,
    WS_POPUP,
    x, y, width, height,
    NULL, NULL, hInstance, NULL
);

// 设置透明度
SetLayeredWindowAttributes(hwnd, RGB(0,0,0), 200, LWA_ALPHA);

// DWM 扩展边框（实现 Aero 效果）
MARGINS margins = {-1, -1, -1, -1};
DwmExtendFrameIntoClientArea(hwnd, &margins);

// 枚举桌面图标（COM）
IShellWindows* pShellWindows;
CoCreateInstance(CLSID_ShellWindows, ...);
// 获取桌面 IFolderView 接口
// 调用 GetItemPosition、SetItemPosition 等方法
```

### 7. 数据持久化

- **配置文件**：保存栅栏布局、图标位置、用户设置
- **注册表**：存储启动配置、Shell 集成信息
- **实时保存**：图标移动后立即保存，避免丢失布局

## 性能特点

- **启动时间**：< 0.1 秒（原生 C++ 编译）
- **内存占用**：20-50 MB（取决于栅栏数量）
- **CPU 占用**：空闲时接近 0%
- **GPU 加速**：利用 DWM 硬件合成，无额外 GPU 负担
