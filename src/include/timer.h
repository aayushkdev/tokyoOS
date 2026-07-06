#pragma once

#include <stdint.h>

#define TIMER_VECTOR 64

int init_timer(void);
void timer_handle_tick(void);
uint64_t timer_ticks(void);
