# Timer compare bug

## Bad

```c
  int start = TIMER_CounterGet(TIMER4);

  app_log_info("Wait start\n");
  while ( TIMER_CounterGet(TIMER4) < start + 5000 )
    ;
  app_log_info("Wait done\n");
```

Bug: 16 bit timer compared with 32 bit int
Note: Generally happens much rarer. However, this kind of bug is recognizable
that it tends to be periodic with the timer. E.g., a 32 bit, microsecond timer
overflows every ~72 minutes.

## Still bad

```c
  uint16_t start = TIMER_CounterGet(TIMER4);

  app_log_info("Wait start\n");
  while ( TIMER_CounterGet(TIMER4) < start + 5000 )
    ;
  app_log_info("Wait done\n");
```

Bug: Same as above. Right side is autocasted to int

## Better

```c
  uint16_t start = TIMER_CounterGet(TIMER4);

  app_log_info("Wait start\n");
  while ( TIMER_CounterGet(TIMER4) < (uint16_t)(start + 5000) )
    ;
  app_log_info("Wait done\n");
```

Issue only obvious if delay is increased e.g., to 15000.

Bug: Timers shouldn't be compared like this. E.g., if start is 55000, the right
side of the equitation is 55000+15000=70000, which overflows, resulting 4464.
The left side will be bigger than this between 55000 and 65535, so the loop will
exit early.

## Good

```c
  uint16_t end = TIMER_CounterGet(TIMER4) + 5000;

  app_log_info("Wait start\n");
  while ( (int16_t)(end - TIMER_CounterGet(TIMER4)) > 0 )
    ;
  app_log_info("Wait done\n");
```
