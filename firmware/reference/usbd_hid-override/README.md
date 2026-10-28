# usbd_hid 手工替换版（复合 HID）

这两个文件（`usbd_hid.c`、`usbd_hid.h`）是 **2024-07-21** 手工改造的版本，用来替换 STM32CubeMX 自动生成的同名文件。

## 为什么需要它们

`.ioc` 里 USB 设备只能配置**一个** HID 类。CubeMX 生成的 `usbd_hid.c` 因此只描述一种设备（要么键盘、要么鼠标）。本项目需要**同一个 USB 接口里同时有键盘和鼠标**，于是手工把报告描述符改成复合式：

| Report ID | 设备 | 报告长度 |
|---|---|---|
| `1` | 键盘 | 9 字节（ID + 8） |
| `2` | 鼠标 | 5 字节（ID + 4），含 X / Y / Wheel |

改动点（相对自动生成版）：

1. 描述符数组改名为 `HID_KEY_MOUSE_ReportDesc`，内容包含两个 `COLLECTION`（Keyboard + Mouse）与两个 `REPORT_ID`。
2. 长度常量 `HID_KEY_MOUSE_REPORT_DESC_SIZE` 同步调整。
3. 鼠标描述符里增加了 `USAGE (Wheel)`（`0x09, 0x38`），并把 X/Y/Wheel 一起声明为相对轴（`REPORT_COUNT 3`），逻辑范围 −127…127。
4. 调用处（`usbd_hid.c` 内的 `USBD_HID_GetReportDescriptor` 等）改为指向新数组。

## 什么时候必须用它

**只要你在 CubeMX 里点过 "Generate Code"**，这两个文件就会被覆盖回单设备版。此时必须：

```bash
cp usbd_hid.c  ../../gen2-2025/Middlewares/ST/STM32_USB_Device_Library/Class/HID/Src/usbd_hid.c
cp usbd_hid.h  ../../gen2-2025/Middlewares/ST/STM32_USB_Device_Library/Class/HID/Inc/usbd_hid.h
```

（第一代把路径换成 `gen1-2024`。）然后重新 Build。

## 症状对照

| 现象 | 说明 |
|---|---|
| 只出现键盘，鼠标没有 | 描述符里少了 Report ID 2 那段 |
| 只出现鼠标，键盘没有 | 反之 |
| 设备管理器报"描述符请求失败" | 长度常量与实际数组长度不一致 |
| 鼠标能动但滚轮没反应 | 描述符里缺 `USAGE (Wheel)` |

## 来源与技术参考

这套复合描述符的写法不是本项目原创，来自一篇公开的技术分享：

- **MCU起航《基于STM32CUBE的USB鼠标键盘二合一》**（腾讯云开发者社区，2020-06-29；原载于微信公众号，后台关键词「USB键盘鼠标二合一」）
  <https://cloud.tencent.cn/developer/article/1652367>

**与文章一致的部分**（可逐条对上）：

- 配置描述符：`bNumEndpoints` 由 1 改成 **2**；接口协议由 2（鼠标）改成 **1**（键盘）
- 附加一个"鼠标输出端点"描述符：`0x01, 0x03, 0x10, 0x00`
- `HID_EPIN_SIZE` = **0x10**
- 报告描述符长度 **117**（键盘段 65 + 鼠标段 62，两个 Report ID）

**Quileg 自己加的部分**：在鼠标描述符里增加了 `USAGE (Wheel)`（`0x09,0x38`），长度因此从 **117 → 119**（`HID_KEY_MOUSE_REPORT_DESC_SIZE 119U`），见 `usbd_hid.c` 里带"新增的滚轮使用"注释的那一行。

> **许可说明**：那篇文章的示例工程通过公众号/网盘分发，**没有附带开源许可**；它本身主要是在 ST 的 USB Device Library 模板上改描述符。本仓库**不包含**它的原始工程，只保留 Quileg 当年实际使用的这两个文件（模板部分仍属 ST，SLA0044）。如果你是该方案的作者、希望调整署名方式，请联系 <contact@airdiode.com>。

## 备注

- 这两个文件基于 ST 的 USB Device Library 模板（SLA0044），所以文件头保留 ST 版权声明；Quileg 的改造部分按 MIT 提供。详见 [`../../../THIRD_PARTY_NOTICES.md`](../../../THIRD_PARTY_NOTICES.md)。
- 文件为 CRLF 换行，与当年 CubeIDE 环境一致。
