/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "usart.h"
#include "usb_device.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
#include "usbd_hid.h"
#include "oled.h" //显示屏
#include "Serial.h" //串口
#include "Key.h" //按键
// 实现鼠标和键盘一体化后，由.ioc文件自动生成的usbd_hid.h 和 usbd_hid.c文件，需要被手工修，或直接替换
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

// HID report descriptor, example for a simple mouse
static uint8_t Mouse[9] = {2,0,0,0,0,0,0,0,0};
static uint8_t KeyBoard[9] = {1,0,0,4,0,0,0,0,0};
static uint8_t KeyBoard01[9] = {1,0,0,0,0,0,0,0,0};
static uint8_t mouse_keyboard_test_1[10] = {0x01,0x0A,0x0A,0x20,0x00,0x00,0x00,0x00,0x00,0x00};//MAC截屏测试命令
static uint8_t mouse_keyboard_test_2[10] = {0x02,0x00,0x40,0x00,0x00,0x00,0x00,0x00,0x00,0x00};//向右移动鼠标
static uint8_t mouse_keyboard_test_3[10] = {0x02,0x00,0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x00};//向左移动鼠标
static uint8_t mouse_keyboard_test_4[10] = {0x02,0x00,0x00,0xC0,0x00,0x00,0x00,0x00,0x00,0x00};//向上移动鼠标
static uint8_t mouse_keyboard_test_5[10] = {0x02,0x00,0x00,0x40,0x00,0x00,0x00,0x00,0x00,0x00};//向下移动鼠标
static uint8_t mouse_keyboard_test_6[10] = {0x01,0x01,0x0A,0x3A,0x00,0x00,0x00,0x00,0x00,0x00};//安卓截屏测试命令

// HID 键名数组
const char* HID_KEY_NAMES[] = {
    "None", "Err", "Err", "Err", "a", "b", "c", "d", "e", "f",
    "g", "h", "i", "j", "k", "l", "m", "n", "o", "p",
    "q", "r", "s", "t", "u", "v", "w", "x", "y", "z",
    "1", "2", "3", "4", "5", "6", "7", "8", "9", "0",
    "Enter", "Esc", "Bksp", "Tab", "Space", "-", "=", "[", "]", "\\",
    "None", ";", "'", "`", ",", ".", "/", "Caps", "F1", "F2",
    "F3", "F4", "F5", "F6", "F7", "F8", "F9", "F10", "F11", "F12",
    "PrtSc", "ScrLk", "Pause", "Ins", "Home", "PgUp", "Del", "End", "PgDn", "Right",
    "Left", "Down", "Up", "NumLk", "KP /", "KP *", "KP -", "KP +", "KP Enter", "KP 1",
    "KP 2", "KP 3", "KP 4", "KP 5", "KP 6", "KP 7", "KP 8", "KP 9", "KP 0", "KP .",
    "Err", "Err", "Err", "Err", "Err", "Err", "Err", "Err", "Err", "Err",
    "Err", "Err", "Err", "Err", "Err", "Err", "Err", "Err", "Err", "Err"
};

// 修饰键数组
const char* MODIFIER_KEYS[] = {
    "", "Ctrl", "Shift", "Ctrl+Shift", "Alt", "Ctrl+Alt", "Shift+Alt", "Ctrl+Shift+Alt",
    "GUI", "Ctrl+GUI", "Shift+GUI", "Ctrl+Shift+GUI", "Alt+GUI", "Ctrl+Alt+GUI", "Shift+Alt+GUI", "Ctrl+Shift+Alt+GUI"
};

