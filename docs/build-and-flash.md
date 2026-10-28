# 编译与烧录

> 说明：本项目已归档，**未在干净环境下重新验证编译**。下面是按当年的工程配置整理的步骤；遇到版本差异请以 CubeIDE 的提示为准。

## 一、环境

| 项 | 值 |
|---|---|
| IDE | **STM32CubeIDE**（第一代工作区是 1.15.1，第二代是 1.18.1；用更新的版本打开通常也能编译） |
| 芯片包 | STM32F1 系列（CubeIDE 自带） |
| 编译器 | GNU Arm Toolchain（CubeIDE 内置） |
| 烧录器 | ST-Link V2（或兼容的 SWD 下载器） |
| 目标板 | STM32F103C8T6，8 MHz 晶振 |

## 二、导入并编译

1. 打开 STM32CubeIDE → **File → Open Projects from File System…**（或 Import → Existing Projects into Workspace）。
2. 目录选 `firmware/gen1-2024/` 或 `firmware/gen2-2025/`，勾选识别出的工程 → Finish。
3. 左侧选中工程 → 右键 **Build Project**（或点锤子图标）。
4. 产物在 `Debug/` 下：`<工程名>.elf`、`.hex`、`.bin`。
   > 仓库里**没有** `Debug/`（已排除构建产物），第一次构建会自动生成。

## 三、烧录

**方式 A：CubeIDE 内直接下载**（推荐）

1. 板子的 SWD 接到 ST-Link：`SWDIO`(PA13)、`SWCLK`(PA14)、`GND`、`3.3V`。
2. ST-Link 插电脑。
3. 工具栏 **Run → Run As → STM32 C/C++ Application**（或 Debug 按钮）。
4. 首次会提示选择调试配置，选 **STM32 C/C++ Application** + 你的 ST-Link。

**方式 B：STM32CubeProgrammer**

1. 用 CubeIDE 构建出 `.hex` / `.bin`。
2. CubeProgrammer 选 ST-Link → Connect → 打开文件 → Download。
3. `BOOT0 = 0`，下载后复位运行。

> 若连不上：检查 `BOOT0` 是否为 0、SWD 线序、是否被读保护（旧片可能设了 RDP，需要先解除，会全片擦除）。

### 砖了怎么救：ST-Link 完全连不上

如果某次写入的设置（误关调试口、改错时钟、进入低功耗）导致 ST-Link 连不上，用 **BOOT0 + 全片擦除**救回来：

1. **BOOT0 跳线置 1**（高电平）；
2. 按**复位** —— 芯片进入系统 Bootloader，不再执行用户代码；
3. 打开 **STM32CubeProgrammer**，选 `ST-LINK` 连接；
4. 执行 **Full chip erase**（全片擦除）；
5. 关闭连接；
6. **BOOT0 恢复 0**，按复位；
7. 回 STM32CubeIDE 重新烧写正确程序。

> 全片擦除会清掉芯片里的所有内容（含原程序）。这是救砖的代价。
> 这套流程来自作者当年的开发备忘，原文见 [`development-notes.md`](development-notes.md) 第三节。

## 四、⚠️ 重新生成代码后必须替换 usbd_hid

这是本项目最容易踩的坑。

**现象**：烧录成功、USB 也识别了，但**只出现键盘或只出现鼠标**，另一个完全不见。

**原因**：`.ioc` 里 USB 设备只能选一个 HID 类，CubeMX 生成的是**单设备**的 `usbd_hid.c/.h`。本项目在 2024-07-21 手工把它们改成了**复合设备**（两个 Report ID）。你在 CubeMX 里点一次 "Generate Code"，这两个文件就会被覆盖回单设备版本。

**解决**：用仓库里保存的手工版覆盖生成结果：

```bash
cp firmware/reference/usbd_hid-override/usbd_hid.c \
   firmware/gen2-2025/Middlewares/ST/STM32_USB_Device_Library/Class/HID/Src/usbd_hid.c
cp firmware/reference/usbd_hid-override/usbd_hid.h \
   firmware/gen2-2025/Middlewares/ST/STM32_USB_Device_Library/Class/HID/Inc/usbd_hid.h
```

（第一代同理，把路径换成 `gen1-2024`。）

覆盖后重新 Build 即可。`usbd_hid-override/README.md` 里有这两个文件与"自动生成版"的差异说明。

## 五、常见问题

| 现象 | 原因 / 处理 |
|---|---|
| USB 完全不识别 | 没接 PA11/PA12；USB 时钟不是 48 MHz；`usbd_conf.c` 的端点配置被改坏 |
| 只识别到键盘（或只有鼠标） | `usbd_hid` 被 CubeMX 覆盖成单设备 → 见第四节 |
| 设备管理器报"未知 USB 设备（描述符请求失败）" | 描述符数组长度常量（`HID_KEY_MOUSE_REPORT_DESC_SIZE`）与实际不符 |
| 串口没反应 | TX/RX 接反（设备 PA9 是 **TX**，接电脑 RX）；波特率不是 9600；共地没接 |
| 串口收到乱码 | 波特率/校验位不对；或 USB-TTL 是 5 V 电平（F103 只耐 3.3 V） |
| 能收到命令但键鼠不动 | 按键命令的 `[2]` 为 0 时不会抬起，后续帧可能被"按住"状态挡住；先发一条全 0 键盘帧 |
| 命令偶发丢失 | 串口单缓冲 + 主循环阻塞，发送太快 → 加间隔 |
| 编译报 `undefined reference to OLED_*` | 把 `Core/oled/` 从编译里排除了但 `main.c` 还在调用 |
| 上电就开始乱动/截屏 | 第二代的上电自检 → 自行注释 `main.c` 里的演示调用（不提供补丁） |

## 六、想自己改的地方（按性价比）

1. **删掉 OLED 死代码**（第二代）：可省掉每条命令的 I²C 开销。
2. **把上电自检做成编译开关**：自己加一个 `#ifdef SELFTEST` 包住那段演示即可。
3. **键盘按住改成非阻塞**：用 SysTick 计时，不要 `HAL_Delay`，这样长按期间还能收命令。
4. **串口加环形缓冲**：解决单缓冲丢帧。
5. **改成绝对坐标**？硬件做不到——它不知道屏幕分辨率；只能在**上位机**侧标定（第三代的 ESP32 方案就是用观测来解这个问题）。
6. **换 MCU**：F103C8 的 64 KB 已接近上限；要加功能建议直接上 F103CB（128 KB）或 G0/G4 系列（带 USB、更便宜更快）。

## 七、构建产物不要提交

如果你把本项目 fork 成自己的仓库，记得忽略：

```gitignore
Debug/
.settings/
*.launch
.DS_Store
```

（本仓库在导出时已经剔除了这些。）
