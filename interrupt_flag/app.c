/***************************************************************************//**
 * @file
 * @brief Top level application functions
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement. This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/
#include "em_timer.h"
#include "app_log.h"
#include "em_cmu.h"

bool timerExpired = false;

/***************************************************************************//**
 * Initialize application.
 ******************************************************************************/
void app_init(void)
{
  app_log_info("Init start\n");
  CMU_ClockEnable(cmuClock_TIMER4, true);
  TIMER_Init_TypeDef timerInit = TIMER_INIT_DEFAULT;
  timerInit.prescale = timerPrescale1024;
  timerInit.enable = false;
  timerInit.oneShot = true;
  TIMER_Init(TIMER4, &timerInit);

  TIMER_InitCC_TypeDef timerCCInit = TIMER_INITCC_DEFAULT;
  timerCCInit.mode = timerCCModeCompare;
  TIMER_InitCC(TIMER4, 0, &timerCCInit);
  TIMER_CompareSet(TIMER4, 0, 30000);

  TIMER_IntEnable(TIMER4, TIMER_IEN_CC0);
  NVIC_EnableIRQ(TIMER4_IRQn);

  TIMER_Enable(TIMER4, true);
  app_log_info("Init done\n");
}

/***************************************************************************//**
 * App ticking function.
 ******************************************************************************/
void app_process_action(void)
{
  while (!timerExpired )
    ;
  timerExpired = false;
  app_log_info("Timer expired\n");
}

void TIMER4_IRQHandler(void){
  uint32_t flags = TIMER_IntGet(TIMER4);
  TIMER_IntClear(TIMER4, flags);

  timerExpired = true;
}
