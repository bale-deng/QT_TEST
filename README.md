# WeatherApp (Qt6 + Python) - 天气笔记应用

一个专业的桌面天气笔记应用，采用 Qt6 (C++) 与 Python 后端构建。集成了带实时预览的 Markdown 编辑器、表情选择器以及自包含的 Python 运行时。

---

## ✨ 核心功能

- **天气仪表盘**：实时天气数据，支持自动刷新与持久化，侧边栏快速查看。
- **Markdown 编辑器**：全功能编辑器，支持实时预览、格式化工具栏、图片粘贴自动保存。
- **界面缩放**：支持通过 Ctrl+滚轮或快捷键 (Ctrl +/-/0) 进行平滑缩放。
- **文件管理**：完整的文件操作（新建、打开、保存、另存为），支持编码检测与原子化写入。
- **表情选择器**：内置 800+ 表情符号（快捷键 Ctrl+E）。
- **嵌入式 Python**：自包含运行时，实现零依赖部署，支持自动发现系统环境。
- **智能重启**：Python 后端进程自动监控与恢复机制。
- **主题支持**：持久化的深色/浅色模式切换。

---

## 🚀 快速入门

### 开发环境要求
- Windows 10/11
- Qt 6.5+ (MinGW)
- CMake 3.16+
- Python 3.x

### 构建与部署
要创建一个自包含的发布包：
```batch
# 1. 下载嵌入式 Python (仅需运行一次)
download_python_runtime.bat

# 2. 构建并打包项目
build_project.bat
```
编译后的程序和依赖将生成在 `release/` 目录中。

---

## 📦 部署模式对比

| 模式 | 优点 | 缺点 |
| :--- | :--- | :--- |
| **嵌入式模式 (推荐)** | 零依赖、稳定性极高 | 安装包体积较大 (~80MB) |
| **系统 Python 模式** | 体积小 (~5MB) | 需要用户自行安装 Python 及依赖库 |

---

## 🛠 配置文件
设置存储在程序目录下的 `config.ini` 中：
```ini
[General]
theme=dark                    # 主题："dark" 或 "light"
defaultCity=Beijing           # 默认天气城市
pythonRestartAttempts=3       # 最大重启重试次数
zoomLevel=1.0                 # 界面缩放级别
```

---

## 📂 工程结构

```text
.
|-- docs/                 # 项目文档与阶段报告
|-- include/              # C++ 头文件
|-- src/                  # C++ 源代码
|-- python/               # Python 后端脚本
|-- python_runtime/       # 嵌入式 Python 环境 (可选)
|-- resources/            # QSS 样式表与静态资源
|-- scripts/              # 辅助工具脚本
|-- release/              # 编译产物与分发包
`-- CMakeLists.txt        # CMake 构建配置
```

---

## 📚 相关文档
- [快速开始指南](docs/QUICK_START.md)
- [部署优化方案](docs/DEPLOYMENT_OPTIMIZATION.md)
- [项目初始蓝图](docs/PROJECT_INIT_SCHEMA.md)

---
*最后更新：2026-01-04*
*版本：v1.1.00*
