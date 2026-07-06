#pragma once

#include <stdarg.h>

void kprintf(const char *format, ...);
void kvprintf(void (*output_func)(char), const char *format, va_list args);