// Declare the external USB device handle
extern USBD_HandleTypeDef hUsbDeviceFS;

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USB_DEVICE_Init();
  MX_USART1_UART_Init();

  /* USER CODE BEGIN 2 */
  OLED_InitDisplay();//OLED显示初始化 需要放在 MX_GPIO_Init() 之后，用 oled.c 里的初始化设置代替 gpio.c 里的系统初始化
  Key_Init();//按键初始化
  Serial_Init();// 串口 UART 的初始化
  uint8_t KeyNum; // 定义用于接收按键键码的变量

  //蜂鸣器
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);
  HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_8);//LED指示灯闪烁
  HAL_Delay(200);  // 响200ms
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);
  HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_8);//LED指示灯闪烁
  HAL_Delay(200);  // 延时

  processMouseCommand(mouse_keyboard_test_2);// 右
  HAL_Delay(500);  // 延时
  processMouseCommand(mouse_keyboard_test_3);// 左
  processMouseCommand(mouse_keyboard_test_3);// 左
  HAL_Delay(500);  // 延时
  processMouseCommand(mouse_keyboard_test_4);// 上
  HAL_Delay(500);  // 延时
  processMouseCommand(mouse_keyboard_test_5);// 下
  processMouseCommand(mouse_keyboard_test_5);// 下
  HAL_Delay(500);  // 延时
  processMouseCommand(mouse_keyboard_test_3);// 左
  HAL_Delay(500);  // 延时
  processMouseCommand(mouse_keyboard_test_2);// 右
  processMouseCommand(mouse_keyboard_test_2);// 右
  HAL_Delay(500);  // 延时
  processMouseCommand(mouse_keyboard_test_5);// 下
  HAL_Delay(500);  // 延时
  processMouseCommand(mouse_keyboard_test_4);// 上
  processMouseCommand(mouse_keyboard_test_4);// 上
  HAL_Delay(500);  // 延时

  processKeyboardCommand(mouse_keyboard_test_1);// MAC截屏
  processKeyboardCommand(mouse_keyboard_test_6);// 安卓截屏

  //蜂鸣器
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);
  HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_8);//LED指示灯闪烁
  HAL_Delay(200);  // 响400ms
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);
  HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_8);//LED指示灯闪烁
  HAL_Delay(200);  // 响400ms
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);
  HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_8);//LED指示灯闪烁
  HAL_Delay(200);  // 响400ms
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);
  HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_8);//LED指示灯闪烁

  /*设置发送数据包数组的初始值*/
  memset(Serial_TxPacket, 0, 10); //包有效数据长度为10，0-9

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
      initializeMouseKeyBoards(); // 初始化鼠标键盘预设函数
      KeyNum = Key_GetNum();			//获取按键键码

      switch (KeyNum)
      {
      	  case 0://led测试 PA0 MAC截屏及鼠标移动测试
      	  case 10://led测试 PB10 MAC截屏及鼠标移动测试
      		HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_8);//LED指示灯闪烁

      		//蜂鸣器
      		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);
      		HAL_Delay(200);  // 响200ms
      		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);

      		processKeyboardCommand(mouse_keyboard_test_1);// 截屏
      		HAL_Delay(200);  // 延时
      		processMouseCommand(mouse_keyboard_test_2);// 右
      		HAL_Delay(1000);  // 延时
      		processMouseCommand(mouse_keyboard_test_3);// 左
      		processMouseCommand(mouse_keyboard_test_3);// 左
      		HAL_Delay(1000);  // 延时
      		processMouseCommand(mouse_keyboard_test_4);// 上
      		HAL_Delay(1000);  // 延时
      		processMouseCommand(mouse_keyboard_test_5);// 下
      		processMouseCommand(mouse_keyboard_test_5);// 下
      		HAL_Delay(1000);  // 延时
      		processMouseCommand(mouse_keyboard_test_3);// 左
      		HAL_Delay(1000);  // 延时
      		processMouseCommand(mouse_keyboard_test_2);// 右
      		processMouseCommand(mouse_keyboard_test_2);// 右
      		HAL_Delay(1000);  // 延时
      		processMouseCommand(mouse_keyboard_test_5);// 下
      		HAL_Delay(1000);  // 延时
      		processMouseCommand(mouse_keyboard_test_4);// 上
      		processMouseCommand(mouse_keyboard_test_4);// 上

      		//蜂鸣器
      		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);
      		HAL_Delay(400);  // 响400ms
      		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);

      		break;
      	  case 11://led测试 PB11 安卓截屏及鼠标移动测试
      		HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_8);//LED指示灯闪烁

      		//蜂鸣器
      		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);
      		HAL_Delay(200);  // 响200ms
      		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);

        	processKeyboardCommand(mouse_keyboard_test_6);// 截屏
        	HAL_Delay(200);  // 延时
        	processMouseCommand(mouse_keyboard_test_2);// 右
        	HAL_Delay(1000);  // 延时
        	processMouseCommand(mouse_keyboard_test_3);// 左
        	processMouseCommand(mouse_keyboard_test_3);// 左
        	HAL_Delay(1000);  // 延时
        	processMouseCommand(mouse_keyboard_test_4);// 上
        	HAL_Delay(1000);  // 延时
        	processMouseCommand(mouse_keyboard_test_5);// 下
        	processMouseCommand(mouse_keyboard_test_5);// 下
        	HAL_Delay(1000);  // 延时
        	processMouseCommand(mouse_keyboard_test_3);// 左
        	HAL_Delay(1000);  // 延时
        	processMouseCommand(mouse_keyboard_test_2);// 右
        	processMouseCommand(mouse_keyboard_test_2);// 右
        	HAL_Delay(1000);  // 延时
        	processMouseCommand(mouse_keyboard_test_5);// 下
        	HAL_Delay(1000);  // 延时
        	processMouseCommand(mouse_keyboard_test_4);// 上
        	processMouseCommand(mouse_keyboard_test_4);// 上

      		//蜂鸣器
      		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);
      		HAL_Delay(400);  // 响400ms
      		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);

      		break;
      	  case 5:
      	  case 6:
      	  case 7:
      	  case 12:
      	  case 14:
      		OLED_ShowString(4, 3, "             "); // 擦干净
      		OLED_ShowNum(4, 3, KeyNum, 2);
      		break;
          default:
              break;
      }

	  if (Serial_GetRxPacketFlag() == 1)			//检查串口包接收数据的标志位
	  {
	      switch (Serial_RxPacket[0]) // 命令
	      {
	      	case 0x00: // 保留
	      		break;
	        case 0x01: // 键盘
	            processKeyboardCommand(Serial_RxPacket);// 处理串口收到的键盘数据
//	            HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_8);//LED指示灯闪烁
	            break;
	        case 0x02: // 鼠标
	            processMouseCommand(Serial_RxPacket);// 处理串口收到的数据
//	        	HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_8);//LED指示灯闪烁
	            break;
	        case 0x03: // 蜂鸣器
	        	HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_8);//LED指示灯闪烁
	        	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);
	        	HAL_Delay(30);  // 响30ms
	        	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);
	        	break;
	        default:
	            break;
	      }
		  /*串口发送命令执行完成数据包，原命令数据返回*/
		  memcpy(Serial_TxPacket, Serial_RxPacket, 10);//包有效数据长度为10，0-9
		  Serial_SendPacket();		//串口发送数据包Serial_TxPacket

      }

	  // HAL_Delay(10);  // 延时

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USB;
  PeriphClkInit.UsbClockSelection = RCC_USBCLKSOURCE_PLL_DIV1_5;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
