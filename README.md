# GitUpload
一个简化的Git上传助手，帮助开发者快速将项目上传到Git仓库

版本：v0.0.1_beta

## 功能
1. 一键Git上传 - 自动执行从init到push的完整Git流程
2. 智能配置 - 首次运行引导配置，后续自动保存
3. 安全修复 - 自动修复Windows Git安全目录问题
4. 状态检查 - 实时验证项目目录和Git可用性

## 核心执行流程
1. 读取配置文件 → upload_config.txt
2. 修复Git安全目录 → git config --global --add safe.directory
3. 切换到项目目录 → cd /d "项目路径"
4. 执行Git命令序列：
   git init
   git add --all
   git commit -m "Initial commit"
   git branch -M main
   git remote add origin <URL>
   git push -u origin main --force
5. 清理临时文件 → 删除git_upload.bat
