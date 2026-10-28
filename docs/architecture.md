# 固件结构与运行时行为

## 一、工程分层

```
firmware/gen2-2025/
├── Core/                       应用代码（Quileg 自己写的部分）
│   ├── Src/main.c              ★ 主循环、命令分发、HID 发送、OLED 刷新
│   ├── Serial/Serial.c/.h      ★ 串口初始化 + 收包状态机 + 发送/回显
│   ├── Key/Key.c/.h            ★ 按键扫描（阻塞式）
│   ├── oled/oled.c/.h          软件 I²C 的 OLED 驱动（第一代有屏）
│   ├── Inc/、Startup/          CubeMX 生成的骨架
│   └── Src/stm32f1xx_it.c      中断向量（USART1 的处理实际在 Serial.c）
├── USB_DEVICE/                 CubeMX 生成的 USB 设备初始化与描述符
│   ├── App/usb_device.c        注册 HID 类
│   ├── App/usbd_desc.c         字符串/VID/PID（仍是 ST 默认值）
│   └── Target/usbd_conf.c      USB 底层配置（端点、时钟）
├── Middlewares/ST/STM32_USB_Device_Library/
│   ├── Core/                   USB 设备栈
│   └── Class/HID/Src/usbd_hid.c  ★ 复合报告描述符（手工改过，见 hardware.md）
├── Drivers/                    ST HAL + CMSIS（原样）
└── *.ioc                       CubeMX 工程配置（引脚/时钟/外设的唯一真源）
```

一句话：**`Core/` 是本项目，其余是 ST 的脚手架**。

## 二、启动流程

```
HAL_Init → SystemClock_Config(72MHz) → MX_GPIO_Init
        → MX_USB_DEVICE_Init（USB 上线，等待枚举）
        → MX_USART1_UART_Init
        → OLED_InitDisplay（第一代：亮屏；第二代：仍然执行，但没屏）
        → Key_Init → Serial_Init（开 RX 中断）
        → [第二代] 蜂鸣器 + LED 上电提示
        → [第二代] ⚠️ 上电自检：鼠标画方框 + 按 ⌘+Shift+3 / 安卓截屏
        → 主循环
```

> **上电自检是第二代独有**，也是本项目最"坑"的一处：插到 Mac 上会真的截一张图。归档版不提供补丁；如需关闭，请自行注释 `main.c` 里的演示调用。

## 三、主循环

```c
while (1) {
    initializeMouseKeyBoards();   // 清空报告缓冲
    KeyNum = Key_GetNum();        // ⚠️ 阻塞：按住不放会卡在这里
    switch (KeyNum) { ... }       // 按键触发的测试动作 / OLED 显示

    if (Serial_GetRxPacketFlag() == 1) {
        switch (Serial_RxPacket[0]) {
            case 0x01: processKeyboardCommand(...); break;
            case 0x02: processMouseCommand(...);    break;
            case 0x03: 蜂鸣器 30ms;                  break;
        }
        memcpy(Serial_TxPacket, Serial_RxPacket, 10);
        Serial_SendPacket();      // 回显同一帧（ACK）
    }
}
```

**注意 `Key_GetNum()` 是阻塞实现**：它里面有"等待松手"的 `while` 循环和 20 ms 消抖。按住任意按键期间，**串口命令不会被处理**。这是当年为了简单而做的取舍。

## 四、两条数据通道

```
上位机 ──(9600 8N1, 12 字节帧)──► USART1 RX 中断
                                      │ 状态机：FF → 10 字节 → FE
                                      ▼
                                Serial_RxPacket[10] + 标志位
                                      │ 主循环轮询
                                      ▼
                          processKeyboard/MouseCommand
                                      │
                                      ▼
                        USBD_HID_SendReport(Report ID 1/2) ──► USB ──► 电脑
```

- **键盘**：发按下报告 →（若 `[2] != 0`）延时 → 发全 0 释放报告。
- **鼠标**：按键/位移/滚轮一次发完，**位移本身包含在这一次报告里**。
- **蜂鸣器**：不碰 HID，只翻 LED + 响 30 ms。

## 五、时间成本（为什么一条流程要跑几十秒）

| 环节 | 耗时 |
|---|---|
| 一帧 12 字节 @ 9600 bps | ≈ 12.5 ms |
| 键盘按住（典型 `[2]=0x0A`） | 100 ms（**阻塞**） |
| 鼠标一帧 | ≈ 12.5 ms（不阻塞） |
| 回显一帧 | ≈ 12.5 ms |
| 上位机帧间间隔 | 助手侧约 10–50 ms |

所以一条含 200 个动作的流程，光串口与按住时间就在 **20–40 秒**量级——这正是第三代"AI 闭环、按需动作"想解决的问题。

## 六、已知限制（按严重度）

1. **上电自检会注入按键**（仅第二代）→ 需自行修改源码关闭（不提供补丁）。
2. **串口单缓冲**：处理期间到达的新帧会覆盖旧帧 → 上位机必须节流。
3. **按键扫描阻塞**：按住设备上的物理键会冻住串口处理。
4. **键盘按住同样阻塞主循环**：最长 2.55 s。
5. **OLED 死代码**（第二代）：每条命令都做软件 I²C，白耗时间。
6. **无坐标系**：只有相对位移，靠"归位 + 数格子"，分辨率/窗口一变就得重录。
7. **无反馈**：设备不知道执行结果，失败无法感知。
8. **纯文本流程**：`.quileg` 是 hex + 注释，没有条件、循环、变量。

## 七、为什么有了第三代

前两代的根本问题不是"做得不够好"，而是**形态决定的**：

| 缺陷 | 后果 |
|---|---|
| 盲操作 | 不知道上一动作是否生效，无法重试/回退 |
| 相对坐标 | 必须先归位，分辨率一变就要重录 |
| 100 ms 级节拍 | 一轮几百条指令 → 几十秒，AI 试错成本无法接受 |

第三代 **Quileg HID** 换掉了每一环：ESP32-S3 走蓝牙（不再插线）、目标换成手机（投屏看画面）、AI 通过 MCP 一步一看（闭环、可校验）。详见 <https://jianghui1974.github.io/Quileg-HID/>。