//OLED 初始化代码
void OLED_InitDisplay() {
	OLED_Init();  // 初始化OLED显示

    // 初始显示标签
	OLED_ShowString(1, 1, "S H:     V:");
    OLED_ShowString(2, 1, "B L    R    M");
    OLED_ShowString(3, 1, "W");
    OLED_ShowString(4, 1, "K");
}

// 更新鼠标滚动值
void OLED_UpdateScroll(int hScroll, int vScroll) {
	if (hScroll == 0){
		OLED_ShowString(1, 5, "     ");
	}else{
		OLED_ShowSignedNum(1, 5, hScroll, 3); // 更新水平滚动值，宽度3
	}
	if (vScroll == 0){
		OLED_ShowString(1, 12, "     ");
	}else{
		OLED_ShowSignedNum(1, 12, vScroll, 3); // 更新垂直滚动值，宽度3
	}
}

// 更新鼠标按键状态
void OLED_UpdateButtons(int btnLeft, int btnRight, int btnMiddle) {
	OLED_ShowChar(2, 5, btnLeft ? '*' : ' ');  // 更新左键状态
    OLED_ShowChar(2, 10, btnRight ? '*' : ' '); // 更新右键状态
    OLED_ShowChar(2, 15, btnMiddle ? '*' : ' '); // 更新中键状态
}

// 更新鼠标滚轮状态
void OLED_UpdateWheel(int wheel) {
	if (wheel == 0){
		OLED_ShowString(3, 3, "    ");
	}else{
		OLED_ShowSignedNum(3, 3, wheel, 3); // 更新滚轮滚动值，宽度3
	}
}

