# MPEV 在线编译配置指南

本文档说明如何使用在线编译服务构建MPEV项目。

## 方案一：GitHub Actions（推荐）

### 优势
- ✅ 完全免费（公共仓库）
- ✅ 自动多平台编译（Windows/Linux/macOS）
- ✅ 自动生成发布版本
- ✅ 支持手动触发编译

### 配置步骤

#### 1. 创建GitHub仓库
```bash
# 初始化Git仓库
git init
git add .
git commit -m "Initial commit"

# 创建GitHub仓库并推送
git remote add origin https://github.com/yourusername/MPEV.git
git push -u origin main
```

#### 2. 启用GitHub Actions
- 推送代码后，GitHub会自动检测`.github/workflows/build.yml`
- 在仓库的"Actions"标签页可以看到构建状态

#### 3. 手动触发编译
- 进入仓库的"Actions"标签页
- 选择"Build MPEV"工作流
- 点击"Run workflow"按钮

#### 4. 下载编译结果
- 构建完成后，在"Actions"标签页点击具体的构建记录
- 在"Artifacts"部分可以下载编译好的程序

### 自动发布
当推送到`main`分支时，会自动创建GitHub Release，包含：
- Windows版本：`MPEV.exe`
- Linux版本：`MPEV`
- macOS版本：`MPEV.app`

---

## 方案二：GitLab CI/CD

### 优势
- ✅ 公共仓库免费
- ✅ 支持私有仓库（有限额度）
- ✅ 与GitLab完美集成

### 配置步骤

#### 1. 创建GitLab仓库
```bash
git remote add gitlab https://gitlab.com/yourusername/MPEV.git
git push gitlab main
```

#### 2. 配置CI/CD Runner
- 进入仓库的"Settings" -> "CI/CD"
- 配置Shared Runners或自己的Runner

#### 3. 启用CI/CD
- GitLab会自动检测`.gitlab-ci.yml`
- 在"CI/CD"标签页可以看到流水线状态

---

## 方案三：其他在线编译服务

### Felgo Cloud IDE
- 网址：https://felgo.com/
- 提供在线Qt开发环境
- 支持Web、桌面、移动平台

### Replit
- 网址：https://replit.com/
- 支持C++和Qt项目
- 需要手动配置Qt环境

### Gitpod
- 网址：https://gitpod.io/
- 基于VS Code的在线IDE
- 可以配置Qt开发环境

---

## 本地编译（备选方案）

如果不想使用在线编译，可以使用本地编译：

### Windows
```batch
# 安装Qt 6.x
# 运行构建脚本
build.bat
```

### Linux
```bash
# 安装依赖
sudo apt-get install build-essential qt6-base-dev

# 编译项目
mkdir build && cd build
qmake ../MPEV.pro
make -j$(nproc)
```

### macOS
```bash
# 安装Qt
brew install qt@6

# 编译项目
mkdir build && cd build
qmake ../MPEV.pro
make -j$(sysctl -n hw.ncpu)
```

---

## 常见问题

### Q: 构建失败怎么办？
A: 检查Actions日志，常见原因：
- Qt版本不兼容
- 缺少依赖模块
- 代码编译错误

### Q: 如何修改Qt版本？
A: 修改`.github/workflows/build.yml`中的`version`字段

### Q: 如何添加Qt模块？
A: 在`modules`字段添加需要的模块，如`'qtsvg qtcharts'`

### Q: 构建太慢怎么办？
A: GitHub Actions有缓存机制，首次构建较慢，后续会加速

---

## 相关链接

- [GitHub Actions文档](https://docs.github.com/en/actions)
- [install-qt-action](https://github.com/jurplel/install-qt-action)
- [Qt官方文档](https://doc.qt.io/)
- [GitLab CI/CD文档](https://docs.gitlab.com/ee/ci/)

---

**提示**：推荐使用GitHub Actions，配置简单且完全免费。
