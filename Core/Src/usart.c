/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    usart.c
  * @brief   This file provides code for the configuration
  *          of the USART instances.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
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
#include "usart.h"

/* USER CODE BEGIN 0 */
#define USART1_RX_BUFFER_SIZE 128U
#define USART1_RX_BUFFER_MASK (USART1_RX_BUFFER_SIZE - 1U)

static uint8_t uart1_rx_byte;
static volatile uint8_t uart1_rx_buffer[USART1_RX_BUFFER_SIZE];
static volatile uint8_t uart1_rx_head;
static volatile uint8_t uart1_rx_tail;
static uint8_t uart1_echo_buffer[USART1_RX_BUFFER_SIZE];
static volatile uint8_t uart1_echo_head;
static volatile uint8_t uart1_echo_tail;
static volatile uint8_t uart1_echo_busy;

static void USART1_EchoStartNext(void)
{
  if ((uart1_echo_busy == 0U) && (uart1_echo_head != uart1_echo_tail))
  {
    uart1_echo_busy = 1U;
    if (HAL_UART_Transmit_IT(&huart1, &uart1_echo_buffer[uart1_echo_tail], 1U) != HAL_OK)
      uart1_echo_busy = 0U;
  }
}

/* USER CODE END 0 */

UART_HandleTypeDef huart1;

/* USART1 init function */

void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 9600;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

void HAL_UART_MspInit(UART_HandleTypeDef* uartHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(uartHandle->Instance==USART1)
  {
  /* USER CODE BEGIN USART1_MspInit 0 */

  /* USER CODE END USART1_MspInit 0 */
    /* USART1 clock enable */
    __HAL_RCC_USART1_CLK_ENABLE();

    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**USART1 GPIO Configuration
    PA9     ------> USART1_TX
    PA10     ------> USART1_RX
    */
    GPIO_InitStruct.Pin = GPIO_PIN_9;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_10;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    HAL_NVIC_SetPriority(USART1_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(USART1_IRQn);

  /* USER CODE BEGIN USART1_MspInit 1 */

  /* USER CODE END USART1_MspInit 1 */
  }
}

void HAL_UART_MspDeInit(UART_HandleTypeDef* uartHandle)
{

  if(uartHandle->Instance==USART1)
  {
  /* USER CODE BEGIN USART1_MspDeInit 0 */

  /* USER CODE END USART1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_USART1_CLK_DISABLE();

    /**USART1 GPIO Configuration
    PA9     ------> USART1_TX
    PA10     ------> USART1_RX
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_9|GPIO_PIN_10);

  /* USER CODE BEGIN USART1_MspDeInit 1 */

  /* USER CODE END USART1_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */

HAL_StatusTypeDef USART1_ReceiveStart(void)
{
  uart1_rx_head = 0U;
  uart1_rx_tail = 0U;
  uart1_echo_head = 0U;
  uart1_echo_tail = 0U;
  uart1_echo_busy = 0U;
  return HAL_UART_Receive_IT(&huart1, &uart1_rx_byte, 1U);
}

uint8_t USART1_Available(void)
{
  return (uint8_t)((uart1_rx_head - uart1_rx_tail) & USART1_RX_BUFFER_MASK);
}

uint8_t USART1_ReadByte(uint8_t *byte)
{
  if ((byte == NULL) || (uart1_rx_head == uart1_rx_tail))
    return 0U;

  *byte = uart1_rx_buffer[uart1_rx_tail];
  uart1_rx_tail = (uint8_t)((uart1_rx_tail + 1U) & USART1_RX_BUFFER_MASK);
  return 1U;
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *uart)
{
  if (uart->Instance == USART1)
  {
    uint8_t next = (uint8_t)((uart1_rx_head + 1U) & USART1_RX_BUFFER_MASK);

    if (next != uart1_rx_tail)
    {
      uart1_rx_buffer[uart1_rx_head] = uart1_rx_byte;
      uart1_rx_head = next;
    }

    next = (uint8_t)((uart1_echo_head + 1U) & USART1_RX_BUFFER_MASK);
    if (next != uart1_echo_tail)
    {
      uart1_echo_buffer[uart1_echo_head] = uart1_rx_byte;
      uart1_echo_head = next;
      USART1_EchoStartNext();
    }

    (void)HAL_UART_Receive_IT(&huart1, &uart1_rx_byte, 1U);
  }
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *uart)
{
  if (uart->Instance == USART1)
  {
    uart1_echo_tail = (uint8_t)((uart1_echo_tail + 1U) & USART1_RX_BUFFER_MASK);
    uart1_echo_busy = 0U;
    USART1_EchoStartNext();
  }
}

void HAL_UART_ErrorCallback(UART_HandleTypeDef *uart)
{
  if (uart->Instance == USART1)
    (void)HAL_UART_Receive_IT(&huart1, &uart1_rx_byte, 1U);
}

/* USER CODE END 1 */
