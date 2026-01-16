#include <iostream>
#include <fstream>
#include <string>
#include <windows.h>
#include <conio.h>
#include <cstdlib>
#include <vector>
#include <direct.h>
#include <sys/stat.h>
#include <io.h>
#include <fcntl.h>

using namespace std;

// 配置文件路径
const wstring CONFIG_FILE = L"upload_config.txt";
// 临时批处理文件路径
const wstring BATCH_FILE = L"git_upload.bat";

// 配置结构体
struct Config {
    wstring projectPath;    // 项目目录（Git仓库根目录）
    wstring gitRepoUrl;     // 仓库上传网址
};

// 设置控制台编码（使用宽字符）
void setupConsoleEncoding() {
    // 设置控制台为UTF-16输出
    _setmode(_fileno(stdout), _O_U16TEXT);
    _setmode(_fileno(stdin), _O_U16TEXT);
}

// 获取当前程序所在目录
wstring getCurrentDirectory() {
    wchar_t buffer[1024];
    if (_wgetcwd(buffer, sizeof(buffer) / sizeof(wchar_t)) != NULL) {
        return wstring(buffer);
    }
    return L"";
}

// 检查Git是否在系统PATH中
bool isGitInPath() {
    return system("git --version > nul 2>&1") == 0;
}

// 读取配置文件
Config readConfigFile() {
    Config config;
    wifstream configFile(CONFIG_FILE);
    wstring line;

    if (!configFile.is_open()) {
        wcerr << L"错误：无法读取配置文件！" << endl;
        exit(1);
    }

    while (getline(configFile, line)) {
        size_t pos = line.find(L'=');
        if (pos != wstring::npos) {
            wstring key = line.substr(0, pos);
            wstring value = line.substr(pos + 1);

            if (key == L"project_path") {
                config.projectPath = value;
            }
            else if (key == L"git_repo_url") {
                config.gitRepoUrl = value;
            }
        }
    }

    configFile.close();
    return config;
}

// 显示当前配置
void showCurrentConfig(const Config& config) {
    wcout << L"=================== 当前配置 ===================" << endl;
    wcout << L"项目目录: " << config.projectPath << endl;
    wcout << L"仓库网址: " << config.gitRepoUrl << endl;

    // 检查Git是否可用
    wcout << L"\nGit状态: ";
    if (isGitInPath()) {
        wcout << L"✓ Git可用" << endl;
    }
    else {
        wcout << L"✗ Git不可用，请确保Git已安装并添加到PATH" << endl;
    }

    // 检查项目目录是否存在
    wcout << L"项目目录状态: ";
    struct _stat64 info;
    if (_wstat64(config.projectPath.c_str(), &info) == 0) {
        if (info.st_mode & _S_IFDIR) {
            wcout << L"✓ 目录存在" << endl;
        }
        else {
            wcout << L"✗ 不是有效目录" << endl;
        }
    }
    else {
        wcout << L"✗ 目录不存在" << endl;
    }

    wcout << L"================================================" << endl;
}

// 显示主菜单
void showMainMenu() {
    system("cls");
    wcout << L"================== Git 项目上传工具 ==================" << endl << endl;
    wcout << L"                        主菜单" << endl << endl;
    wcout << L"======================================================" << endl << endl;
    wcout << L"         [1] -------------- 执行Git上传操作" << endl << endl;
    wcout << L"         [2] ------------- 重新配置项目信息" << endl << endl;
    wcout << L"         [3] ----------------- 查看当前配置" << endl << endl;
    wcout << L"         [4] ---------- 修复Git安全目录问题" << endl << endl;
    wcout << L"         [0] --------------------- 退出程序" << endl << endl;
    wcout << L"======================================================" << endl << endl;
    wcout << L"\n请选择操作:" << endl;
}

// 检查配置文件是否存在
bool configExists() {
    wifstream configFile(CONFIG_FILE);
    return configFile.good();
}

