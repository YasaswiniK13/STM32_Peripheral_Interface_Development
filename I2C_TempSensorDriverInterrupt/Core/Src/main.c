/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
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

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "FreeRTOS.h"
#include "task.h"
#include <string.h>
#include "queue.h"
#include "stm32l475e_iot01_tsensor.h"
#include <math.h>
#include <stdio.h>
#include "hts221.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
TaskHandle_t pSOS_Transmit;
TickType_t LastWakeTime;
TaskHandle_t pUART_SendMsg;
uint8_t repeat;
uint8_t cmd;
QueueHandle_t pUARTQueue;
uint8_t receiveBuf;
TaskHandle_t pTempSensorRead;
int readComplete = 0;
int writeComplete = 0;

char msg[] = "= = = = > Temperature sensor HT221 initialized \r\n";
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc1;

DFSDM_Channel_HandleTypeDef hdfsdm1_channel2;

I2C_HandleTypeDef hi2c1;
I2C_HandleTypeDef hi2c2;

OSPI_HandleTypeDef hospi1;

SPI_HandleTypeDef hspi1;
SPI_HandleTypeDef hspi3;

UART_HandleTypeDef huart4;
UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;
UART_HandleTypeDef huart3;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void PeriphCommonClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_ADC1_Init(void);
static void MX_DFSDM1_Init(void);
static void MX_I2C1_Init(void);
static void MX_I2C2_Init(void);
static void MX_OCTOSPI1_Init(void);
static void MX_SPI1_Init(void);
static void MX_SPI3_Init(void);
static void MX_UART4_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_USART3_UART_Init(void);
static void MX_USB_OTG_FS_USB_Init(void);
/* USER CODE BEGIN PFP */
static void MyLED2_init(void);
static void MyButton_init(void);
static void SOS_Transmit(void *parameter);
static void UART_SendMsg(void *parameter);
static void MyUART1_init(void);
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);
static void TempSensorRead(void *parameter);
static void MyTSENSOR_init();
static float MyTSENSOR_ReadTemp();
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
BaseType_t flag;
BaseType_t flag_uart;
BaseType_t flag_tempSensor;
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

/* Configure the peripherals common clocks */
  PeriphCommonClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_ADC1_Init();
  MX_DFSDM1_Init();
  MX_I2C1_Init();
  MX_I2C2_Init();
  MX_OCTOSPI1_Init();
  MX_SPI1_Init();
  MX_SPI3_Init();
  MX_UART4_Init();
  MX_USART1_UART_Init();
  MX_USART2_UART_Init();
  MX_USART3_UART_Init();
  MX_USB_OTG_FS_USB_Init();
  /* USER CODE BEGIN 2 */
 MyLED2_init();
 MyButton_init();
 MyUART1_init();
 //BSP_TSENSOR_Init();


 HAL_UART_Transmit(&huart1, (uint8_t *)msg, strlen(msg), 1000);

 flag = xTaskCreate(SOS_Transmit, "SOS_Transmit", 200, (void *) 300, 3, &pSOS_Transmit);
 if(flag!=pdPASS)
 {
	 Error_Handler();
 }
 LastWakeTime=xTaskGetTickCount();
