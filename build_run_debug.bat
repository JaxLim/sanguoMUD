@echo off
REM 可选：控制台改成 UTF-8，避免中文变火星文
REM chcp 65001 >nul

set ROOT=%~dp0
set BUILD=%ROOT%build
set CONFIG=Debug

REM 生成 build 目录并进入
if not exist "%BUILD%" mkdir "%BUILD%"
cd /d "%BUILD%"

REM 生成 VS2022 x64 工程
cmake -G "Visual Studio 17 2022" -A x64 .. || goto :err

REM 编译
cmake --build . --config %CONFIG% || goto :err

REM 部署 Qt 运行库到“实际生成的 exe”所在目录
set EXE=%BUILD%\ui_qt\%CONFIG%\ui_qt.exe
if not exist "%EXE%" (
  echo [ERROR] 没找到exe: %EXE%
  goto :err
)

"D:\QT\6.9.1\msvc2022_64\bin\windeployqt.exe" "%EXE%" || goto :err

REM 运行
"%EXE%"
goto :eof

:err
echo [ERROR] 构建或部署失败。
pause
