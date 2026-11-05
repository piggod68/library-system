@echo off
title 借书系统 - 一键启动

echo ==========================================
echo    借书系统  启动中
echo ==========================================
echo.

rem 进入项目目录（%~dp0 是本文件所在文件夹）
cd /d "%~dp0api练习"

rem ---------- 第 1 步：启动后端 ----------
echo [1/3] 启动后端 ...
start "借书系统-后端" cmd /k python app.py

echo       等待后端启动（6 秒）...
timeout /t 6 >nul

rem ---------- 第 2 步：启动 cpolar 隧道 ----------
echo [2/3] 启动 cpolar 隧道 ...
start "借书系统-隧道" cmd /k ""C:\Program Files\cpolar\cpolar.exe" http 8000"

echo       等待隧道建立（8 秒）...
timeout /t 8 >nul

rem ---------- 第 3 步：打开浏览器 ----------
echo [3/3] 打开浏览器 ...
start http://127.0.0.1:8000/

echo.
echo ==========================================
echo    已启动
echo.
echo    本机预览:  http://127.0.0.1:8000/
echo    公网地址:  看「借书系统-隧道」窗口里
echo               Forwarding 那一行的 https 地址
echo.
echo    关闭方法:  那两个新窗口里各按 Ctrl+C
echo ==========================================
echo.
pause
