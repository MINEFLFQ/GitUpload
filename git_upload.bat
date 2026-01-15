@echo off
chcp 65001
echo 开始执行Git上传操作...
echo.
echo 1. 初始化Git仓库...
git init
if %errorlevel% neq 0 goto error
echo.
echo 2. 添加所有文件...
git add --all
if %errorlevel% neq 0 goto error
echo.
echo 3. 提交更改...
git commit -m "Initial commit"
if %errorlevel% neq 0 goto error
echo.
echo 4. 设置主分支...
git branch -M main
if %errorlevel% neq 0 goto error
echo.
echo 5. 添加远程仓库...
git remote add origin 
if %errorlevel% neq 0 goto error
echo.
echo 6. 推送代码...
git push -u origin main --force
if %errorlevel% neq 0 goto error
echo.
echo 操作完成！
pause
exit /b 0

:error
echo 错误：Git操作失败！
pause
exit /b 1
