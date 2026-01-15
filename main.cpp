#include <iostream>
#include <fstream>
#include <string>
#include <windows.h>
#include <conio.h>
#include <cstdlib>
#include <vector>
#include <locale>
#include <codecvt>

using namespace std;

// 配置文件路径
const string CONFIG_FILE = "upload_config.txt";
// 临时批处理文件路径
const string BATCH_FILE = "git_upload.bat";

// 配置结构体
struct Config {
    string powershellPath;
    string gitRepoUrl;
};

// 设置控制台编码（修复中文显示问题）
void setupConsoleEncoding() {
    // 设置控制台输出编码为GBK（简体中文Windows默认编码）
    SetConsoleOutputCP(936);
    SetConsoleCP(936);
}

// 显示键位提示
void showKeyInstructions() {
    system("cls");
    cout << "=================== Git 上传工具 ===================" << endl;
    cout << "按 ESC 键: 退出程序" << endl;
    cout << "按 回车键: 执行Git上传操作" << endl;
    cout << "===================================================" << endl;
    cout << "\n等待按键...";
}

// 检查配置文件是否存在
bool configExists() {
    ifstream configFile(CONFIG_FILE);
    return configFile.good();
}

// 创建并配置配置文件
void createConfigFile() {
    Config config;

    cout << "首次运行，需要配置Git上传信息" << endl;
    cout << "=======================================" << endl;

    // 获取PowerShell/Git路径
    cout << "请输入PowerShell/Git的安装目录（例如：C:\\Program Files\\Git\\bin）：" << endl;
    getline(cin, config.powershellPath);

    // 获取Git仓库URL
    cout << "请输入Git仓库上传网址：" << endl;
    getline(cin, config.gitRepoUrl);

    // 写入配置文件
    ofstream configFile(CONFIG_FILE);
    if (configFile.is_open()) {
        configFile << "powershell_path=" << config.powershellPath << endl;
        configFile << "git_repo_url=" << config.gitRepoUrl << endl;
        configFile.close();
        cout << "配置文件已创建: " << CONFIG_FILE << endl;
    }
    else {
        cerr << "错误：无法创建配置文件！" << endl;
        exit(1);
    }

    cout << "配置完成！按任意键继续..." << endl;
    _getch();
}

// 读取配置文件
Config readConfigFile() {
    Config config;
    ifstream configFile(CONFIG_FILE);
    string line;

    if (!configFile.is_open()) {
        cerr << "错误：无法读取配置文件！" << endl;
        exit(1);
    }

    while (getline(configFile, line)) {
        size_t pos = line.find('=');
        if (pos != string::npos) {
            string key = line.substr(0, pos);
            string value = line.substr(pos + 1);

            if (key == "powershell_path") {
                config.powershellPath = value;
            }
            else if (key == "git_repo_url") {
                config.gitRepoUrl = value;
            }
        }
    }

    configFile.close();
    return config;
}

// 创建批处理文件（使用英文提示，避免编码问题）
void createBatchFile(const Config& config) {
    ofstream batchFile(BATCH_FILE);
    if (!batchFile.is_open()) {
        cerr << "Error: Cannot create batch file!" << endl;
        return;
    }

    // 写入批处理命令（使用英文）
    batchFile << "@echo off" << endl;
    batchFile << "echo =======================================" << endl;
    batchFile << "echo Git Upload Tool" << endl;
    batchFile << "echo =======================================" << endl;
    batchFile << "echo." << endl;

    // 设置Git路径（如果提供了）
    if (!config.powershellPath.empty()) {
        // 将路径添加到PATH环境变量
        batchFile << "set PATH=" << config.powershellPath << ";%PATH%" << endl;
    }

    // 执行Git命令
    batchFile << "echo [1/6] Initializing Git repository..." << endl;
    batchFile << "git init" << endl;
    batchFile << "if %errorlevel% neq 0 goto error" << endl;
    batchFile << "echo." << endl;

    batchFile << "echo [2/6] Adding all files..." << endl;
    batchFile << "git add --all" << endl;
    batchFile << "if %errorlevel% neq 0 goto error" << endl;
    batchFile << "echo." << endl;

    batchFile << "echo [3/6] Committing changes..." << endl;
    batchFile << "git commit -m \"Initial commit\"" << endl;
    batchFile << "if %errorlevel% neq 0 goto error" << endl;
    batchFile << "echo." << endl;

    batchFile << "echo [4/6] Setting main branch..." << endl;
    batchFile << "git branch -M main" << endl;
    batchFile << "if %errorlevel% neq 0 goto error" << endl;
    batchFile << "echo." << endl;

    batchFile << "echo [5/6] Adding remote repository..." << endl;
    batchFile << "git remote add origin " << config.gitRepoUrl << endl;
    batchFile << "if %errorlevel% neq 0 goto error" << endl;
    batchFile << "echo." << endl;

    batchFile << "echo [6/6] Pushing to remote..." << endl;
    batchFile << "git push -u origin main --force" << endl;
    batchFile << "if %errorlevel% neq 0 goto error" << endl;
    batchFile << "echo." << endl;

    batchFile << "echo =======================================" << endl;
    batchFile << "echo Operation completed successfully!" << endl;
    batchFile << "echo =======================================" << endl;
    batchFile << "pause" << endl;
    batchFile << "exit /b 0" << endl;
    batchFile << endl;

    batchFile << ":error" << endl;
    batchFile << "echo =======================================" << endl;
    batchFile << "echo Error: Git operation failed!" << endl;
    batchFile << "echo =======================================" << endl;
    batchFile << "pause" << endl;
    batchFile << "exit /b 1" << endl;

    batchFile.close();
}

// 执行批处理文件并捕获输出
void executeGitCommands(const Config& config) {
    // 创建批处理文件
    createBatchFile(config);

    cout << "\n正在执行Git上传操作..." << endl;
    cout << "=======================================" << endl;

    // 执行批处理文件
    string command = "\"" + BATCH_FILE + "\"";
    int result = system(command.c_str());

    // 清理临时批处理文件
    remove(BATCH_FILE.c_str());

    cout << "=======================================" << endl;
    cout << "操作完成！按任意键返回主菜单..." << endl;
    _getch();
}

int main() {
    // 设置控制台编码为GBK（修复中文显示问题）
    setupConsoleEncoding();

    // 检查配置文件是否存在，不存在则创建
    if (!configExists()) {
        createConfigFile();
    }

    // 读取配置文件
    Config config = readConfigFile();

    // 主循环
    while (true) {
        showKeyInstructions();

        // 等待按键
        int key = _getch();

        // 处理按键
        if (key == 27) { // ESC键
            cout << "\n\n程序退出。" << endl;
            break;
        }
        else if (key == 13) { // 回车键
            cout << "\n\n开始执行Git上传操作..." << endl;
            executeGitCommands(config);
        }
    }

    return 0;
}