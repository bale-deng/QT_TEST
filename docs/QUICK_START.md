# Quick Start Guide

## 🎯 部署模式选择

### 推荐：自包含部署（无需系统 Python）

**适用场景：** 目标机器可能没有 Python 环境

**步骤：**
```batch
# 1. 下载并配置 Python 运行时（仅需一次，约 2 分钟）
download_python_runtime.bat

# 2. 构建项目
build_project.bat

# 3. 发布包位于 release/ 目录，包含：
#    - WeatherApp.exe
#    - python_runtime/ (约 50MB)
#    - python/ (脚本)
#    - Qt6 DLLs
```

**优势：**
- ✅ 目标机器无需安装 Python
- ✅ 解压即用，零配置
- ✅ 避免 Python 版本冲突

---

### 备选：系统 Python 部署

**适用场景：** 目标机器已有 Python 3.x 环境

**步骤：**
```batch
# 1. 直接构建（跳过 download_python_runtime.bat）
build_project.bat

# 2. 目标机器需要：
#    - Python 3.x 已安装并在 PATH 中
#    - 运行：pip install -r python\requirements.txt
```

**优势：**
- ✅ 包体积小（约 5MB）
- ✅ 利用系统已有 Python

---

## 🚀 快速测试

### 测试 1：基本功能
```batch
# 运行应用
build/bin/WeatherApp.exe
```
1. 从下拉菜单选择城市
2. 点击 "Refresh Weather"
3. **预期：** 2-5 秒内加载天气数据

### 测试 2：验证 Python 查找逻辑
```batch
# 查看应用日志（调试模式）
# 应显示以下之一：
# "Using embedded Python runtime: E:/path/to/python_runtime/python.exe"
# "Using system Python: C:/Python311/python.exe"
```

### 测试 3：Theme 安全验证
1. 关闭应用
2. 编辑配置文件（通常在 `%APPDATA%` 或注册表）
3. 修改 `theme` 为 `malicious`
4. 重启应用
5. **预期：** 应用回退到 "dark" 主题并输出警告

### 测试 4：无 Python 环境测试（自包含模式）
```batch
# 1. 临时重命名系统 Python（模拟无 Python 环境）
ren C:\Python311 C:\Python311_backup

# 2. 运行应用
release\WeatherApp.exe

# 3. 验证功能正常

# 4. 恢复系统 Python
ren C:\Python311_backup C:\Python311
```

---

## 📋 已实现的优化

| 类别 | 问题 | 状态 |
|------|------|------|
| 🔴 环境依赖 | 硬编码 Python 路径 | ✅ 已修复 |
| 🔴 环境依赖 | 依赖系统 Python | ✅ 已修复 |
| 🟡 安全加固 | Theme 注入风险 | ✅ 已修复 |
| 🟢 资源管理 | QSettings 泄露 | ✅ 无风险 |
| 🟢 资源管理 | QFile 泄露 | ✅ 无风险 |
| 🟢 资源管理 | QProcess 泄露 | ✅ 无风险 |

---

## 🔧 技术细节

### Python 查找优先级
1. **本地运行时：** `./python_runtime/python.exe`
2. **系统 Python：** PATH 中的 `python.exe` 或 `python3.exe`
3. **回退：** 默认 `python` 命令（会失败并提示用户）

### Theme 白名单
- 允许值：`"dark"`, `"light"`
- 无效值自动回退到 `"dark"`
- 防止资源路径注入

### 文件结构（自包含模式）
```
release/
├── WeatherApp.exe
├── python_runtime/          # Python 3.11.9 embeddable
│   ├── python.exe
│   ├── python311.dll
│   ├── Lib/
│   └── Scripts/
├── python/                  # 应用脚本
│   ├── weather_service.py
│   └── requirements.txt
├── Qt6Core.dll
├── Qt6Widgets.dll
├── Qt6Gui.dll
├── libgcc_s_seh-1.dll
├── libstdc++-6.dll
├── libwinpthread-1.dll
└── DEPLOYMENT_INFO.txt
```

---

## 📚 相关文档

- **DEPLOYMENT_OPTIMIZATION.md** - 详细优化报告
- **PHASE3_PACKAGING.md** - 打包流程说明
- **RELEASE_PACKAGE_REPORT.md** - 发布包分析

---

## ❓ 常见问题

### Q: 如何更新 Python 运行时？
A: 删除 `python_runtime/` 目录，重新运行 `download_python_runtime.bat`

### Q: 如何切换部署模式？
A:
- **切换到自包含：** 运行 `download_python_runtime.bat` 后重新构建
- **切换到系统 Python：** 删除 `python_runtime/` 后重新构建

### Q: 如何验证是否使用了嵌入式 Python？
A: 查看 `DEPLOYMENT_INFO.txt` 中的 "Python Runtime" 字段

### Q: 包体积太大怎么办？
A: 使用系统 Python 模式，包体积从 ~50MB 降至 ~5MB

---

*最后更新：2026-01-04*
*版本：v1.1.0 - 环境孤岛优化版*
