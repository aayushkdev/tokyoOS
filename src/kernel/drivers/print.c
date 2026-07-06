#include <disp.h>
#include <kprintf.h>
#include <stddef.h>
#include <stdarg.h>

static uint64_t cursor_x = 0;
static uint64_t cursor_y = 0;
static const uint64_t line_height = 24;
static const uint64_t line_width = 24;
static const uint64_t max_height = 720;

void newline() {
    cursor_x = 0;
    cursor_y += line_height;
    if (cursor_y >= max_height) {
        cursor_y = max_height - line_height;
    }
}

static void framebuffer_putc(char c) {
    if (c == '\n') {
        newline();
    } else {
        drawChar(cursor_x, cursor_y, c, 0xFFFFFF);
        cursor_x += line_width;
        if (cursor_x >= fb_info.width) {
            newline();
        }
    }
}

static void reverse_string(char *str, int length) {
    for (int j = 0, k = length - 1; j < k; j++, k--) {
        char temp = str[j];
        str[j] = str[k];
        str[k] = temp;
    }
}

static void utoa(uint64_t num, char *str, int base) {
    int i = 0;
    if (num == 0) {
        str[i++] = '0';
        str[i] = '\0';
        return;
    }

    while (num != 0) {
        uint64_t rem = num % base;
        str[i++] = (rem > 9) ? (rem - 10) + 'a' : rem + '0';
        num /= base;
    }

    str[i] = '\0';
    reverse_string(str, i);
}

static void itoa(int64_t num, char *str, int base) {
    if (num < 0 && base == 10) {
        *str++ = '-';
        utoa((uint64_t)-num, str, base);
        return;
    }

    utoa((uint64_t)num, str, base);
}

static void output_string(void (*outputFunc)(char), const char *str) {
    if (str == NULL) {
        str = "(null)";
    }

    for (const char *p = str; *p; p++) {
        outputFunc(*p);
    }
}

void kvprintf(void (*outputFunc)(char), const char *format, va_list args) {
    char buffer[32];
    while (*format) {
        if (*format == '%') {
            format++;
            switch (*format) {
                case 'd':
                    itoa(va_arg(args, int), buffer, 10);
                    output_string(outputFunc, buffer);
                    break;
                case 'u':
                    utoa(va_arg(args, unsigned int), buffer, 10);
                    output_string(outputFunc, buffer);
                    break;
                case 'x':
                    utoa(va_arg(args, unsigned int), buffer, 16);
                    outputFunc('0'); outputFunc('x');
                    output_string(outputFunc, buffer);
                    break;
                case 'p':
                    utoa((uint64_t)va_arg(args, void *), buffer, 16);
                    outputFunc('0'); outputFunc('x');
                    output_string(outputFunc, buffer);
                    break;
                case 's':
                    output_string(outputFunc, va_arg(args, char *));
                    break;
                case 'c':
                    outputFunc((char)va_arg(args, int));
                    break;
                case '%':
                    outputFunc('%');
                    break;
                default:
                    outputFunc('%');
                    outputFunc(*format);
                    break;
            }
        } else {
            outputFunc(*format);
        }
        format++;
    }
}



void kprintf(const char *format, ...) {
    va_list args;
    va_start(args, format);
    kvprintf(framebuffer_putc, format, args);
    va_end(args);
}

void kclear(void) {
    clearScreen(0x000000);
    cursor_x = 0;
    cursor_y = 0;
}

void kbackspace(void) {
    if (cursor_x < line_width) {
        return;
    }

    cursor_x -= line_width;
    fillRect(cursor_x, cursor_y, line_width, line_height, 0x000000);
}
