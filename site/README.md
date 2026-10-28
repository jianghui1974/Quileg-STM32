# 项目主页（静态站）

`site/` 是 Quileg STM32 开源项目的**介绍页**，视觉风格与 [Quileg HID 官网](https://jianghui1974.github.io/Quileg-HID/) 保持一致，内容全部来自仓库内已核实的文档。

```
site/
├── index.html        单页：能做什么 / 原理 / 两代 / 开始 / 协议 / 文档 / FAQ / 许可
├── commands.html     命令字典：96 条字节命令 + 逐字节拆解 + 搜索过滤
├── styles.css        全部样式（无外部依赖、无 CDN）
└── assets/
    ├── logo.svg
    ├── product-cases.png
    └── product-oled-in-use.png
```

## 本地预览

```bash
cd site && python3 -m http.server 8000
# 打开 http://127.0.0.1:8000
```

直接双击 `index.html` 也能看（没有 fetch、没有模块，`file://` 可用）。

> `commands.html` 里有一个很小的内联脚本，只做「搜索过滤」；禁用脚本时全部命令照常显示。

## 发布到 GitHub Pages

两种方式任选：

1. **从 `/site` 发布**：仓库 Settings → Pages → Source 选 `Deploy from a branch`，分支 `main`、目录 `/site`。
2. **作为仓库根页面**：把 `site/` 里的文件复制到仓库根（`index.html`、`styles.css`、`assets/`），Pages 选 `/ (root)`。

> 页面里的仓库/文档链接**已写死**为 `https://github.com/jianghui1974/Quileg-STM32`（不依赖 JS，禁用脚本也能点）。
> **如果最终仓库名不是 `Quileg-STM32`，全局替换 `Quileg-STM32` 即可**（约 20 处，只影响链接）。

## 待补

- 硬件资料（正在从 iCloud 取回）：拿到后会把引脚照片/接线示意补进文档，页面再按需加图。
- 英文版：Quileg HID 官网有 `/en/`；本页目前只有中文，需要时可按同样结构加一份。

## 内容口径

- 只写仓库里能核实的事实：`.ioc`、源码、备忘录整理稿。
- 不写推测：主控板型号（Nano STM32 V1.1）来自厂商手册，页面直接写；厂商手册/原理图不随仓库分发。
- 不承诺可购买/可维护：页面第一屏就写明"已归档、前两代未公开发布"。
