@echo off
chcp 65001 > nul
echo 正在为BH插件构建简体和繁体中文版本...

REM 获取当前脚本所在目录
set CURRENT_DIR=%~dp0

REM 设置解决方案路径
set SOLUTION_PATH=%CURRENT_DIR%BH.sln

echo 解决方案路径: %SOLUTION_PATH%

REM 创建输出目录
if not exist "%CURRENT_DIR%\Release" mkdir "%CURRENT_DIR%\Release"

REM 构建简体中文版本
echo 正在构建简体中文版本...
call MSBuild %SOLUTION_PATH% /p:Configuration=ReleaseSimplified /p:Platform=Win32 /t:Rebuild
if %ERRORLEVEL% neq 0 (
    echo 简体中文版本构建失败!
    pause
    exit /b 1
)

REM 构建繁体中文版本
echo 正在构建繁体中文版本...
call MSBuild %SOLUTION_PATH% /p:Configuration=ReleaseTraditional /p:Platform=Win32 /t:Rebuild
if %ERRORLEVEL% neq 0 (
    echo 繁体中文版本构建失败!
    pause
    exit /b 1
)

REM 复制构建结果到Release目录
echo 复制DLL文件到Release目录...
copy /Y "%CURRENT_DIR%\BH\ReleaseSimplified\BH_SC.dll" "%CURRENT_DIR%\Release\"
copy /Y "%CURRENT_DIR%\BH\ReleaseTraditional\BH_TC.dll" "%CURRENT_DIR%\Release\"

echo 构建完成! 文件已保存到 %CURRENT_DIR%\Release 目录
pause 