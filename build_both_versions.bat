@echo off
chcp 65001 > nul
echo 开始构建BH插件的简体中文和繁体中文版本...

rem 尝试几个常见的Visual Studio安装路径
set FOUND_VS=0
set VS_PATH=

rem 尝试VS2022的路径
if exist "D:\Program Files\Microsoft Visual Studio\2022\Community\Common7\IDE\devenv.exe" (
    set VS_PATH="D:\Program Files\Microsoft Visual Studio\2022\Community\Common7\IDE\devenv.exe"
    set FOUND_VS=1
    goto :found_vs
)

if exist "C:\Program Files\Microsoft Visual Studio\2022\Professional\Common7\IDE\devenv.exe" (
    set VS_PATH="C:\Program Files\Microsoft Visual Studio\2022\Professional\Common7\IDE\devenv.exe"
    set FOUND_VS=1
    goto :found_vs
)

if exist "C:\Program Files\Microsoft Visual Studio\2022\Enterprise\Common7\IDE\devenv.exe" (
    set VS_PATH="C:\Program Files\Microsoft Visual Studio\2022\Enterprise\Common7\IDE\devenv.exe"
    set FOUND_VS=1
    goto :found_vs
)

rem 尝试VS2019的路径
if exist "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\Common7\IDE\devenv.exe" (
    set VS_PATH="C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\Common7\IDE\devenv.exe"
    set FOUND_VS=1
    goto :found_vs
)

if exist "C:\Program Files (x86)\Microsoft Visual Studio\2019\Professional\Common7\IDE\devenv.exe" (
    set VS_PATH="C:\Program Files (x86)\Microsoft Visual Studio\2019\Professional\Common7\IDE\devenv.exe"
    set FOUND_VS=1
    goto :found_vs
)

if exist "C:\Program Files (x86)\Microsoft Visual Studio\2019\Enterprise\Common7\IDE\devenv.exe" (
    set VS_PATH="C:\Program Files (x86)\Microsoft Visual Studio\2019\Enterprise\Common7\IDE\devenv.exe"
    set FOUND_VS=1
    goto :found_vs
)

rem 尝试VS2017的路径
if exist "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\Common7\IDE\devenv.exe" (
    set VS_PATH="C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\Common7\IDE\devenv.exe"
    set FOUND_VS=1
    goto :found_vs
)

if exist "C:\Program Files (x86)\Microsoft Visual Studio\2017\Professional\Common7\IDE\devenv.exe" (
    set VS_PATH="C:\Program Files (x86)\Microsoft Visual Studio\2017\Professional\Common7\IDE\devenv.exe"
    set FOUND_VS=1
    goto :found_vs
)

if exist "C:\Program Files (x86)\Microsoft Visual Studio\2017\Enterprise\Common7\IDE\devenv.exe" (
    set VS_PATH="C:\Program Files (x86)\Microsoft Visual Studio\2017\Enterprise\Common7\IDE\devenv.exe"
    set FOUND_VS=1
    goto :found_vs
)

rem 如果上面的都没有找到，尝试使用where命令
for /f "tokens=*" %%i in ('where devenv 2^>nul') do (
    set VS_PATH=%%i
    set FOUND_VS=1
    goto :found_vs
)

:found_vs
if "%FOUND_VS%"=="0" (
    echo 未找到Visual Studio，请确保已安装Visual Studio
    echo 请手动设置VS_PATH变量为devenv.exe的完整路径
    pause
    exit /b 1
)

echo 找到Visual Studio: %VS_PATH%

echo 正在构建简体中文版本 (BH_SC.dll)...
cd /d %~dp0
%VS_PATH% "%~dp0BH.sln" /Build "ReleaseSimplified|Win32" /Project BH

echo 正在构建繁体中文版本 (BH_TC.dll)...
%VS_PATH% "%~dp0BH.sln" /Build "ReleaseTraditional|Win32" /Project BH

set OUTPUT_DIR=%~dp0Release
if not exist "%OUTPUT_DIR%" mkdir "%OUTPUT_DIR%"

if exist "%~dp0BH\ReleaseSimplified\BH_SC.dll" (
    echo 简体中文版本构建成功
    copy /Y "%~dp0BH\ReleaseSimplified\BH_SC.dll" "%OUTPUT_DIR%\BH_SC.dll"
) else (
    echo 简体中文版本构建失败
)

if exist "%~dp0BH\ReleaseTraditional\BH_TC.dll" (
    echo 繁体中文版本构建成功
    copy /Y "%~dp0BH\ReleaseTraditional\BH_TC.dll" "%OUTPUT_DIR%\BH_TC.dll"
) else (
    echo 繁体中文版本构建失败
)

echo 构建完成
echo 输出目录: %OUTPUT_DIR%
pause 