/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
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
#include "adc.h"
#include "dma.h"
#include "i2c.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "./mq4/mq4.h"
#include "./AHT20/AHT20.h"
#include "./MPU6050/mpu6050.h"
#include "../../SYSTEM/delay/delay.h"
#include "../../SYSTEM/sys/sys.h"
#include "./st25dv/st25dv.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
extern ADC_HandleTypeDef hadc1;

MQ4_HandleTypeDef mq4;
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
#define ADC_NbrOfCHN 3
#define ADC_SizeOfCHN 30
#define ADC_DMA_BUF_SIZE        ADC_NbrOfCHN*ADC_SizeOfCHN         /* ADC DMA采集 BUF大小 */
uint16_t g_adc_dma_buf[ADC_DMA_BUF_SIZE];   /* ADC DMA BUF */
extern uint8_t g_adc_dma_sta;               /* DMA传输状态标志, 0,未完成; 1, 已完成 */
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */
    uint16_t i,j;
    uint16_t adcx;
    uint32_t sum;
    float temp;
    float temperature,humidity;
    char message[50];

    int16_t ax, ay, az, gx, gy, gz, tempu;
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
  MX_DMA_Init();
  MX_ADC1_Init();
  MX_USART1_UART_Init();
  MX_I2C1_Init();
  /* USER CODE BEGIN 2 */
  HAL_DMA_Start_IT(&hdma_adc1, (uint32_t)&ADC1->DR, (uint32_t)&g_adc_dma_buf, 0);	/* 启动DMA，并开启中断 */
  HAL_ADC_Start_DMA(&hadc1, (uint32_t*)&g_adc_dma_buf, 0);           	/* 开启ADC，通过DMA传输结果 */
  adc_dma_enable(ADC_DMA_BUF_SIZE);   /* 启动ADC DMA采集 */
  HAL_StatusTypeDef st = ST25DV_WriteCloudURL();
  if(st != HAL_OK) {
	  printf("ST25DV write failed\r\n");
  }
  AHT20_Init();
  MPU6050_Init();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  if (g_adc_dma_sta == 1)
	  	  {
	  		  /* 循环显示通道4~通道5的结果 */
	  		  for(j = 0; j < ADC_NbrOfCHN; j++)  /* 遍历5个通道 */
	  		  {
	  			  sum = 0; /* 清零 */
	  			  for (i = 0; i < ADC_DMA_BUF_SIZE / ADC_NbrOfCHN; i++)  /* 每个通道采集了10次数据,进行10次累加 */
	  			  {
	  				  sum += g_adc_dma_buf[(ADC_NbrOfCHN * i) + j];      /* 相同通道的转换数据累加 */
	  			  }
	  			  adcx = sum / (ADC_DMA_BUF_SIZE / ADC_NbrOfCHN);        /* 取平均值 */

	  			  /* 显示结果 */
	  			  //lcd_show_xnum(108, 110 + (j * 30), adcx, 4, 12, 0, BLUE);   /* 显示ADCC采样后的原始值 */
	  			  printf("channel%d = %d\r\n",j,adcx);
	  			  temp = (float)adcx * (3.3 / 4096);  /* 获取计算后的带小数的实际电压值，比如3.1111 */
	  			  adcx = temp;                        /* 赋值整数部分给adcx变量，因为adcx为u16整形 */
	  			  //lcd_show_xnum(108, 122 + (j * 30), adcx, 1, 12, 0, BLUE);   /* 显示电压值的整数部分，3.1111的话，这里就是显示3 */

	  			  temp -= adcx;                       /* 把已经显示的整数部分去掉，留下小数部分，比如3.1111-3=0.1111 */
	  			  temp *= 1000;                       /* 小数部分乘以1000，例如：0.1111就转换为111.1，相当于保留三位小数。 */
	  			  //lcd_show_xnum(120, 122 + (j * 30), temp, 3, 12, 0X80, BLUE);/* 显示小数部分（前面转换为了整形显示），这里显示的就是111. */
	  		  }

	  		  g_adc_dma_sta = 0;                      /* 清除DMA采集完成状态标志 */
	  		  adc_dma_enable(ADC_DMA_BUF_SIZE);       /* 启动下一次ADC DMA采集 */
	  	  }
	  AHT20_Read(&temperature, &humidity);									//读取数据
	  sprintf(message,"temp:%.1f , hum:%.1f %%\r\n",temperature, humidity); //组合字符串
	  printf(message);
	  MPU6050_Read(&ax, &ay, &az, &gx, &gy, &gz, &tempu);
	  sprintf(message,"ax:%d, ay:%d, az:%d, gx:%d, gy:%d, gz:%d, tempu:%d\r\n",ax, ay, az, gx, gy, gz, tempu); //组合字符串
	  printf(message);
	  HAL_Delay(1000);
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
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;
  PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV6;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

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
#ifdef USE_FULL_ASSERT
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
