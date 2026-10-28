# 第一代固件（2024-07）

**原始工程名**：`go_mouse_keyboard_202407310055`（2024-07-31 00:55 定稿，第一代最后一版）
**开发环境**：STM32CubeIDE 1.15.1 工作区

## 这是什么

STM32F103C8T6 上的 USB HID 复合键鼠：从 USART1（9600）收 12 字节命令帧，转成键盘/鼠标 HID 报告发给电脑。带一块 0.96" 128×64 OLED（中景园 `ZJY096I0400WG01`，软件 I²C），实时显示位移、按键、滚轮与组合键名。

**串口协议见 [`../../docs/protocol.md`](../../docs/protocol.md)**，硬件见 [`../../docs/hardware.md`](../../docs/hardware.md)。

## 目录

| 路径 | 内容 |
|---|---|
| `Core/Src/main.c` | 主循环、命令分发、HID 发送、OLED 刷新 |
| `Core/Serial/` | 串口 + 收包状态机 |
| `Core/Key/` | 按键扫描（本代 3 个键） |
| `Core/oled/` | OLED 驱动（软件 I²C） |
| `USB_DEVICE/`、`Middlewares/` | USB 设备栈与**手工改过的复合 HID 描述符** |
| `Drivers/` | ST HAL + CMSIS（原样） |
| `*.ioc` | CubeMX 配置（引脚/时钟的唯一真源） |

> 已排除：`Debug/`（构建产物）、`.settings/`、`.launch`（含原作者本机路径）、`.DS_Store`。

## 编译

用 STM32CubeIDE 打开本目录（Open Projects from File System），Build 即可。烧录与注意事项见 [`../../docs/build-and-flash.md`](../../docs/build-and-flash.md)。

## 本代特点

- **没有蜂鸣器**，有 LED（PA8）。
- **没有上电自检**（第二代才加的）。
- 上电后安静地等串口命令。

## 已知问题

串口单缓冲、按键扫描阻塞、相对坐标。完整清单见 [`../../docs/architecture.md`](../../docs/architecture.md)。
