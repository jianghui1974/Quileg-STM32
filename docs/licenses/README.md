# 第三方许可文本位置

本目录用于安放第三方组件的许可说明。**ST / Arm 的完整许可原文没有统一收在这里**，而是保留在各自组件目录内（这样与上游一致，也避免出现"两份不同版本"）。

| 组件 | 许可 | 原文位置 |
|---|---|---|
| STM32F1xx HAL Driver | BSD-3-Clause | `firmware/gen1-2024/Drivers/STM32F1xx_HAL_Driver/LICENSE.txt`（第二代同路径） |
| CMSIS | Apache-2.0 | `firmware/gen1-2024/Drivers/CMSIS/LICENSE.txt` |
| STM32 USB Device Library | Ultimate Liberty SLA0044 | 未附全文；见每个源文件头部的声明，官方地址 <https://www.st.com/SLA0044> |

## 关于 SLA0044

ST 的 USB 设备库属于 "Ultimate Liberty" 许可（SLA0044），允许在自有产品中使用与再分发，但有一些条件（例如不得用 ST 名义背书、保留声明等）。**本项目的 MIT 许可不能、也没有覆盖它**。

如果你要：

- **个人学习/复刻**：按现状使用即可，保留文件头声明。
- **商业产品**：请自行阅读 SLA0044 原文，判断你的分发方式是否满足要求；必要时向 ST 获取书面确认。

## 运行时前端库

`tools/web-assistant/` 通过 CDN 使用 jQuery、jQuery UI、Nestable2、Font Awesome，**仓库内不含这些库的代码**，因此也没有在此收录它们的许可全文。离线分发时请自行附带其许可（均为 MIT / CC BY 4.0 / SIL OFL）。

详见 [../../THIRD_PARTY_NOTICES.md](../../THIRD_PARTY_NOTICES.md)。