// 创建并配置配置文件
void createConfigFile() {
    Config config;

    wcout << L"首次运行，需要配置项目上传信息" << endl;
    wcout << L"=======================================" << endl;

    // 获取当前目录作为默认项目路径
    wstring currentDir = getCurrentDirectory();
    if (!currentDir.empty()) {
        wcout << L"当前目录: " << currentDir << endl;
        wcout << L"是否使用当前目录作为项目路径？([Y]/[N]): ";
        wchar_t choice;
        wcin >> choice;
        wcin.ignore();
        if (choice == L'Y' || choice == L'y') {
            config.projectPath = currentDir;
        }
        else {
            wcout << L"请输入项目目录（Git仓库根目录）：" << endl;
            getline(wcin, config.projectPath);
        }
    }
    else {
        wcout << L"请输入项目目录（Git仓库根目录）：" << endl;
        getline(wcin, config.projectPath);
    }

    // 获取Git仓库URL
    wcout << L"请输入Git仓库上传网址（例如：https://github.com/用户名/仓库名.git）：" << endl;
    wcout << L"仓库网址: ";
    getline(wcin, config.gitRepoUrl);

    // 验证输入
    if (config.projectPath.empty() || config.gitRepoUrl.empty()) {
        wcout << L"错误：项目目录和仓库网址都不能为空！" << endl;
        wcout << L"按[任意键]重新输入..." << endl;
        _getch();
        return;
    }

    // 写入配置文件
    wofstream configFile(CONFIG_FILE);
    if (configFile.is_open()) {
        configFile << L"project_path=" << config.projectPath << endl;
        configFile << L"git_repo_url=" << config.gitRepoUrl << endl;
        configFile.close();
        wcout << L"配置文件已创建: " << CONFIG_FILE << endl;
    }
    else {
        wcerr << L"错误：无法创建配置文件！" << endl;
        exit(1);
    }

    wcout << L"配置完成！按[任意键]继续..." << endl;
    _getch();
}

// 重新配置项目信息
void reconfigureGitInfo() {
    system("cls");
    wcout << L"=================== 重新配置项目信息 ===================" << endl;

    Config config;

    // 读取当前配置
    Config currentConfig = readConfigFile();
    wcout << L"当前配置：" << endl;
    showCurrentConfig(currentConfig);
    wcout << endl;

    // 获取项目目录
    wcout << L"请输入新的项目目录（Git仓库根目录）：" << endl;
    wcout << L"（直接按[回车]保持当前值）：";
    getline(wcin, config.projectPath);

    // 如果用户没有输入，保持原值
    if (config.projectPath.empty()) {
        config.projectPath = currentConfig.projectPath;
    }

    // 获取Git仓库URL
    wcout << L"请输入新的Git仓库上传网址：" << endl;
    wcout << L"（直接按[回车]保持当前值）：";
    getline(wcin, config.gitRepoUrl);

    // 如果用户没有输入，保持原值
    if (config.gitRepoUrl.empty()) {
        config.gitRepoUrl = currentConfig.gitRepoUrl;
    }

    // 验证输入
    if (config.projectPath.empty() || config.gitRepoUrl.empty()) {
        wcout << L"错误：项目目录和仓库网址都不能为空！" << endl;
        wcout << L"按[任意键]返回主菜单..." << endl;
        _getch();
        return;
    }

    // 写入配置文件
    wofstream configFile(CONFIG_FILE);
    if (configFile.is_open()) {
        configFile << L"project_path=" << config.projectPath << endl;
        configFile << L"git_repo_url=" << config.gitRepoUrl << endl;
        configFile.close();
        wcout << L"配置文件已更新: " << CONFIG_FILE << endl;
    }
    else {
        wcerr << L"错误：无法更新配置文件！" << endl;
        wcout << L"按[任意键]返回主菜单..." << endl;
        _getch();
        return;
    }

    wcout << L"配置更新完成！按[任意键]返回主菜单..." << endl;
    _getch();
}

// 宽字符串转多字节字符串（用于system命令）
string wstringToString(const wstring& wstr) {
    int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
    string strTo(size_needed, 0);
    WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), &strTo[0], size_needed, NULL, NULL);
    return strTo;
}

