# CxImagePaint 示例工程

这是一个完整的 Visual C++ / MFC 示例工程（Win32，使用共享 MFC DLL），演示如何在 32bit PNG 上使用 CxImage 实现带透明度、硬度、间距和橡皮擦模式的画笔涂鸦，同时支持缩放显示和保存原图。

## 目录结构
- `CxImagePaint.sln`：解决方案文件。
- `CxImagePaint.vcxproj`：工程文件，已配置 Debug/Release（Win32）。
- `CxImagePaintView.cpp`：核心画笔逻辑与缩放绘制。
- 其余文件为标准 SDI 框架（App/Doc/View/Frame）与资源描述。

## 依赖与配置
1. 安装 Visual Studio 2019 或更新版本，勾选 **MFC for C++** 工作负载。
2. 将 [CxImage](https://www.codeproject.com/Articles/1300/CxImage) 源码加入解决方案或预编译为 `CxImage.lib`。
   - 更新 `CxImagePaint.vcxproj` 中的 `AdditionalIncludeDirectories` 及 `AdditionalDependencies` 指向实际的 CxImage 头文件和库输出目录。
3. 工程假设使用 **动态链接 MFC**（`UseOfMfc=Dynamic`）。

## 功能概览
- 打开/保存 32bit PNG（自动转为 32bpp 以便写入 alpha）。
- 画笔属性：大小、透明度、不透明边缘（硬度）、间距，支持橡皮擦模式。
- 符合画笔间距的拖动涂抹，并使用硬度计算边缘衰减。
- 缩放预览：Ctrl+数字小键盘 +/- 对应菜单快捷键。

## 运行
打开 `CxImagePaint.sln`，确保 CxImage 依赖已配置，然后编译运行。首次启动后通过菜单 **File → Open** 选择 PNG 进行绘制，完成后 **File → Save** 将结果保存回原文件。
