@echo off
echo =======================================
echo Git Upload Tool
echo =======================================
echo.
set PATH=C:\Users\XiGeminorum\source\Java\Project\Geminorum's PlayerLog;%PATH%
echo [1/6] Initializing Git repository...
git init
if %errorlevel% neq 0 goto error
echo.
echo [2/6] Adding all files...
git add --all
if %errorlevel% neq 0 goto error
echo.
echo [3/6] Committing changes...
git commit -m "Initial commit"
if %errorlevel% neq 0 goto error
echo.
echo [4/6] Setting main branch...
git branch -M main
if %errorlevel% neq 0 goto error
echo.
echo [5/6] Adding remote repository...
git remote add origin https://github.com/MINEFLFQ/Geminorum-s-PlayerLog.git
if %errorlevel% neq 0 goto error
echo.
echo [6/6] Pushing to remote...
git push -u origin main --force
if %errorlevel% neq 0 goto error
echo.
echo =======================================
echo Operation completed successfully!
echo =======================================
pause
exit /b 0

:error
echo =======================================
echo Error: Git operation failed!
echo =======================================
pause
exit /b 1
