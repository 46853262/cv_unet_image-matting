# CxImagePaint 对话框示例（VS2013）

这是一个完整的 Visual C++ / MFC **对话框** 示例工程（Win32，使用共享 MFC DLL），基于 VS2013
的项目文件，演示如何在 32bit PNG 上使用 CxImage 实现带透明度、硬度、间距、橡皮擦模式的画笔涂抹，
并在对话框内提供缩放、打开、保存和基础参数滑块。

## 目录结构
- `CxImagePaint.sln`：VS2013 解决方案文件（Format v12，Visual Studio 2013）。
- `CxImagePaint.vcxproj`：工程文件，已配置 Debug/Release（Win32）。
- `CxImagePaintDlg.*`：对话框逻辑，包含按钮、滑块与命令处理。
- `PaintCanvas.*`：自定义绘制控件，承载 CxImage 的 32bpp PNG 加载/保存和画笔绘制逻辑。
- 其余文件为预编译头及资源定义。

## 依赖与配置
1. 使用 **Visual Studio 2013** 打开解决方案，确保安装了 **MFC** 组件。
2. 将 [CxImage](https://www.codeproject.com/Articles/1300/CxImage) 源码加入解决方案，或预编译为 `CxImage.lib`。
   - 根据你的路径调整 `CxImagePaint.vcxproj` 中的 `AdditionalIncludeDirectories` 与 `AdditionalDependencies`。
3. 工程默认使用 **动态链接 MFC**，目标平台工具集为 `v120`（VS2013）。如需更新工具集，可在项目属性中修改。

## 运行
- 点击 "Open..." 选择 32bit PNG（内部会确保 32bpp 并补全 Alpha）。
- 在画布上拖动左键即可涂抹；勾选 "Eraser" 进入橡皮擦模式。
- 使用滑块调整笔刷 **Size / Opacity / Hardness / Spacing**，或点击 "Color..." 选择颜色。
- 按 "Zoom +" / "Zoom -" 缩放预览，"Save" 会将结果保存回原图（首次保存会弹出另存为）。
