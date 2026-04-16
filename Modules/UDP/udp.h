/**
  ******************************************************************************
  * @file           : udp.h
  * @brief          : UDP driver interface
  * @author         : MicroTechnics (microtechnics.ru)
  ******************************************************************************
  */

#ifndef UDP_H
#define UDP_H



/* Includes ------------------------------------------------------------------*/

#include "stm32f1xx_hal.h"
#include "common.h"



/* Declarations and definitions ----------------------------------------------*/

#define UDP_DEMO_PORT                                                   33333                                                

typedef struct UDP_Frame
{
  uint16_t srcPort;
  uint16_t destPort;
  uint16_t len;
  uint16_t checkSum;
  uint8_t data[];
} UDP_Frame;



/* Functions -----------------------------------------------------------------*/

extern uint16_t UDP_Process(UDP_Frame* udpFrame, uint16_t frameLen);



#endif // #ifndef UDP_H