// 宽字符串转ANSI字符串（用于批处理文件路径）
string wstringToAnsi(const wstring& wstr) {
    int size_needed = WideCharToMultiByte(CP_ACP, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
    string strTo(size_needed, 0);
    WideCharToMultiByte(CP_ACP, 0, &wstr[0], (int)wstr.size(), &strTo[0], size_needed, NULL, NULL);
    return strTo;
}

// 创建批处理文件（使用英文避免编码问题）
void createBatchFile(const Config& config) {
    // 转换路径为ANSI用于批处理文件
    string projectPathAnsi = wstringToAnsi(config.projectPath);
    string repoUrlAnsi = wstringToAnsi(config.gitRepoUrl);

    ofstream batchFile(wstringToAnsi(BATCH_FILE));
    if (!batchFile.is_open()) {
        wcerr << L"错误：无法创建批处理文件！" << endl;
        return;
    }

    // 写入批处理命令（使用英文避免编码问题）
    batchFile << "@echo off" << endl;
    batchFile << "chcp 65001 > nul" << endl;
    batchFile << "echo =======================================" << endl;
    batchFile << "echo Git Project Upload Tool" << endl;
    batchFile << "echo =======================================" << endl;
    batchFile << "echo." << endl;

    // 修复Git安全目录配置
    batchFile << "echo Fixing Git safe directory configuration..." << endl;
    batchFile << "git config --global --add safe.directory \"" << projectPathAnsi << "\"" << endl;
    batchFile << "echo Git safe directory added" << endl;
    batchFile << "echo." << endl;

    // 切换到项目目录
    batchFile << "echo Changing to project directory: " << projectPathAnsi << endl;
    batchFile << "cd /d \"" << projectPathAnsi << "\"" << endl;
    batchFile << "if %errorlevel% neq 0 goto error" << endl;
    batchFile << "echo Current directory: %cd%" << endl;
    batchFile << "echo." << endl;

    // 检查Git是否可用
    batchFile << "echo Checking if Git is available..." << endl;
    batchFile << "git --version" << endl;
    batchFile << "if %errorlevel% neq 0 (" << endl;
    batchFile << "    echo Error: Git is not available! Please ensure Git is installed and in PATH" << endl;
    batchFile << "    goto error" << endl;
    batchFile << ")" << endl;
    batchFile << "echo." << endl;

    // 在项目目录中再次添加安全目录
    batchFile << "echo Adding safe configuration in project directory..." << endl;
    batchFile << "git config --local --add safe.directory ." << endl;
    batchFile << "echo." << endl;

    // 执行Git命令序列
    batchFile << "echo [1/6] Initializing Git repository..." << endl;
    batchFile << "git init" << endl;
    batchFile << "echo." << endl;

    batchFile << "echo [2/6] Adding all files to staging area..." << endl;
    batchFile << "git add --all" << endl;
    batchFile << "echo." << endl;

    batchFile << "echo [3/6] Committing changes..." << endl;
    batchFile << "git commit -m \"Initial commit\"" << endl;
    batchFile << "echo." << endl;

    batchFile << "echo [4/6] Setting main branch..." << endl;
    batchFile << "git branch -M main" << endl;
    batchFile << "echo." << endl;

    batchFile << "echo [5/6] Adding remote repository..." << endl;
    batchFile << "git remote remove origin 2>nul" << endl;
    batchFile << "git remote add origin \"" << repoUrlAnsi << "\"" << endl;
    batchFile << "echo." << endl;

    batchFile << "echo [6/6] Pushing to remote repository..." << endl;
    batchFile << "git push -u origin main --force" << endl;
    batchFile << "if %errorlevel% neq 0 goto push_error" << endl;
    batchFile << "echo." << endl;

    batchFile << "echo =======================================" << endl;
    batchFile << "echo Success! Git upload operation completed!" << endl;
    batchFile << "echo =======================================" << endl;
    batchFile << "pause" << endl;
    batchFile << "exit /b 0" << endl;
    batchFile << endl;

    batchFile << ":push_error" << endl;
    batchFile << "echo Note: Push may have failed, but local Git operations are complete" << endl;
    batchFile << "echo Possible reasons: Network issues, insufficient permissions, or repo already exists" << endl;
    batchFile << "echo Local Git repository is initialized. You can manually execute: git push -u origin main" << endl;
    batchFile << "echo =======================================" << endl;
    batchFile << "pause" << endl;
    batchFile << "exit /b 1" << endl;
    batchFile << endl;

    batchFile << ":error" << endl;
    batchFile << "echo =======================================" << endl;
    batchFile << "echo Error: Operation failed!" << endl;
    batchFile << "echo Please check:" << endl;
    batchFile << "echo 1. Is the project directory correct?" << endl;
    batchFile << "echo 2. Is Git installed and in PATH?" << endl;
    batchFile << "echo 3. Is network connection working?" << endl;
    batchFile << "echo =======================================" << endl;
    batchFile << "pause" << endl;
    batchFile << "exit /b 1" << endl;

    batchFile.close();
}

// 执行Git上传操作
void executeGitCommands(const Config& config) {
    // 显示当前配置
    system("cls");
    wcout << L"=================== 准备上传 ===================" << endl;
    showCurrentConfig(config);

    // 确认上传
    wcout << L"\n确认使用以上配置执行Git上传操作吗？" << endl;
    wcout << L"按[回车]键继续，[ESC]返回主菜单..." << endl;

    int key = _getch();
    if (key == 27) { // ESC键
        return;
    }

    // 检查Git是否可用
    if (!isGitInPath()) {
        wcout << L"\n错误：Git在系统PATH中不可用！" << endl;
        wcout << L"请确保Git已安装并添加到系统PATH环境变量。" << endl;
        wcout << L"按[任意键]返回主菜单..." << endl;
        _getch();
        return;
    }

    // 检查项目目录是否存在
    struct _stat64 info;
    if (_wstat64(config.projectPath.c_str(), &info) != 0 || !(info.st_mode & _S_IFDIR)) {
        wcout << L"\n错误：项目目录不存在或无效！" << endl;
        wcout << L"请检查项目目录路径是否正确。" << endl;
        wcout << L"按[任意键]返回主菜单..." << endl;
        _getch();
        return;
    }

    // 创建批处理文件
    createBatchFile(config);

    wcout << L"\n正在执行Git上传操作..." << endl;
    wcout << L"=======================================" << endl;

    // 执行批处理文件
    string command = "\"" + wstringToAnsi(BATCH_FILE) + "\"";
    int result = system(command.c_str());

    // 清理临时批处理文件
    _wremove(BATCH_FILE.c_str());

    wcout << L"=======================================" << endl;
    wcout << L"操作完成！按[任意键]返回主菜单..." << endl;
    _getch();
}

// 修复Git安全目录问题
void fixGitSafeDirectory() {
    system("cls");
    wcout << L"=================== 修复Git安全目录问题 ===================" << endl;

    // 读取当前配置
    Config config = readConfigFile();

    wcout << L"当前项目目录: " << config.projectPath << endl;
    wcout << L"\n此功能将修复Git的安全目录权限问题。" << endl;
    wcout << L"是否继续？([Y]/[N]): ";

    wchar_t choice;
    wcin >> choice;
    wcin.ignore();

    if (choice == L'Y' || choice == L'y') {
        // 创建修复批处理
        wstring fixBat = L"fix_git_safe.bat";
        string fixBatAnsi = wstringToAnsi(fixBat);
        string projectPathAnsi = wstringToAnsi(config.projectPath);

        ofstream batchFile(fixBatAnsi);

        if (batchFile.is_open()) {
            batchFile << "@echo off" << endl;
            batchFile << "chcp 65001 > nul" << endl;
            batchFile << "echo Fixing Git safe directory issue..." << endl;
            batchFile << "echo." << endl;

            // 全局添加安全目录
            batchFile << "git config --global --add safe.directory \"" << projectPathAnsi << "\"" << endl;
            batchFile << "echo Added to global safe directories" << endl;

            // 如果目录存在，也添加到本地
            batchFile << "cd /d \"" << projectPathAnsi << "\" 2>nul" << endl;
            batchFile << "if %errorlevel% equ 0 (" << endl;
            batchFile << "    git config --local --add safe.directory ." << endl;
            batchFile << "    echo Added to local safe directories" << endl;
            batchFile << ")" << endl;

            batchFile << "echo." << endl;
            batchFile << "echo Fix completed!" << endl;
            batchFile << "pause" << endl;
            batchFile.close();

            // 执行修复
            system(("\"" + fixBatAnsi + "\"").c_str());

            // 清理临时文件
            _wremove(fixBat.c_str());
        }
    }

    wcout << L"按[任意键]返回主菜单..." << endl;
    _getch();
}

int main() {
    // 设置控制台编码为宽字符
    setupConsoleEncoding();

    // 检查配置文件是否存在，不存在则创建
    if (!configExists()) {
        createConfigFile();
    }

    // 读取配置文件
    Config config = readConfigFile();

    // 主循环
    while (true) {
        showMainMenu();

        // 等待用户输入
        wstring input;
        getline(wcin, input);

        // 处理用户选择
        if (input.empty()) continue;

        wchar_t choice = input[0];

        switch (choice) {
        case L'1': // 执行Git上传
            executeGitCommands(config);
            break;

        case L'2': // 重新配置
            reconfigureGitInfo();
            // 重新读取配置
            config = readConfigFile();
            break;

        case L'3': // 查看当前配置
            system("cls");
            wcout << L"=================== 当前配置 ===================" << endl;
            showCurrentConfig(config);
            wcout << L"\n按[任意键]返回主菜单..." << endl;
            _getch();
            break;

        case L'4': // 修复Git安全目录问题
            fixGitSafeDirectory();
            break;

        case L'0': // 退出程序
            wcout << L"\n程序退出。" << endl;
            return 0;

        default: // 无效输入
            wcout << L"\n无效选择，请重新输入！" << endl;
            wcout << L"按[任意键]继续..." << endl;
            _getch();
            break;
        }
    }

    return 0;
}