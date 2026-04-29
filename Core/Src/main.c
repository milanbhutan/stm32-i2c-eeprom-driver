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
#include "EEPROM.h"
#include "delay.h"
#include <stdio.h>
#include <string.h>

#define RETURN_CURSOR "[H"
#define RESTORE_CURSOR "8"
#define CLEAR_SCREEN "[2J"
#define REMOVE_CURSOR_ATTR "[0m"
#define BLINKING_MODE "[5m"


void SystemClock_Config(void);
static void MX_GPIO_Init(void);
void LPUART_setup(void);
void LPUART_ESC_Print(char message[]);
void LPUART_Print(char message[]);

volatile char operation;
volatile char NumBytes[10]="";
volatile char address[10]="";
volatile char data[50];
volatile uint8_t indexToOp;
volatile uint8_t indexToAddr;
volatile uint8_t indexToData;
volatile uint8_t indexToNumBytes;

int main(void)
{
  //configuration
  HAL_Init();
  SystemClock_Config();
  SysTick_Init();
  MX_GPIO_Init();
  EEPROM_init();
  LPUART_setup();

  LPUART_ESC_Print(CLEAR_SCREEN);
  LPUART_ESC_Print(RETURN_CURSOR);

  while(1){
  //clear data buffer
  for(int i;i<indexToData;i++){
	  data[i]='\0';
  }
  //clear NumBytes buffer
  for(int i;i<indexToNumBytes;i++){
	  NumBytes[i]='\0';
  }
  //clear indices
  indexToOp=0;
  indexToAddr=0;
  indexToData=0;
  indexToNumBytes=0;

  char read_word[50]="";
  LPUART_Print("Read or Write Operation?(type R or W):");
  //checks if 1 char has been typed
  while(!(indexToOp==1));
  LPUART_Print("\r\n");
  LPUART_Print("Which address is to be accessed?(0x7FFF-0x0000):");
  //checks if 4 char has been typed
  while(!(indexToAddr==4));
  LPUART_Print("\r\n");
  if(operation=='R'){
	  LPUART_Print("How many characters are being read? (type 'D' to enter value):");
	  //checks if D has been typed
	  while(!(NumBytes[indexToNumBytes-1]=='D'));
	  //clear D char
	  NumBytes[indexToNumBytes-1]='\0';
	  LPUART_Print("\r\n");
	  EEPROM_read_string(read_word,address,NumBytes);
	  LPUART_Print("You read: ");
	  LPUART_Print(read_word);
  	  LPUART_Print("\r\n");
  }
  if(operation=='W'){
	  LPUART_Print("What word is to be written?(type 'D' to enter value):");
	  //checks if 1 char has been typed
	  while(!(data[indexToData-1]=='D'));
	  //clear D char
	  data[indexToData-1]='\0';
	  LPUART_Print("\r\n");
	  EEPROM_write_string(data,address);
	  LPUART_Print("You wrote: ");
	  LPUART_Print(data);
	  LPUART_Print("\r\n");
    }
  }

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

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_MSI;
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
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_MSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
  RCC->CFGR=0;
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
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOG_CLK_ENABLE();
  HAL_PWREx_EnableVddIO2();
  __HAL_RCC_GPIOA_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, LD3_Pin|LD2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(USB_PowerSwitchOn_GPIO_Port, USB_PowerSwitchOn_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : B1_Pin */
  GPIO_InitStruct.Pin = B1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : LD3_Pin LD2_Pin */
  GPIO_InitStruct.Pin = LD3_Pin|LD2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : USB_OverCurrent_Pin */
  GPIO_InitStruct.Pin = USB_OverCurrent_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(USB_OverCurrent_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : USB_PowerSwitchOn_Pin */
  GPIO_InitStruct.Pin = USB_PowerSwitchOn_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(USB_PowerSwitchOn_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : STLK_RX_Pin STLK_TX_Pin */
  GPIO_InitStruct.Pin = STLK_RX_Pin|STLK_TX_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF8_LPUART1;
  HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

  /*Configure GPIO pins : USB_SOF_Pin USB_ID_Pin USB_DM_Pin USB_DP_Pin */
  GPIO_InitStruct.Pin = USB_SOF_Pin|USB_ID_Pin|USB_DM_Pin|USB_DP_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF10_OTG_FS;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
void LPUART_setup(void) {
	// power avail on PG[15:2] (LPUART1)
	PWR->CR2 |= (PWR_CR2_IOSV);
	// enable LPUART clock bridge
	RCC->APB1ENR2 |= RCC_APB1ENR2_LPUART1EN;
	// enable GPIOG clock
	RCC->AHB2ENR |= (RCC_AHB2ENR_GPIOGEN);
	//configure PG7 and PG8 for LPUART
	GPIOG->MODER   &= ~(GPIO_MODER_MODE7 | GPIO_MODER_MODE8);
	GPIOG->MODER   |=  (GPIO_MODER_MODE7_1 | GPIO_MODER_MODE8_1);
	GPIOG->OTYPER  &= ~(GPIO_OTYPER_OT7 | GPIO_OTYPER_OT8);
	GPIOG->PUPDR   &= ~(GPIO_PUPDR_PUPD7 | GPIO_PUPDR_PUPD8);
	GPIOG->OSPEEDR |=  ((3 << GPIO_OSPEEDR_OSPEED7_Pos) | (3 << GPIO_OSPEEDR_OSPEED8_Pos));
	GPIOG->AFR[0] |=(GPIO_AF8_LPUART1<<(4*7));
	GPIOG->AFR[1] |=(GPIO_AF8_LPUART1);
	// 8-bit data
	LPUART1->CR1 &= ~(USART_CR1_M1 | USART_CR1_M0);
	// enable LPUART1
	LPUART1->CR1 |= USART_CR1_UE;
	// enable xmit & recv
	LPUART1->CR1 |= (USART_CR1_TE | USART_CR1_RE);
	// enable LPUART1 recv interrupt
	LPUART1->CR1 |= USART_CR1_RXNEIE;
	// clear Recv-Not-Empty flag
	LPUART1->ISR &= ~(USART_ISR_RXNE);
	// integer divisor which will a baud rate close to 115.2 kbps
	LPUART1->BRR=8889;
	// enable LPUART1 ISR
	NVIC->ISER[2] = (1 << (LPUART1_IRQn & 0x1F));
	// enable global interrupts
	__enable_irq();

}

void LPUART_Print( char message[] ) {
   uint16_t iStrIdx = 0;
   while ( message[iStrIdx] != 0 ) {
	   // wait for empty xmit buffer
      while(!(LPUART1->ISR & USART_ISR_TXE))
         ;
      // send this character
      LPUART1->TDR = message[iStrIdx];
      // advance index to next char
	iStrIdx++;
   }
}

//adds ESC char before message
void LPUART_ESC_Print(char message[]) {
   uint16_t iStrIdx = 0;
   // wait for empty xmit buffer
   while(!(LPUART1->ISR & USART_ISR_TXE))
            ;
   //ESC char added
   LPUART1->TDR ='\x1B';
   while ( message[iStrIdx] != 0 ) {
	   // wait for empty xmit buffer
      while(!(LPUART1->ISR & USART_ISR_TXE))
         ;
      // send this character
      LPUART1->TDR = message[iStrIdx];
      // advance index to next char
	iStrIdx++;
   }
}

void LPUART1_IRQHandler( void  ) {
   uint8_t charRecv;
   if (LPUART1->ISR & USART_ISR_RXNE) {
	  charRecv = LPUART1->RDR;
      if(indexToOp==0){
	  operation=charRecv;
	  indexToOp++;
      }
      else if(indexToAddr<4){
	  address[indexToAddr++]=charRecv;
      }
      else if( operation=='R'&&!(NumBytes[indexToNumBytes-1]=='D')){
      NumBytes[indexToNumBytes++]=charRecv;
      }
      else if(operation=='W'&&!(data[indexToData-1]=='D')){
	  data[indexToData++]=charRecv;
      }
      // wait for empty TX buffer
	  while( !(LPUART1->ISR & USART_ISR_TXE) )
               ;
      // echo char to terminal
		LPUART1->TDR = charRecv;
	}
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
