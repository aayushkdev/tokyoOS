#include <disp.h>
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

void putc(char c) {
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

void itoa(int num, char *str, int base) {
    int i = 0, isNegative = 0;
    if (num == 0) {
        str[i++] = '0';
        str[i] = '\0';
        return;
    }
    if (num < 0 && base == 10) {
        isNegative = 1;
        num = -num;
    }
    while (num != 0) {
        int rem = num % base;
        str[i++] = (rem > 9) ? (rem - 10) + 'a' : rem + '0';
        num /= base;
    }
    if (isNegative)
        str[i++] = '-';
    str[i] = '\0';
    for (int j = 0, k = i - 1; j < k; j++, k--) {
        char temp = str[j];
        str[j] = str[k];
        str[k] = temp;
    }
}

void vsprintf(void (*outputFunc)(char), const char *format, va_list args) {
    char buffer[32];
    while (*format) {
        if (*format == '%') {
            format++;
            switch (*format) {
                case 'd':
                    itoa(va_arg(args, int), buffer, 10);
                    for (char *p = buffer; *p; p++) outputFunc(*p);
                    break;
                case 'x':
                case 'p':
                    itoa(va_arg(args, unsigned int), buffer, 16);
                    outputFunc('0'); outputFunc('x');
                    for (char *p = buffer; *p; p++) outputFunc(*p);
                    break;
                case 's':
                    for (char *p = va_arg(args, char *); *p; p++) outputFunc(*p);
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



void printf(const char *format, ...) {
    va_list args;
    va_start(args, format);
    vsprintf(putc, format, args);
    va_end(args);
}