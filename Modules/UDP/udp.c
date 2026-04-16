/**
  ******************************************************************************
  * @file           : udp.c
  * @brief          : UDP driver
  * @author         : MicroTechnics (microtechnics.ru)
  ******************************************************************************
  */



/* Includes ------------------------------------------------------------------*/
#include "udp.h"
#include "ip.h"
#include "main.h"

/* Command protocol ----------------------------------------------------------*/
#define CMD_MODE_TEST                ((uint8_t)0x00u)
#define CMD_MODE_MANUAL              ((uint8_t)0x01u)
#define CMD_MODE_PRESET              ((uint8_t)0x02u)

#define TEST_LED_LOW                 ((uint8_t)0x00u)
#define TEST_LED_HIGH                ((uint8_t)0x01u)
#define TEST_LED_BLINK_2             ((uint8_t)0x02u)

#define PRESET_LEFT                  ((uint8_t)0x00u)
#define PRESET_CENTER                ((uint8_t)0x01u)
#define PRESET_RIGHT                 ((uint8_t)0x02u)
#define PRESET_SCAN                  ((uint8_t)0x03u)

/*
 * Masks for CH1..CH8:
 * bit0 -> CH1, bit1 -> CH2, ..., bit7 -> CH8.
 * Подстройте под вашу фактическую схему лучей.
 */
#define CH_MASK_LEFT                 ((uint8_t)0x07u)  /* CH1..CH3 */
#define CH_MASK_CENTER               ((uint8_t)0x18u)  /* CH4..CH5 */
#define CH_MASK_RIGHT                ((uint8_t)0xE0u)  /* CH6..CH8 */

static uint8_t g_scan_state = 0u; /* 0: left, 1: center, 2: right */

static void CH_SetByMask(uint8_t mask)
{
  HAL_GPIO_WritePin(CH1_GPIO_Port, CH1_Pin, (mask & (1u << 0)) ? GPIO_PIN_SET : GPIO_PIN_RESET);
  HAL_GPIO_WritePin(CH2_GPIO_Port, CH2_Pin, (mask & (1u << 1)) ? GPIO_PIN_SET : GPIO_PIN_RESET);
  HAL_GPIO_WritePin(CH3_GPIO_Port, CH3_Pin, (mask & (1u << 2)) ? GPIO_PIN_SET : GPIO_PIN_RESET);
  HAL_GPIO_WritePin(CH4_GPIO_Port, CH4_Pin, (mask & (1u << 3)) ? GPIO_PIN_SET : GPIO_PIN_RESET);
  HAL_GPIO_WritePin(CH5_GPIO_Port, CH5_Pin, (mask & (1u << 4)) ? GPIO_PIN_SET : GPIO_PIN_RESET);
  HAL_GPIO_WritePin(CH6_GPIO_Port, CH6_Pin, (mask & (1u << 5)) ? GPIO_PIN_SET : GPIO_PIN_RESET);
  HAL_GPIO_WritePin(CH7_GPIO_Port, CH7_Pin, (mask & (1u << 6)) ? GPIO_PIN_SET : GPIO_PIN_RESET);
  HAL_GPIO_WritePin(CH8_GPIO_Port, CH8_Pin, (mask & (1u << 7)) ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

static void LED_TestAction(uint8_t action)
{
  if (action == TEST_LED_LOW)
  {
    HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);
  }
  else if (action == TEST_LED_HIGH)
  {
    HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET);
  }
  else if (action == TEST_LED_BLINK_2)
  {
    for (uint8_t i = 0; i < 2u; i++)
    {
      HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET);
      HAL_Delay(100);
      HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);
      HAL_Delay(100);
    }
  }
}

static void PresetAction(uint8_t preset)
{
  if (preset == PRESET_LEFT)
  {
    CH_SetByMask(CH_MASK_LEFT);
  }
  else if (preset == PRESET_CENTER)
  {
    CH_SetByMask(CH_MASK_CENTER);
  }
  else if (preset == PRESET_RIGHT)
  {
    CH_SetByMask(CH_MASK_RIGHT);
  }
  else if (preset == PRESET_SCAN)
  {
    if (g_scan_state == 0u)
    {
      CH_SetByMask(CH_MASK_LEFT);
    }
    else if (g_scan_state == 1u)
    {
      CH_SetByMask(CH_MASK_CENTER);
    }
    else
    {
      CH_SetByMask(CH_MASK_RIGHT);
    }

    g_scan_state++;
    if (g_scan_state >= 3u)
    {
      g_scan_state = 0u;
    }
  }
}

/*----------------------------------------------------------------------------*/
uint16_t UDP_Process(UDP_Frame* udpFrame, uint16_t frameLen)
{
  uint16_t newFrameLen = 0;

  (void)frameLen;

  uint16_t destPort = ntohs(udpFrame->destPort);
  uint16_t len = ntohs(udpFrame->len);
  uint16_t dataLen = len - sizeof(UDP_Frame);

  if ((destPort == UDP_DEMO_PORT) && (dataLen >= 2u))
  {
    uint8_t mode = udpFrame->data[0];
    uint8_t value = udpFrame->data[1];

    if (mode == CMD_MODE_TEST)
    {
      LED_TestAction(value);
    }
    else if (mode == CMD_MODE_MANUAL)
    {
      CH_SetByMask(value);
    }
    else if (mode == CMD_MODE_PRESET)
    {
      PresetAction(value);
    }
    else
    {
      /* Unknown mode: ignore */
    }
  }

  /* Keep current reply behavior */
  {
    uint16_t swapPort = udpFrame->destPort;
    udpFrame->destPort = udpFrame->srcPort;
    udpFrame->srcPort = swapPort;
  }

  udpFrame->checkSum = 0;
  newFrameLen = len;

  return newFrameLen;
}



/*----------------------------------------------------------------------------*/
