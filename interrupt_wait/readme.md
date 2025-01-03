# Interrupt error handling and race condition bug

## Bad

```c
  RAIL_StartTx(rail_handle, 25, RAIL_TX_OPTION_RESEND, NULL);
  app_log_info("Tx start\n");
  state = S_TX_WAIT;
```

Bug: No error handling on `RAIL_StartTx()`

## Error handling fixed

```c
  RAIL_Status_t status = RAIL_StartTx(rail_handle, 21, RAIL_TX_OPTION_RESEND, NULL);
  if ( status == RAIL_STATUS_NO_ERROR ){
      app_log_info("Tx start\n");
      state = S_TX_WAIT;
  } else {
      app_log_info("Tx start fail 0x%04lx\n", status);
  }
```
Bug: channel 25 does not exist.

## A new error

```c
  RAIL_Status_t status = RAIL_StartTx(rail_handle, 20, RAIL_TX_OPTION_RESEND, NULL);
  if ( status == RAIL_STATUS_NO_ERROR ){
      app_log_info("Tx start\n");
      state = S_TX_WAIT;
  } else {
      app_log_info("Tx start fail 0x%04lx\n", status);
  }
```

## Find the issue with logic analyzer

```c
void app_process_action(RAIL_Handle_t rail_handle)
{
  switch( state ){
    case S_TX:
      RAIL_Status_t status = RAIL_StartTx(rail_handle, 20, RAIL_TX_OPTION_RESEND, NULL);
      GPIO_PinOutSet(gpioPortC, 5);
      if ( status == RAIL_STATUS_NO_ERROR ){
          app_log_info("Tx start\n");
          state = S_TX_WAIT;
          GPIO_PinOutToggle(gpioPortC, 7);
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
      GPIO_PinOutClear(gpioPortC, 5);
  }
}
```

Bug: Tx completes while printing, so `sl_rail_util_on_event()` is called before
the `state` variable is set in the main loop.

## Better

```c
  CORE_DECLARE_IRQ_STATE;
  CORE_ENTER_CRITICAL();
  RAIL_Status_t status = RAIL_StartTx(rail_handle, 20, RAIL_TX_OPTION_RESEND, NULL);
  GPIO_PinOutSet(gpioPortC, 5);
  if ( status == RAIL_STATUS_NO_ERROR ){
      app_log_info("Tx start\n");
      state = S_TX_WAIT;
      GPIO_PinOutToggle(gpioPortC, 7);
  } else {
      app_log_info("Tx start fail 0x%04lx\n", status);
  }
  CORE_EXIT_CRITICAL();
```

Problem: Critical section is way too big, printing from critical section.

## Even better

```c
  CORE_DECLARE_IRQ_STATE;
  CORE_ENTER_CRITICAL();
  RAIL_Status_t status = RAIL_StartTx(rail_handle, 20, RAIL_TX_OPTION_RESEND, NULL);
  GPIO_PinOutSet(gpioPortC, 5);
  if ( status == RAIL_STATUS_NO_ERROR ){
      state = S_TX_WAIT;
      GPIO_PinOutToggle(gpioPortC, 7);
  }
  CORE_EXIT_CRITICAL();
  app_log_info("Tx start 0x%04lx\n", status);
```

Problem: This can be solved without critical sections.

## Good

```c
  state = S_TX_WAIT;
  RAIL_Status_t status = RAIL_StartTx(rail_handle, 20, RAIL_TX_OPTION_RESEND, NULL);
  GPIO_PinOutSet(gpioPortC, 5);
  if ( status != RAIL_STATUS_NO_ERROR ){
      state = S_TX;
  }
  app_log_info("Tx start 0x%04lx\n", status);
```