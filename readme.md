# 操作系统内核学习与实践

[![C](https://img.shields.io/badge/Language-C-A8B9CC.svg)](https://en.wikipedia.org/wiki/C_(programming_language))
[![Linux](https://img.shields.io/badge/OS-Linux-1793D1.svg)](https://www.kernel.org/)
[![Kernel](https://img.shields.io/badge/Kernel-4.14.141-8A2BE2.svg)](https://cdn.kernel.org/pub/linux/kernel/v4.x/linux-4.14.141.tar.xz)
[![Arch](https://img.shields.io/badge/Arch-x86__64-0078D6.svg)](https://en.wikipedia.org/wiki/X86-64)
[![License: MIT](https://img.shields.io/badge/License-MIT-green.svg)](LICENSE)
[![GitHub stars](https://img.shields.io/github/stars/MiChuan/OS_Kernel?style=social)](https://github.com/MiChuan/OS_Kernel/stargazers)

本项目基于 MIT License 开源发布，欢迎在遵守许可证条款的前提下自由使用、修改与再发布。

## 目录导航

- [项目简介](#introduction)
- [项目亮点](#highlights)
- [实验模块](#experiments)
- [开发环境](#requirements)
- [目录结构](#structure)
- [快速开始](#quick-start)
- [常见问题](#faq)
- [开源声明与版权归属](#open-source-statement--copyright)
- [其他说明](#notes)

<a id="introduction"></a>
## 项目简介

本项目是操作系统课程学习与 Linux 内核编程实践仓库，基于 **linux-4.14.141** 内核，包含两个相互独立的实验：

1. **编译内核与添加系统调用**：通过重新编译内核的方式，新增 `mycopy` 系统调用实现用户态文件拷贝，并同步添加 `hello_world` 演示调用。
2. **模块化添加字符设备驱动**：以内核模块方式实现字符设备驱动 `mydev`，支持打开 / 关闭、读 / 写等基本操作。

每个实验均包含完整源码、测试程序与逐步实验文档，可作为操作系统课程设计、内核编程入门与复习参考。

<a id="highlights"></a>
## 项目亮点

- **双实验覆盖内核核心机制**：系统调用与字符设备驱动是 Linux 内核两大基础机制，本项目提供完整可运行的实现。
- **完整可复现**：每个实验均包含修改后的内核文件、Makefile、测试程序与图文实验文档，可按步骤复现。
- **经典内核版本**：基于 linux-4.14.141，在 Ubuntu 16.04 LTS 环境实测通过。
- **学习导向**：文档含环境搭建、关键代码分析、调试记录与核心代码目录说明，适合课程学习与期末复习。

<a id="experiments"></a>
## 实验模块

### 实验一：编译内核与添加系统调用

采用重新编译内核的方法，为系统添加自定义系统调用，实现文件拷贝功能，并编写应用程序进行测试。

- 通过 `SYSCALL_DEFINE2` 宏实现内核例程 `mycopy`，使用 `strndup_user` 完成用户空间文件名到内核空间的拷贝
- 配合 `set_fs(KERNEL_DS)` 调用内核文件接口，并在返回用户空间前恢复访问范围
- 涉及内核源码修改：`kernel/sys.c`、`include/linux/syscalls.h`、`arch/x86/entry/syscalls/syscall_64.tbl`
- 系统调用分配（x86_64）：`333 hello_world`、`334 mycopy`
- 提供测试程序 `test_copy.c`、`test_hello.c` 与内核空间访问示例 `example.c`

详细文档：[compile_kernel&add_syscall/readme.md](https://github.com/MiChuan/OS_Kernel/blob/master/compile_kernel%26add_syscall/readme.md)

### 实验二：模块化添加设备驱动

采用模块方法，为系统添加一个字符设备驱动程序，实现打开 / 关闭、读 / 写等基本操作，并编写应用程序进行测试。

- 通过 `register_chrdev` 注册字符设备（实验环境主设备号为 241，由系统动态分配），使用 `mknod` 生成设备文件
- 使用 `copy_to_user` / `copy_from_user` 完成内核与用户空间的数据交换
- 实现简单的互斥占用逻辑（`open_process` 计数 + `try_module_get` / `module_put`）
- 提供 `Makefile` 编译脚本与测试程序 `test.c`

详细文档：[modularly_add_device_drivers/readme.md](https://github.com/MiChuan/OS_Kernel/blob/master/modularly_add_device_drivers/readme.md)

<a id="requirements"></a>
## 开发环境

两个实验均基于以下环境验证：

- 操作系统：Ubuntu 16.04 LTS 64 位
- 内核版本：linux-4.14.141
- 编译器：GCC 5.4.0
- 编辑器：Vim
- 硬件：Intel Core i5-6200U @ 2.30GHz × 4，内存 8G

> 提示：实验一需要重新编译并安装内核，实验二需要加载内核模块，均涉及系统级操作，建议在虚拟机或实验专用环境中复现。

<a id="structure"></a>
## 目录结构

```text
OS_Kernel/
├── compile_kernel&add_syscall/       # 实验一：编译内核与添加系统调用
│   ├── readme.md                     # 实验文档（环境、步骤、关键代码）
│   ├── sys.c                         # 系统调用例程（新增 mycopy 等）
│   ├── syscalls.h                    # 系统调用函数声明
│   ├── syscall_64.tbl                # 系统调用表（x86_64）
│   ├── mycopy.c                      # 修改内容总结
│   ├── example.c                     # 内核空间访问说明示例程序
│   ├── test_copy.c                   # 测试文件拷贝系统调用
│   ├── test_hello.c                  # 测试 hello_world 系统调用
│   └── test_pro/                     # 测试程序与样例文件（IOT / IOT1 / test_copy）
├── modularly_add_device_drivers/     # 实验二：模块化添加设备驱动
│   ├── readme.md                     # 实验文档（环境、步骤、关键代码、调试记录）
│   ├── mydev.c                       # 字符设备驱动程序源码
│   ├── Makefile                      # 模块编译脚本
│   ├── help.txt                      # Makefile 语法与模块加载命令参考
│   ├── test.c                        # 驱动测试程序源码
│   └── test                          # 测试程序编译产物
├── LICENSE                           # MIT 许可证
└── readme.md                         # 项目说明（本文件）
```

<a id="quick-start"></a>
## 快速开始

获取项目代码：

```bash
git clone https://github.com/MiChuan/OS_Kernel.git
cd OS_Kernel
```

两个实验相互独立，可直接进入对应目录查看源码与完整实验文档：

- 实验一：[compile_kernel&add_syscall/](https://github.com/MiChuan/OS_Kernel/tree/master/compile_kernel%26add_syscall) —— 从下载内核源码（linux-4.14.141）开始，按文档步骤修改 `sys.c`、`syscalls.h`、`syscall_64.tbl`，编译安装内核后运行测试程序
- 实验二：[modularly_add_device_drivers/](https://github.com/MiChuan/OS_Kernel/tree/master/modularly_add_device_drivers) —— 将 `mydev.c` 与 `Makefile` 放入内核源码目录，执行 `make` 编译生成 `mydev.ko`，依次执行 `insmod`、`mknod` 后运行测试程序

> 注意：内核编译与模块加载需要 root 权限，并保证编译环境与运行内核版本一致（linux-4.14.141）。

<a id="faq"></a>
## 常见问题

### 1. 编译内核或模块失败

请确认内核源码版本为 linux-4.14.141，且模块编译使用的内核源码路径与当前运行内核一致；同时检查是否已安装编译所需依赖（如 `libncurses*`、`build-essential`、`openssl` 等）。

### 2. insmod 加载模块失败

请以 root 权限执行；内核模块对编译环境敏感，模块编译使用的内核版本必须与加载环境匹配，否则会因 version magic 不一致而报错。

### 3. mknod 创建设备文件时设备号不正确

主设备号以 `/proc/devices` 实际显示为准。实验文档中的 241 为实验环境动态分配的结果，其他环境下请按实际显示值执行 `mknod /dev/mydev c <主设备号> 0`。

### 4. 系统调用号冲突或无效

`syscall_64.tbl` 仅适用于 x86_64 架构，实验分配的 333 / 334 基于 linux-4.14.141；不同内核版本或架构下系统调用号可能不同，请重新确认后使用。

### 5. 在新版内核上编译报错

实验基于较旧内核实现，`set_fs` / `get_fs`、`sys_open` / `sys_read` / `sys_write` 等接口在新版本内核中已发生变化，建议使用文档所述环境（Ubuntu 16.04 LTS + linux-4.14.141）复现。

<a id="open-source-statement--copyright"></a>
## 开源声明与版权归属

本项目基于 MIT License 开源发布（详见 [LICENSE](LICENSE)）。在保留原始署名与许可证声明的前提下，使用者可以自由复制、修改、分发、再发布，或用于个人学习与二次开发。

若将本项目代码用于课程设计、毕业设计展示或二次开发，建议保留原作者信息、仓库地址及许可说明，以尊重原始创作贡献。

<a id="notes"></a>
## 其他说明

- 本仓库为操作系统课程学习与实践项目，源码与文档仅供学习参考。
- 实验一需要重新编译并安装内核，请在虚拟机或实验专用环境中操作，避免影响日常使用的主机系统。
- 仓库中包含部分编译产物（如 `test_pro/`、`test`），可直接参考运行结果。
- 各实验子目录中的文档还包含关键代码分析与调试记录，建议结合源码阅读。
