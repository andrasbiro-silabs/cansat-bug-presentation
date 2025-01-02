# Interrupt flag bug

## Bad

```c
bool timerExpired = false;
```

Bug: Compiler recognizes that no called function sets `timerExpired` to `false`,
so it optimizes it out.

## Good

```c
volatile bool timerExpired = false;
```
