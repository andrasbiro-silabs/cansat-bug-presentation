/***************************************************************************//**
 * @file
 * @brief app_process.c
 *******************************************************************************
 * # License
 * <b>Copyright 2024 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * SPDX-License-Identifier: Zlib
 *
 * The licensor of this software is Silicon Laboratories Inc.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 *******************************************************************************
 * # Experimental Quality
 * This code has not been formally tested and is provided as-is. It is not
 * suitable for production environments. In addition, this code will not be
 * maintained and there may be no bug maintenance planned for these resources.
 * Silicon Labs may update projects from time to time.
 ******************************************************************************/

// -----------------------------------------------------------------------------
//                                   Includes
// -----------------------------------------------------------------------------
#include "rail.h"
#include "app_log.h"
#include <stdio.h>
#include "em_gpio.h"

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------

typedef enum {
  S_IDLE,
  S_TX,
  S_TX_WAIT,
  S_TX_COMPLETE,
} state_t;

// -----------------------------------------------------------------------------
//                          Static Function Declarations
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                                Static Variables
// -----------------------------------------------------------------------------


static volatile state_t state = S_TX;

// -----------------------------------------------------------------------------
//                          Public Function Definitions
// -----------------------------------------------------------------------------

/******************************************************************************
 * Application state machine, called infinitely
 *****************************************************************************/
void app_process_action(RAIL_Handle_t rail_handle)
{
  switch( state ){
    case S_TX:
      RAIL_Status_t status = RAIL_StartTx(rail_handle, 20, RAIL_TX_OPTION_RESEND, NULL);
      if ( status == RAIL_STATUS_NO_ERROR ){
          app_log_info("Tx start\n");
          state = S_TX_WAIT;
      } else {
          app_log_info("Tx start fail 0x%04lx\n", status);
      }
      break;
    case S_TX_COMPLETE:
      app_log_info("Tx complete\n");
      state = S_TX;
      break;
    default:
      break;
  }
}


/******************************************************************************
 * RAIL callback, called if a RAIL event occurs
 *****************************************************************************/
void sl_rail_util_on_event(RAIL_Handle_t rail_handle, RAIL_Events_t events)
{
  (void) rail_handle;
  if ( events & RAIL_EVENTS_TX_COMPLETION ){
      state = S_TX_COMPLETE;
  }
}

// -----------------------------------------------------------------------------
//                          Static Function Definitions
// -----------------------------------------------------------------------------
