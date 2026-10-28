# Quileg 助手（网页版上位机）

单文件网页，用 **Web Serial API** 直接控制设备：连接串口 → 编辑/导入流程 → 一键执行。

```
tools/web-assistant/
├── quileg-assistant.html   ★ 主文件：打开它就行（原文件名「Quileg 助手.html」）
├── assets/                 操作示意图（已从原外链本地化）
└── README.md
```

## 怎么用

1. 用**桌面版 Chrome 或 Edge**（Web Serial 目前只有 Chromium 系支持）打开 `quileg-assistant.html`。
   > 直接双击打开（`file://`）即可，不需要本地服务器。
2. 设备接 USB-TTL，选对串口（9600）。
3. 点页面上的「连接」→ 在浏览器弹窗里选择串口 → 授权。
4. 导入 `../../examples/*.quileg` 或自己拖拽编辑一条流程 → 点执行。

> 浏览器需要用户**手动授权**才能访问串口；页面不会自动连接，也不会把流程上传到任何地方。

## 功能

- 键盘 / 鼠标指令的可视化编辑，拖拽排序（Nestable2）
- 预设快捷按钮（常用键、组合键、鼠标动作）
- 批量生成指令（文本 → 多行 hex）
- `.quileg` 流程的导入 / 导出（纯文本，一行一条 hex + 中文注释）
- 中英文界面切换

## 两个必须知道的点

1. **首次打开需要联网**：页面通过 cdnjs 加载 jQuery、jQuery UI、Nestable2、Font Awesome。离线使用需自行把这些库放到本地并改 `<script>`/`<link>` 的引用。仓库内**不含**这些库。
2. **发送节奏**：固件的串口收包是**单缓冲**，处理期间到达的新帧会被覆盖。助手内部对帧间做了节流；如果你自己写脚本，记得加间隔（详见 [`../../docs/protocol.md`](../../docs/protocol.md) 第六节）。

## 与本仓库其他部分的关系

- 协议：[`../../docs/protocol.md`](../../docs/protocol.md)
- 固件：[`../../firmware/gen1-2024/`](../../firmware/gen1-2024/)、[`../../firmware/gen2-2025/`](../../firmware/gen2-2025/)
- 示例流程：[`../../examples/`](../../examples/)

## 本仓库版本相对原版的改动（仅脱敏，未改功能）

- 6 张操作示意图由 `oss.godar.cn` 外链改为本地 `assets/`。
- 页脚版权占位文案（原为「© 2024 网页版权信息」）改为 `© 2024-2025 Quileg · MIT License`。
- 导出 `.quileg` 时的文件头注释：原指向 `Quileg.cn` 官网，改为指向本仓库 `docs/protocol.md`。

功能、界面与指令集与原版一致。原版是 2024-09-02 的工作版本。
