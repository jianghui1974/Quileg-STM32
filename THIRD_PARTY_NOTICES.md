# 第三方组件与许可

本仓库由两部分组成：**Quileg 自己的代码**（MIT，见 `LICENSE`）与 **ST / Arm 的组件**（保留其原有许可）。本文件说明后者。

## 一、随仓库分发的第三方代码

| 组件 | 位置 | 许可 | 说明 |
|---|---|---|---|
| STM32F1xx HAL Driver | `firmware/*/Drivers/STM32F1xx_HAL_Driver/` | **BSD-3-Clause** | 完整许可文本随目录提供：`LICENSE.txt`（ST 注明若未随包提供许可则适用 BSD-3-Clause） |
| CMSIS（Cortex-M 头文件等） | `firmware/*/Drivers/CMSIS/` | **Apache-2.0** | 完整许可文本随目录提供：`LICENSE.txt` |
| STM32 USB Device Library | `firmware/*/Middlewares/ST/STM32_USB_Device_Library/` | **Ultimate Liberty license SLA0044** | 每个源文件头部有声明，未另附许可全文。原文：`This software component is licensed by ST under Ultimate Liberty license SLA0044 … www.st.com/SLA0044` |

> **重要**：MIT 只覆盖 Quileg 自己写的代码与文档（`Core/`、`USB_DEVICE/` 中以 Quileg 为主的改动、`tools/`、`docs/` 等）。ST / Arm 组件**不被 MIT 重新授权**；再分发时请保留其许可与版权声明，并遵守各自条款。
>
### 技术参考（非代码依赖）

`firmware/reference/usbd_hid-override/` 里复合 HID 描述符的写法参考了公开文章 **MCU起航《基于STM32CUBE的USB鼠标键盘二合一》**（2020，<https://cloud.tencent.cn/developer/article/1652367>）；Quileg 在此基础上增加了 Wheel 轴（描述符 117 → 119 字节）。该文的示例工程未附开源许可，本仓库不包含它的原始工程。详见该目录的 `README.md`。

> `Core/Src/main.c` 等 CubeMX 生成的骨架文件头部仍带 ST 版权行——这些文件的**原始模板**属于 ST，Quileg 的改动部分按 MIT 提供。若你要把本项目用于商业产品，请自行核对 SLA0044 的条款（该许可允许在专有产品中使用，但不得用 ST 名义背书等）。

## 二、运行时依赖（**未**随仓库打包）

`tools/web-assistant/quileg-assistant.html` 通过 CDN 加载以下前端库，仓库内不含其代码，也不对其做再分发：

| 库 | 版本 | 许可 |
|---|---|---|
| jQuery | 3.6.0 | MIT |
| jQuery UI | 1.12.1 | MIT |
| Nestable2 | 1.6.0 | MIT（原作者 Ramon Smit / David Bushell） |
| Font Awesome | 6.1.1 | 图标 CC BY 4.0；字体 SIL OFL 1.1；代码 MIT |

因此**首次打开该页面需要联网**；离线使用需自行把这几个库下载到本地并改引用路径。

## 三、图片与文档

- `docs/images/product-cases.png`、`docs/images/product-oled-in-use.png`：产品实拍（多外壳合影 / 通电使用中），来自作者自己的作品页 <https://www.godar.cn/products/quileg/>，随本项目以同一许可提供。
- `tools/web-assistant/assets/*`：网页助手的操作示意图，作者自制。

## 四、商标

STM32、STM32Cube 是 STMicroelectronics 的商标；Arm、Cortex 是 Arm Limited 的商标；Apple、iPhone、iPad、macOS 是 Apple Inc. 的商标。本项目与上述公司无隶属关系，仅为兼容性说明而提及。