/*
 flag_uart = xTaskCreate(UART_SendMsg, "UART_SendMsg", 200, "\rS\n Send SOS with LEDs!!! Please enter 1,2 or 3 for number of repeats:", 3, &pUART_SendMsg);
 if(flag_uart!=pdPASS)
 {
	 Error_Handler();
 }

 HAL_UART_Receive_IT(&huart1, &receiveBuf, 1);
 pUARTQueue = xQueueCreate(10, sizeof(uint8_t));
*/

 flag_tempSensor = xTaskCreate(TempSensorRead, "TempSensorRead", 200, "\r\n Temperature Sensor Reading!!!", 3, &pTempSensorRead);
 if(flag_tempSensor!=pdPASS)
  {
 	 Error_Handler();
  }
 //start Scheduler
 vTaskStartScheduler();

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	/*  while(SOS_transmission==1)
	  {
		  for(int j=0; j<3; j++)
		  {

	        //sending 's'
	        for(int i=0; i<3; i++)
	           {
		        //turning up the led for a dot
		        //HAL_GPIO_WritePin(GPIOB, LED2_Pin, GPIO_PIN_SET); or
		        //write a 1 to Bit 14 of GPIOB ODR(output data register)
		          GPIOB->ODR |=(1<<14);
		          HAL_Delay(300);
	            //turning the led off for spacing
		        //HAL_GPIO_WritePin(GPIOB, LED2_Pin, GPIO_PIN_RESET); or
		        //write a 0 to Bit 14 of GPIOB ODR(output data register)
		          GPIOB->ODR &=~(1<<14);
                  HAL_Delay(300);
	            }
	         //3 time units between characters
	         HAL_Delay(300*2);

	         //sending 'o'
	         for(int i=0; i<3; i++)
	 	       {
	 		    //turning up the led for a dash
	 		    //HAL_GPIO_WritePin(GPIOB, LED2_Pin, GPIO_PIN_SET);
		        //write a 1 to Bit 14 of GPIOB ODR(output data register)
		  	    GPIOB->ODR |=(1<<14);
	 		    HAL_Delay(300*3);
	 	        //turning the led off for spacing
	 		    //HAL_GPIO_WritePin(GPIOB, LED2_Pin, GPIO_PIN_RESET);
	 		    //write a 0 to Bit 14 of GPIOB ODR(output data register)
	 		    GPIOB->ODR &=~(1<<14);
	 		    HAL_Delay(300);
	 	       }
	 	     //3 time units between characters
	 	     HAL_Delay(300*2);
	         //sending 's'
	         for(int i=0; i<3; i++)
	 		  {
	 			  //turning up the led for a dot
	 			  //HAL_GPIO_WritePin(GPIOB, LED2_Pin, GPIO_PIN_SET);
		         //write a 1 to Bit 14 of GPIOB ODR(output data register)
		  		  GPIOB->ODR |=(1<<14);
		          HAL_Delay(300);
	 		      //turning the led off for spacing
	 			  //HAL_GPIO_WritePin(GPIOB, LED2_Pin, GPIO_PIN_RESET);
		          //write a 0 to Bit 14 of GPIOB ODR(output data register)
		          GPIOB->ODR &=~(1<<14);
		          HAL_Delay(300);
	 		  }
	         //7 time units between words
	 	     HAL_Delay(300*6);
          }
		  SOS_transmission=0;
       }*/
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

  /** Configure the main internal regulator output voltage
  */
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure LSE Drive Capability
  */
  HAL_PWR_EnableBkUpAccess();
  __HAL_RCC_LSEDRIVE_CONFIG(RCC_LSEDRIVE_LOW);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_LSE
                              |RCC_OSCILLATORTYPE_MSI;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.MSIState = RCC_MSI_ON;
  RCC_OscInitStruct.MSICalibrationValue = 0;
  RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_6;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV8;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }

  /** Enable MSI Auto calibration
  */
  HAL_RCCEx_EnableMSIPLLMode();
}

/**
  * @brief Peripherals Common Clock Configuration
  * @retval None
  */