// 更新键盘按键（显示组合键）
void OLED_UpdateKey(uint8_t modifier, uint8_t key1, uint8_t key2) {
    static char combo_key[64];
    if (modifier != 0) {
        if (key2 != 0) {
            snprintf(combo_key, sizeof(combo_key), "%s+%s+%s", MODIFIER_KEYS[modifier], HID_KEY_NAMES[key1], HID_KEY_NAMES[key2]);
        } else {
            snprintf(combo_key, sizeof(combo_key), "%s+%s", MODIFIER_KEYS[modifier], HID_KEY_NAMES[key1]);
        }
    } else {
        if (key2 != 0) {
            snprintf(combo_key, sizeof(combo_key), "%s+%s", HID_KEY_NAMES[key1], HID_KEY_NAMES[key2]);
        } else {
            snprintf(combo_key, sizeof(combo_key), "%s", HID_KEY_NAMES[key1]);
        }
    }
    OLED_ShowString(4, 3, "             "); // 擦干净
    OLED_ShowString(4, 3, combo_key); // 更新显示键名
}

// 初始化鼠标键盘预设函数
void initializeMouseKeyBoards() {
	  // KeyBoard[0] 是一个固定值，通常用于表示HID报告的ID。对于键盘，这个值一般为1。
	  // KeyBoard[1] 是一个位图，用于表示修饰键（如Ctrl、Shift、Alt等）的状态。
	  // KeyBoard[2] 保留位，一般设为0。
	  // KeyBoard[3] 到 KeyBoard[8] 用于存储按下的按键的键值，最多可以同时表示6个按键的按下状态。
	  // 这样可以在一个HID报告中同时表示多个按键的状态，从而实现组合键的功能。

	// 使用 memset 将 KeyBoard 和 KeyBoard01 的所有元素初始化为 0

	memset(Mouse, 0, sizeof(Mouse));
    memset(KeyBoard, 0, sizeof(KeyBoard));
    memset(KeyBoard01, 0, sizeof(KeyBoard01));

    // 手动设置 KeyBoard[0] 和 KeyBoard01[0] 为 1
    Mouse[0] = 2;
    KeyBoard[0] = 1;
    KeyBoard01[0] = 1;
}

// 处理串口收到的键盘数据
void processKeyboardCommand(uint8_t* packet) {
    if (packet[0] != 0x01) return; // 检查是否为键盘命令

    // KeyBoard[0] 是一个固定值，通常用于表示HID报告的ID。对于键盘，这个值一般为1。
    KeyBoard[1] = packet[1]; // KeyBoard[1] 是一个位图，用于表示修饰键（如Ctrl、Shift、Alt等）的状态。
    // KeyBoard[2] 保留位，一般设为0。
    KeyBoard[3] = packet[3]; // KeyBoard[3] 到 KeyBoard[8] 用于存储按下的按键的键值，最多可以同时表示6个按键的按下状态。
    KeyBoard[4] = packet[4];
    KeyBoard[5] = packet[5];
    KeyBoard[6] = packet[6];
    KeyBoard[7] = packet[7];
    KeyBoard[8] = packet[8];

    OLED_UpdateKey(KeyBoard[1], KeyBoard[3], KeyBoard[4]); // 更新OLED显示组合键
    USBD_HID_SendReport(&hUsbDeviceFS, (uint8_t*)&KeyBoard, sizeof(KeyBoard));
    if (packet[2]!=0) {//有延时，表示需要抬起按键，如果值为0，表示按住不抬起键盘
    	HAL_Delay(packet[2] * 10);  // 延时1等于10毫秒
    	USBD_HID_SendReport(&hUsbDeviceFS, (uint8_t*)&KeyBoard01, sizeof(KeyBoard01));
    }
}

// 处理串口收到的鼠标数据
void processMouseCommand(uint8_t* packet) {
    if (packet[0] != 0x02) return; // 检查是否为鼠标命令

    uint8_t buttonState = packet[1];
    int8_t xMove = (int8_t)packet[2];
    int8_t yMove = (int8_t)packet[3];
    int8_t wheelMove = (int8_t)packet[4];

    // 更新鼠标按键状态
    Mouse[0] = 0x02;
    Mouse[1] = buttonState;
    Mouse[2] = xMove;
    Mouse[3] = yMove;
    Mouse[4] = wheelMove;

    // 发送HID报告
    USBD_HID_SendReport(&hUsbDeviceFS, Mouse, sizeof(Mouse));

    // 更新OLED显示
    OLED_UpdateScroll(xMove, yMove);
    OLED_UpdateButtons(buttonState & 0x01, buttonState & 0x02, buttonState & 0x04);
    OLED_UpdateWheel(wheelMove);
}

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
