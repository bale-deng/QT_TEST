# WeatherApp (Qt6 + Python)

A professional desktop weather application built with Qt6 (C++) and a Python backend. It features a Markdown editor with live preview, an emoji picker, and a self-contained Python runtime.

一个专业的桌面天气应用，采用 Qt6 (C++) 与 Python 后端构建。集成了带实时预览的 Markdown 编辑器、表情选择器以及自包含的 Python 运行时。

---

## ✨ Features / 功能

- **Weather Dashboard**: Real-time weather data with automatic refresh and persistence.
- **Markdown Editor**: Full-featured editor with live preview and formatting toolbar.
- **Emoji Picker**: Integrated picker with 800+ emojis (Ctrl+E).
- **Embedded Python**: Self-contained runtime for zero-dependency deployment.
- **Smart Restart**: Automatic Python process recovery with exponential backoff.
- **Theme Support**: Persistent Dark and Light modes with secure whitelist validation.

- **天气仪表盘**：实时天气数据，支持自动刷新与持久化。
- **Markdown 编辑器**：全功能编辑器，支持实时预览与格式化工具栏。
- **表情选择器**：内置 800+ 表情符号（快捷键 Ctrl+E）。
- **嵌入式 Python**：自包含运行时，实现零依赖部署。
- **智能重启**：Python 进程自动恢复机制，支持指数退避。
- **主题支持**：持久化的深色/浅色模式，具备安全白名单验证。

---

## 🚀 Getting Started / 快速入门

### Development / 开发环境
- Windows 10/11
- Qt 6.5+ (MinGW)
- CMake 3.16+
- Python 3.x

### Build & Deploy / 构建与部署
To create a self-contained release package:
```batch
# 1. Download embedded Python (one-time)
download_python_runtime.bat

# 2. Build and package
build_project.bat
```
The output will be in the `release/` directory.

---

## 📦 Deployment Modes / 部署模式

| Mode | Pros | Cons |
| :--- | :--- | :--- |
| **Embedded (Recommended)** | Zero dependency, stable | Larger package (~80MB) |
| **System Python** | Small package (~5MB) | Requires system Python & pip install |

---

## 🛠 Configuration / 配置
Settings are stored in `config.ini`:
```ini
[General]
theme=dark                    # "dark" or "light"
defaultCity=Beijing           # Default weather city
pythonRestartAttempts=3       # Max retry attempts
```

---

## 🔒 Security & Robustness / 安全与稳定性
- **Theme Whitelisting**: Prevents path injection by validating theme names.
- **Process Isolation**: Python backend runs in a separate process with automatic monitoring.
- **Error Recovery**: Automatic detection and restart of backend services.

---

## 📂 Project Structure / 工程结构

<!-- PROJECT_TREE_START -->
```text
.
|-- docs/                 # Documentation & Phase reports
|-- include/              # C++ Headers
|-- src/                  # C++ Source code
|-- python/               # Python backend scripts
|-- python_runtime/       # Embedded Python environment (optional)
|-- resources/            # QSS styles & assets
|-- scripts/              # Helper scripts
|-- release/              # Compiled output & distribution package
`-- CMakeLists.txt        # Build configuration
```
<!-- PROJECT_TREE_END -->

---

## 📚 Documentation / 更多文档
- [Detailed Implementation Logs](docs/PHASE1_IMPLEMENTATION.md)
- [Packaging & Distribution Guide](docs/PHASE3_IMPLEMENTATION.md)
- [Project Schema](docs/PROJECT_INIT_SCHEMA.md)

---
*Last Updated: 2026-01-04*