void PeriphCommonClock_Config(void)
{
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the peripherals clock
  */
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USB|RCC_PERIPHCLK_ADC;
  PeriphClkInit.AdcClockSelection = RCC_ADCCLKSOURCE_PLLSAI1;
  PeriphClkInit.UsbClockSelection = RCC_USBCLKSOURCE_PLLSAI1;
  PeriphClkInit.PLLSAI1.PLLSAI1Source = RCC_PLLSOURCE_MSI;
  PeriphClkInit.PLLSAI1.PLLSAI1M = 1;
  PeriphClkInit.PLLSAI1.PLLSAI1N = 24;
  PeriphClkInit.PLLSAI1.PLLSAI1P = RCC_PLLP_DIV2;
  PeriphClkInit.PLLSAI1.PLLSAI1Q = RCC_PLLQ_DIV2;
  PeriphClkInit.PLLSAI1.PLLSAI1R = RCC_PLLR_DIV2;
  PeriphClkInit.PLLSAI1.PLLSAI1ClockOut = RCC_PLLSAI1_48M2CLK|RCC_PLLSAI1_ADC1CLK;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief ADC1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC1_Init(void)
{

  /* USER CODE BEGIN ADC1_Init 0 */

  /* USER CODE END ADC1_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC1_Init 1 */

  /* USER CODE END ADC1_Init 1 */

  /** Common config
  */
  hadc1.Instance = ADC1;
  hadc1.Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV1;
  hadc1.Init.Resolution = ADC_RESOLUTION_12B;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.ScanConvMode = ADC_SCAN_DISABLE;
  hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  hadc1.Init.LowPowerAutoWait = DISABLE;
  hadc1.Init.ContinuousConvMode = DISABLE;
  hadc1.Init.NbrOfConversion = 1;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc1.Init.DMAContinuousRequests = DISABLE;
  hadc1.Init.Overrun = ADC_OVR_DATA_PRESERVED;
  hadc1.Init.OversamplingMode = DISABLE;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_1;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLETIME_2CYCLES_5;
  sConfig.SingleDiff = ADC_SINGLE_ENDED;
  sConfig.OffsetNumber = ADC_OFFSET_NONE;
  sConfig.Offset = 0;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC1_Init 2 */

  /* USER CODE END ADC1_Init 2 */

}

/**
  * @brief DFSDM1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_DFSDM1_Init(void)
{

  /* USER CODE BEGIN DFSDM1_Init 0 */

  /* USER CODE END DFSDM1_Init 0 */

  /* USER CODE BEGIN DFSDM1_Init 1 */

  /* USER CODE END DFSDM1_Init 1 */
  hdfsdm1_channel2.Instance = DFSDM1_Channel2;
  hdfsdm1_channel2.Init.OutputClock.Activation = ENABLE;
  hdfsdm1_channel2.Init.OutputClock.Selection = DFSDM_CHANNEL_OUTPUT_CLOCK_SYSTEM;
  hdfsdm1_channel2.Init.OutputClock.Divider = 2;
  hdfsdm1_channel2.Init.Input.Multiplexer = DFSDM_CHANNEL_EXTERNAL_INPUTS;
  hdfsdm1_channel2.Init.Input.DataPacking = DFSDM_CHANNEL_STANDARD_MODE;
  hdfsdm1_channel2.Init.Input.Pins = DFSDM_CHANNEL_SAME_CHANNEL_PINS;
  hdfsdm1_channel2.Init.SerialInterface.Type = DFSDM_CHANNEL_SPI_RISING;
  hdfsdm1_channel2.Init.SerialInterface.SpiClock = DFSDM_CHANNEL_SPI_CLOCK_INTERNAL;
  hdfsdm1_channel2.Init.Awd.FilterOrder = DFSDM_CHANNEL_FASTSINC_ORDER;
  hdfsdm1_channel2.Init.Awd.Oversampling = 1;
  hdfsdm1_channel2.Init.Offset = 0;
  hdfsdm1_channel2.Init.RightBitShift = 0x00;
  if (HAL_DFSDM_ChannelInit(&hdfsdm1_channel2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN DFSDM1_Init 2 */

  /* USER CODE END DFSDM1_Init 2 */

}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.Timing = 0x00000E14;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Analogue filter
  */
  if (HAL_I2CEx_ConfigAnalogFilter(&hi2c1, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Digital filter
  */
  if (HAL_I2CEx_ConfigDigitalFilter(&hi2c1, 0) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief I2C2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C2_Init(void)
{

  /* USER CODE BEGIN I2C2_Init 0 */

  /* USER CODE END I2C2_Init 0 */

  /* USER CODE BEGIN I2C2_Init 1 */

  /* USER CODE END I2C2_Init 1 */
  hi2c2.Instance = I2C2;
  hi2c2.Init.Timing = 0x00000E14;
  hi2c2.Init.OwnAddress1 = 0;
  hi2c2.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c2.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c2.Init.OwnAddress2 = 0;
  hi2c2.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
  hi2c2.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c2.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c2) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Analogue filter
  */
  if (HAL_I2CEx_ConfigAnalogFilter(&hi2c2, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Digital filter
  */
  if (HAL_I2CEx_ConfigDigitalFilter(&hi2c2, 0) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C2_Init 2 */

  /* USER CODE END I2C2_Init 2 */

}

/**
  * @brief OCTOSPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_OCTOSPI1_Init(void)
{

  /* USER CODE BEGIN OCTOSPI1_Init 0 */

  /* USER CODE END OCTOSPI1_Init 0 */

  OSPIM_CfgTypeDef OSPIM_Cfg_Struct = {0};

  /* USER CODE BEGIN OCTOSPI1_Init 1 */

  /* USER CODE END OCTOSPI1_Init 1 */
  /* OCTOSPI1 parameter configuration*/
  hospi1.Instance = OCTOSPI1;
  hospi1.Init.FifoThreshold = 1;
  hospi1.Init.DualQuad = HAL_OSPI_DUALQUAD_DISABLE;
  hospi1.Init.MemoryType = HAL_OSPI_MEMTYPE_MACRONIX;
  hospi1.Init.DeviceSize = 32;
  hospi1.Init.ChipSelectHighTime = 1;
  hospi1.Init.FreeRunningClock = HAL_OSPI_FREERUNCLK_DISABLE;
  hospi1.Init.ClockMode = HAL_OSPI_CLOCK_MODE_0;
  hospi1.Init.ClockPrescaler = 1;
  hospi1.Init.SampleShifting = HAL_OSPI_SAMPLE_SHIFTING_NONE;
  hospi1.Init.DelayHoldQuarterCycle = HAL_OSPI_DHQC_DISABLE;
  hospi1.Init.ChipSelectBoundary = 0;
  hospi1.Init.DelayBlockBypass = HAL_OSPI_DELAY_BLOCK_BYPASSED;
  if (HAL_OSPI_Init(&hospi1) != HAL_OK)
  {
    Error_Handler();
  }
  OSPIM_Cfg_Struct.ClkPort = 1;
  OSPIM_Cfg_Struct.NCSPort = 1;
  OSPIM_Cfg_Struct.IOLowPort = HAL_OSPIM_IOPORT_1_LOW;
  if (HAL_OSPIM_Config(&hospi1, &OSPIM_Cfg_Struct, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN OCTOSPI1_Init 2 */

  /* USER CODE END OCTOSPI1_Init 2 */

}

/**
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_4BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_4;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 7;
  hspi1.Init.CRCLength = SPI_CRC_LENGTH_DATASIZE;
  hspi1.Init.NSSPMode = SPI_NSS_PULSE_ENABLE;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

}

/**
  * @brief SPI3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI3_Init(void)
{

  /* USER CODE BEGIN SPI3_Init 0 */

  /* USER CODE END SPI3_Init 0 */

  /* USER CODE BEGIN SPI3_Init 1 */

  /* USER CODE END SPI3_Init 1 */
  /* SPI3 parameter configuration*/
  hspi3.Instance = SPI3;
  hspi3.Init.Mode = SPI_MODE_MASTER;
  hspi3.Init.Direction = SPI_DIRECTION_2LINES;
  hspi3.Init.DataSize = SPI_DATASIZE_4BIT;
  hspi3.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi3.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi3.Init.NSS = SPI_NSS_SOFT;
  hspi3.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_4;
  hspi3.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi3.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi3.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi3.Init.CRCPolynomial = 7;
  hspi3.Init.CRCLength = SPI_CRC_LENGTH_DATASIZE;
  hspi3.Init.NSSPMode = SPI_NSS_PULSE_ENABLE;
  if (HAL_SPI_Init(&hspi3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI3_Init 2 */

  /* USER CODE END SPI3_Init 2 */

}

/**
  * @brief UART4 Initialization Function
  * @param None
  * @retval None
  */
static void MX_UART4_Init(void)
{

  /* USER CODE BEGIN UART4_Init 0 */

  /* USER CODE END UART4_Init 0 */

  /* USER CODE BEGIN UART4_Init 1 */

  /* USER CODE END UART4_Init 1 */
  huart4.Instance = UART4;
  huart4.Init.BaudRate = 115200;
  huart4.Init.WordLength = UART_WORDLENGTH_8B;
  huart4.Init.StopBits = UART_STOPBITS_1;
  huart4.Init.Parity = UART_PARITY_NONE;
  huart4.Init.Mode = UART_MODE_TX_RX;
  huart4.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart4.Init.OverSampling = UART_OVERSAMPLING_16;
  huart4.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart4.Init.ClockPrescaler = UART_PRESCALER_DIV1;
  huart4.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart4) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetTxFifoThreshold(&huart4, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetRxFifoThreshold(&huart4, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_DisableFifoMode(&huart4) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN UART4_Init 2 */

  /* USER CODE END UART4_Init 2 */

}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  huart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart1.Init.ClockPrescaler = UART_PRESCALER_DIV1;
  huart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetTxFifoThreshold(&huart1, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetRxFifoThreshold(&huart1, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_DisableFifoMode(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_RTS_CTS;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  huart2.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart2.Init.ClockPrescaler = UART_PRESCALER_DIV1;
  huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetTxFifoThreshold(&huart2, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetRxFifoThreshold(&huart2, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_DisableFifoMode(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief USART3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART3_UART_Init(void)
{

  /* USER CODE BEGIN USART3_Init 0 */

  /* USER CODE END USART3_Init 0 */

  /* USER CODE BEGIN USART3_Init 1 */

  /* USER CODE END USART3_Init 1 */
  huart3.Instance = USART3;
  huart3.Init.BaudRate = 115200;
  huart3.Init.WordLength = UART_WORDLENGTH_8B;
  huart3.Init.StopBits = UART_STOPBITS_1;
  huart3.Init.Parity = UART_PARITY_NONE;
  huart3.Init.Mode = UART_MODE_TX_RX;
  huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart3.Init.OverSampling = UART_OVERSAMPLING_16;
  huart3.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart3.Init.ClockPrescaler = UART_PRESCALER_DIV1;
  huart3.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart3) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetTxFifoThreshold(&huart3, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetRxFifoThreshold(&huart3, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_DisableFifoMode(&huart3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART3_Init 2 */

  /* USER CODE END USART3_Init 2 */

}

/**
  * @brief USB_OTG_FS Initialization Function
  * @param None
  * @retval None
  */
static void MX_USB_OTG_FS_USB_Init(void)
{

  /* USER CODE BEGIN USB_OTG_FS_Init 0 */

  /* USER CODE END USB_OTG_FS_Init 0 */

  /* USER CODE BEGIN USB_OTG_FS_Init 1 */

  /* USER CODE END USB_OTG_FS_Init 1 */
  /* USER CODE BEGIN USB_OTG_FS_Init 2 */

  /* USER CODE END USB_OTG_FS_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOE, ST25DV04K_RF_DISABLE_Pin|ISM43362_RST_Pin|ISM43362_SPI3_CSN_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, ARD_D10_Pin|ARD_D4_Pin|ARD_D7_Pin|SPBTLE_RF_RST_Pin
                          |ARD_D9_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, ARD_D8_Pin|ISM43362_BOOT0_Pin|ISM43362_WAKEUP_Pin|LED2_Pin
                          |SPSGRF_915_SDN_Pin|ARD_D5_Pin|SPSGRF_915_SPI3_CSN_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOD, SPBTLE_RF_SPI3_CSN_Pin|PMOD_RESET_Pin|PMOD_SPI2_SCK_Pin|STSAFE_A110_RESET_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, VL53L0X_XSHUT_Pin|LED3_WIFI__LED4_BLE_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : ST25DV04K_RF_DISABLE_Pin ISM43362_RST_Pin ISM43362_SPI3_CSN_Pin */
  GPIO_InitStruct.Pin = ST25DV04K_RF_DISABLE_Pin|ISM43362_RST_Pin|ISM43362_SPI3_CSN_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /*Configure GPIO pins : USB_OTG_FS_OVRCR_EXTI3_Pin ST25DV04K_GPO_Pin SPSGRF_915_GPIO3_EXTI5_Pin SPBTLE_RF_IRQ_EXTI6_Pin
                           ISM43362_DRDY_EXTI1_Pin */
  GPIO_InitStruct.Pin = USB_OTG_FS_OVRCR_EXTI3_Pin|ST25DV04K_GPO_Pin|SPSGRF_915_GPIO3_EXTI5_Pin|SPBTLE_RF_IRQ_EXTI6_Pin
                          |ISM43362_DRDY_EXTI1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /*Configure GPIO pins : BUTTON_EXTI13_Pin VL53L0X_GPIO1_EXTI7_Pin LSM3MDL_DRDY_EXTI8_Pin */
  GPIO_InitStruct.Pin = BUTTON_EXTI13_Pin|VL53L0X_GPIO1_EXTI7_Pin|LSM3MDL_DRDY_EXTI8_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : ARD_D10_Pin ARD_D4_Pin ARD_D7_Pin SPBTLE_RF_RST_Pin
                           ARD_D9_Pin */
  GPIO_InitStruct.Pin = ARD_D10_Pin|ARD_D4_Pin|ARD_D7_Pin|SPBTLE_RF_RST_Pin
                          |ARD_D9_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : ARD_D3_Pin */
  GPIO_InitStruct.Pin = ARD_D3_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(ARD_D3_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : ARD_D6_Pin */
  GPIO_InitStruct.Pin = ARD_D6_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Alternate = GPIO_AF2_TIM3;
  HAL_GPIO_Init(ARD_D6_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : ARD_D8_Pin ISM43362_BOOT0_Pin ISM43362_WAKEUP_Pin SPSGRF_915_SDN_Pin
                           ARD_D5_Pin SPSGRF_915_SPI3_CSN_Pin */
  GPIO_InitStruct.Pin = ARD_D8_Pin|ISM43362_BOOT0_Pin|ISM43362_WAKEUP_Pin|SPSGRF_915_SDN_Pin
                          |ARD_D5_Pin|SPSGRF_915_SPI3_CSN_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : LED2_Pin */
  GPIO_InitStruct.Pin = LED2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(LED2_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : LPS22HB_INT_DRDY_EXTI10_Pin LSM6DSL_INT1_EXTI11_Pin USB_OTG_FS_PWR_EN_Pin ARD_D2_Pin
                           HTS221_DRDY_EXTI15_Pin PMOD_IRQ_EXTI2_Pin */
  GPIO_InitStruct.Pin = LPS22HB_INT_DRDY_EXTI10_Pin|LSM6DSL_INT1_EXTI11_Pin|USB_OTG_FS_PWR_EN_Pin|ARD_D2_Pin
                          |HTS221_DRDY_EXTI15_Pin|PMOD_IRQ_EXTI2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /*Configure GPIO pins : SPBTLE_RF_SPI3_CSN_Pin PMOD_RESET_Pin PMOD_SPI2_SCK_Pin STSAFE_A110_RESET_Pin */
  GPIO_InitStruct.Pin = SPBTLE_RF_SPI3_CSN_Pin|PMOD_RESET_Pin|PMOD_SPI2_SCK_Pin|STSAFE_A110_RESET_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /*Configure GPIO pins : VL53L0X_XSHUT_Pin LED3_WIFI__LED4_BLE_Pin */
  GPIO_InitStruct.Pin = VL53L0X_XSHUT_Pin|LED3_WIFI__LED4_BLE_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pin : USB_OTG_FS_VBUS_Pin */
  GPIO_InitStruct.Pin = USB_OTG_FS_VBUS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(USB_OTG_FS_VBUS_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : USB_OTG_FS_ID_Pin USB_OTG_FS_DM_Pin USB_OTG_FS_DP_Pin */
  GPIO_InitStruct.Pin = USB_OTG_FS_ID_Pin|USB_OTG_FS_DM_Pin|USB_OTG_FS_DP_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF10_OTG_FS;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI9_5_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);

  HAL_NVIC_SetPriority(EXTI15_10_IRQn, 6, 0);
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
static void MyLED2_init(void)
{
  //Configure the MODE register
  //01:general purpose output
  //MODER Bit 29 and Bit 28
  //Reset both bits 28 and 29 to 0
	GPIOB->MODER &=~(0x3 <<28);
  //Set the values to 1
	GPIOB->MODER |=(0x1 <<28);

  //Configure the output type register
  //0:Push-Pull
  //OTYPER Bit 14 to 0
	GPIOB->OTYPER &=~(1<<14);

  //Configure the speed register
  //11:Very high speed
  //OSPEEDR Bit 29 and Bit 28
  //Reset both bits 28 and 29 to 0
	GPIOB->OSPEEDR &=~(0x3 <<28);
  //Set the values to 1
	GPIOB->OSPEEDR |=(0x1 <<28);

  //Configure pull-up/pull-down register
  //00: no pull-up/pull-down
  //PUPDR Bit 29 and Bit 28
  //Reset both bits 28 and 29 to 0
	GPIOB->PUPDR &=~(0x3 <<28);
  //Set the values to 0
	GPIOB->PUPDR |=(0x0 <<28);

}

static void MyButton_init(void)
{
  //Button is Port C, Pin 13, EXTI13
  //Configure SYSCLK
	__HAL_RCC_SYSCFG_CLK_ENABLE();

  //Configure the MODE register
  //00:input mode
  //MODER Bit 27 and Bit 26
  //Reset both bits 27 and 26 to 0
	GPIOC->MODER &=~(0x3 <<26);

  //Configure pull-up/pull-down register
  //00: no pull-up/pull-down
  //PUPDR Bit 27 and Bit 26
  //Reset both bits 27 and 26 to 0
	GPIOC->PUPDR &=~(0x3 <<26);
  //Set the values to 0
	GPIOC->PUPDR |=(0x0 <<26);

  //sysconfig Register
  //EXTI13, Choose port C
  //Configure the External Interrupt Configuration Register
  //SYSCFG_EXTICR4 :only 2nd bit in EXTICR[3] is set to 1
	SYSCFG->EXTICR[3]|=(0x1<<5);

  //Configure the falling trigger selection register
  //EXTI_FTSR
	EXTI->FTSR1 |=(0x1 <<13) ;

  //Configure the Interrupt Mask Register
  //EXTI_IMR
	EXTI->IMR1 |=(0x1 <<13);

  //Configure NVIC Set Enable Register
  //NVIC_ISER
  //NVIC->ISER[] Register no. & location based on position
  //NVIC->ISER[(((uint32_t)EXTI15_10_IRQn) >> 5UL)] = (uint32_t)(1UL << (((uint32_t)EXTI15_10_IRQn) & 0x1FUL));
	NVIC->ISER[1] |= (0x1 << 8);
}

static void MyUART1_init(void)
{
	//configure UART1 manually excluding Baud Rate Register
	//configuring M bits-Bits 12,28 to 0,0 for 1 start bit, 8 data bits, n stop bits in USART_CR1 register
	huart1.Instance->CR1 &=~(0x1 <<12);
	huart1.Instance->CR1 &=~(0x1 <<28);
	//configuring Bit 15 to 1 for oversampling by 8 in USART_CR1 register
	huart1.Instance->CR1 |=(0x1 <<15);
	//configuring Bit 10 to 0 to disable parity control in USART_CR1 register
	huart1.Instance->CR1 &=~(0x1 <<10);
	//configuring Bit 0 to 1 to enable UART in USART_CR1 register
	huart1.Instance->CR1 |=(0x1 <<0);
	//configuring Bit 3 to 1 to enable Transmitter in USART_CR1 register
	huart1.Instance->CR1 |=(0x1 <<3);
	//configuring Bit 2 to 1 to enable Receiver in USART_CR1 register
	huart1.Instance->CR1 |=(0x1 <<2);
	//configuring Bits 12,13 to 0,0 for 1 stop bit in USART_CR2 register
	huart1.Instance->CR2 &=(0x3 <<12);

}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	//activate the message transmission once a button is pushed
	xTaskNotifyFromISR(pUART_SendMsg, 0, eNoAction, NULL);
}
static void SOS_Transmit(void *parameter)
{
  repeat = 2;
  while(1)
	  {
	  //activate when user enters number of repeats

		  //for(int j=0; j<3; j++)
	      //while(repeat>0)
	      while(1)
		  {

	        //sending 's'
	        for(int i=0; i<3; i++)
	           {
		        //turning up the led for a dot
		        //HAL_GPIO_WritePin(GPIOB, LED2_Pin, GPIO_PIN_SET); or
		        //write a 1 to Bit 14 of GPIOB ODR(output data register)
		          GPIOB->ODR |=(1<<14);
		          //HAL_Delay(300);
		          vTaskDelay(300);

	            //turning the led off for spacing
		        //HAL_GPIO_WritePin(GPIOB, LED2_Pin, GPIO_PIN_RESET); or
		        //write a 0 to Bit 14 of GPIOB ODR(output data register)
		          GPIOB->ODR &=~(1<<14);
		          vTaskDelay(300);
	            }
	         //3 time units between characters
	         // HAL_Delay(300*2);
	         vTaskDelay(300*2);

	         //sending 'o'
	         for(int i=0; i<3; i++)
	 	       {
	 		    //turning up the led for a dash
	 		    //HAL_GPIO_WritePin(GPIOB, LED2_Pin, GPIO_PIN_SET);
		        //write a 1 to Bit 14 of GPIOB ODR(output data register)
		  	    GPIOB->ODR |=(1<<14);
	 		    //HAL_Delay(300*3);
		  	    vTaskDelay(300*3);
	 	        //turning the led off for spacing
	 		    //HAL_GPIO_WritePin(GPIOB, LED2_Pin, GPIO_PIN_RESET);
	 		    //write a 0 to Bit 14 of GPIOB ODR(output data register)
	 		    GPIOB->ODR &=~(1<<14);
	 		    //HAL_Delay(300);
	 		    vTaskDelay(300);
	 	       }
	 	     //3 time units between characters
	         //HAL_Delay(300*2);
	         vTaskDelay(300*2);
	         //sending 's'
	         for(int i=0; i<3; i++)
	 		  {
	 			  //turning up the led for a dot
	 			  //HAL_GPIO_WritePin(GPIOB, LED2_Pin, GPIO_PIN_SET);
		         //write a 1 to Bit 14 of GPIOB ODR(output data register)
		  		  GPIOB->ODR |=(1<<14);
		       // HAL_Delay(300);
		  		vTaskDelay(300);
	 		      //turning the led off for spacing
	 			  //HAL_GPIO_WritePin(GPIOB, LED2_Pin, GPIO_PIN_RESET);
		          //write a 0 to Bit 14 of GPIOB ODR(output data register)
		          GPIOB->ODR &=~(1<<14);
		        //HAL_Delay(300);
		          vTaskDelay(300);
	 		  }
	         //7 time units between words
	 	     // HAL_Delay(300*6);
	         vTaskDelay(300*6);
             repeat--;
		  }
	      //SOS_transmission=0;
	      xQueueReceive(pUARTQueue,&cmd, portMAX_DELAY);

	      if(cmd-'0'>=1 && cmd-'0'<=3)
	      {
	    	  repeat = cmd - '0';
	      }
	      else
	      {
	    	  repeat = 0;
	      }
      }
  }

static void UART_SendMsg(void *parameter)
{
	while(1)
	{
		HAL_UART_Transmit(&huart1, parameter, strlen(parameter), 1000);
		//wait for button push to reprint
		xTaskNotifyWait(0, 0, NULL, portMAX_DELAY);
	}
}

static void MyTSENSOR_init()
{
	uint8_t tmp;
	//read CTRL_REG1(0x20)
	//Device address:0xBF
	//HAL_I2C_Mem_Read(hi2c, DevAddress, MemAddress, MemAddSize, pData, Size, Timeout);
	//HAL_I2C_Mem_Read(&hi2c2, 0xBF, 0x20, 0x1U, &tmp,1, 1000);

	//Interrupt based API
	//HAL_I2C_Mem_Read_IT(hi2c, DevAddress, MemAddress, MemAddSize, pData, Size)
	HAL_I2C_Mem_Read_IT(&hi2c2, 0xBF, 0x20, 0x1U, &tmp,1);

	//while(readComplete == 0);
	//readComplete = 0;

	vTaskSuspend(pTempSensorRead);

	//configure BDU:1 //BDU: Bit 2
	tmp |= (1<<0x02);

	//Set ODR to 1Hz //ODR:01 //ODR:Bit 1 and 0
	tmp &= ~ 0x03;
	tmp |= (1<<0x00);

	//Activate the device PD:1
	tmp |= (1<<0x07);

	//write CTRL_REG1(0x20)
	//Device address:0xBE
	//HAL_I2C_Mem_Write(hi2c, DevAddress, MemAddress, MemAddSize, pData, Size, Timeout);
	//HAL_I2C_Mem_Write(&hi2c2, 0xBF, 0x20, 0x1U, &tmp,1, 1000);

	//Interrupt based API
	//HAL_I2C_Mem_Write_IT(hi2c, DevAddress, MemAddress, MemAddSize, pData, Size)
	HAL_I2C_Mem_Write_IT(&hi2c2, 0xBF, 0x20, 0x1U, &tmp,1);

	//while(writeComplete == 0);
	//writeComplete = 0;

	vTaskSuspend(pTempSensorRead);
}

static void TempSensorRead(void *parameter)
{
  char tempReading[100];
  float tempValue =0;

  MyTSENSOR_init();

  while(1)
  {
	  //tempValue = BSP_TSENSOR_ReadTemp();
	  tempValue = MyTSENSOR_ReadTemp();
	  int tempValueInt=tempValue;
	  float tempValueFrac=0;

	  tempValueFrac = tempValue - tempValueInt;
	  int tempValueFrac2digits = trunc(tempValueFrac * 100);

	  snprintf(tempReading, 100, "\r\n TEMPERATURE = %1d.%0.2d \n\r", tempValueInt, tempValueFrac2digits);
      HAL_UART_Transmit(&huart1, (uint8_t *)tempReading, strlen(tempReading), 1000);

      vTaskDelay(5000);
  }
}

static float MyTSENSOR_ReadTemp()
{
	int16_t T0_out, T1_out, T_out, T0_degC_x8_u16, T1_degC_x8_u16;
	  int16_t T0_degC, T1_degC;
	  uint8_t buffer[4], tmp;
	  float tmp_f;

	  //SENSOR_IO_ReadMultiple(DeviceAddr, (HTS221_T0_DEGC_X8 | 0x80), buffer, 2);
	  //HTS221_T0_DEGC_X8: Address 32
	  //HAL_I2C_Mem_Read(hi2c, DevAddress, MemAddress, MemAddSize, pData, Size, Timeout);
	  //HAL_I2C_Mem_Read(&hi2c2, 0xBF, 0x32 | 0x80, 0x1U, buffer, 2, 1000);
	  HAL_I2C_Mem_Read_IT(&hi2c2, 0xBF, 0x32 | 0x80, 0x1U, buffer, 2);
	  //while(readComplete == 0);
	  //readComplete = 0;

	  vTaskSuspend(pTempSensorRead);

	  //tmp = SENSOR_IO_Read(DeviceAddr, HTS221_T0_T1_DEGC_H2);
	  //HTS221_T0_T1_DEGC_H2: Address 0x35
	  //HAL_I2C_Mem_Read(hi2c, DevAddress, MemAddress, MemAddSize, pData, Size, Timeout);
	  //HAL_I2C_Mem_Read(&hi2c2, 0xBF, 0x35, 0x1U, &tmp,1, 1000);
	  HAL_I2C_Mem_Read_IT(&hi2c2, 0xBF, 0x35, 0x1U, &tmp,1);
	  //while(readComplete == 0);
	  //readComplete = 0;

	  vTaskSuspend(pTempSensorRead);

	  T0_degC_x8_u16 = (((uint16_t)(tmp & 0x03)) << 8) | ((uint16_t)buffer[0]);
	  T1_degC_x8_u16 = (((uint16_t)(tmp & 0x0C)) << 6) | ((uint16_t)buffer[1]);
	  T0_degC = T0_degC_x8_u16 >> 3;
	  T1_degC = T1_degC_x8_u16 >> 3;

	  //SENSOR_IO_ReadMultiple(DeviceAddr, (HTS221_T0_OUT_L | 0x80), buffer, 4);
	  //HTS221_T0_OUT_L: Address 0x3C
	  //HAL_I2C_Mem_Read(hi2c, DevAddress, MemAddress, MemAddSize, pData, Size, Timeout);
	  //HAL_I2C_Mem_Read(&hi2c2, 0xBF, 0x3C | 0x80, 0x1U, buffer, 4, 1000);
	  HAL_I2C_Mem_Read_IT(&hi2c2, 0xBF, 0x3C | 0x80, 0x1U, buffer, 4);
	  //while(readComplete == 0);
	  //readComplete = 0;

	  vTaskSuspend(pTempSensorRead);

	  T0_out = (((uint16_t)buffer[1]) << 8) | (uint16_t)buffer[0];
	  T1_out = (((uint16_t)buffer[3]) << 8) | (uint16_t)buffer[2];

	  //SENSOR_IO_ReadMultiple(DeviceAddr, (HTS221_TEMP_OUT_L_REG | 0x80), buffer, 2);
      //HTS221_TEMP_OUT_L_REG: Address 0x2A
	  //HAL_I2C_Mem_Read(hi2c, DevAddress, MemAddress, MemAddSize, pData, Size, Timeout);
	  //HAL_I2C_Mem_Read(&hi2c2, 0xBF, 0x2A | 0x80, 0x1U, buffer, 2, 1000);
	  HAL_I2C_Mem_Read_IT(&hi2c2, 0xBF, 0x2A | 0x80, 0x1U, buffer, 2);
	  //while(readComplete == 0);
	  //readComplete = 0;

	  vTaskSuspend(pTempSensorRead);

	  T_out = (((uint16_t)buffer[1]) << 8) | (uint16_t)buffer[0];

	  tmp_f = (float)(T_out - T0_out) * (float)(T1_degC - T0_degC) / (float)(T1_out - T0_out)  +  T0_degC;

	  return tmp_f;
}


void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	/*BaseType_t xHigherPriorityTaskWoken;
	xHigherPriorityTaskWoken = pdFALSE;

    xQueueSendFromISR(pUARTQueue, &receiveBuf, xHigherPriorityTaskWoken);
    HAL_UART_Receive_IT(&huart1, &receiveBuf, 1);*/
}

void HAL_I2C_MemRxCpltCallback(I2C_HandleTypeDef *hi2c)
{
	//readComplete = 1;
	xTaskResumeFromISR(pTempSensorRead);
}

void HAL_I2C_MemTxCpltCallback(I2C_HandleTypeDef *hi2c)
{
	//writeComplete = 1;
	xTaskResumeFromISR(pTempSensorRead);
}

/* USER CODE END 4 */

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM6 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM6) {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */

  /* USER CODE END Callback 1 */
}

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
