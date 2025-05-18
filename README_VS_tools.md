# Visual Studio外部工具配置指南

## 配置构建两个语言版本的外部工具

为了方便在Visual Studio中直接生成简体中文和繁体中文两个版本的DLL，您可以按照以下步骤配置外部工具：

### 方法1：使用批处理文件

1. 确保项目根目录下已经有`build_vs_shortcut.bat`文件
2. 在Visual Studio中，点击菜单"工具" -> "外部工具..."
3. 在弹出的对话框中，点击"添加"按钮，填写以下信息：
   - 标题：构建中文版本DLL
   - 命令：`$(SolutionDir)build_vs_shortcut.bat`
   - 初始目录：`$(SolutionDir)`
   - 勾选"使用输出窗口"选项
4. 点击"确定"保存设置

完成上述配置后，您可以通过菜单"工具"中找到新添加的"构建中文版本DLL"选项，点击即可同时构建简体和繁体中文版本。

### 方法2：直接在Visual Studio中切换配置

您也可以直接在Visual Studio的配置管理器中切换构建配置：

1. 在Visual Studio顶部工具栏中找到配置下拉菜单（默认显示"Debug"或"Release"）
2. 选择"ReleaseSimplified"构建简体中文版本
3. 选择"ReleaseTraditional"构建繁体中文版本

### 方法3：创建批处理解决方案

您还可以创建一个批处理解决方案，同时构建两个配置：

1. 在解决方案资源管理器中右键点击解决方案名称
2. 选择"批生成..."
3. 在弹出的对话框中勾选"ReleaseSimplified"和"ReleaseTraditional"配置
4. 点击"生成"按钮

## 注意事项

- 确保已正确安装Visual Studio生成工具和C++开发环境
- 批处理文件执行时需要管理员权限（如果Visual Studio不是以管理员身份运行）
- 构建完成后，生成的DLL文件将位于各自配置的输出目录和项目根目录下的Release文件夹中 