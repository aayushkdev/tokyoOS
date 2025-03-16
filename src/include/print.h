#pragma once

#include <stdarg.h>

void printf(const char *format, ...);

void vsprintf(void (*outputFunc)(char), const char *format, va_list args);

void clear();
