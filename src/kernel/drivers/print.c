#include <disp.h>
#include <stdarg.h>

static uint64_t cursor_x = 0;
static uint64_t cursor_y = 0;
static const uint64_t line_height = 24;
static const uint64_t line_width = 24;


//prints individual characters
void putc(char c) {
    if (c == '\n') {
        cursor_y+=line_height;
        cursor_x = 0;
    } else {
        drawChar(cursor_x, cursor_y, c, 0xFFFFFF);
        cursor_x+=line_width;

        if (cursor_x >= fb_info.width) {
            cursor_y+=line_height;
            cursor_x = 0;
        }
    }
}


// Helper function to print a decimal number
void printDec(int num) {
    if (num == 0) {
        putc('0');
        return;
    }

    if (num < 0) { // Handle negative numbers
        putc('-');
        num = -num;
    }

    char buffer[20];
    int i = 0;

    while (num > 0) {
        buffer[i++] = '0' + (num % 10);
        num /= 10;
    }

    while (i > 0) {
        putc(buffer[--i]);
    }
}

// printf implementation with support for %d, %s, %c
void print(const char *format, ...) {
    va_list args;
    va_start(args, format);

    while (*format) {
        if (*format == '%') {
            format++; // Move past '%'
            switch (*format) {
                case 'd': { // Decimal integer
                    int num = va_arg(args, int);
                    printDec(num);
                    break;
                }
                case 's': { // String
                    char *str = va_arg(args, char *);
                    print(str);
                    break;
                }
                case 'c': { // Character
                    char c = (char)va_arg(args, int);
                    putc(c);
                    break;
                }
                case '%': { // Literal '%'
                    putc('%');
                    break;
                }
                default:
                    putc('%');
                    putc(*format); // Print unknown specifier as-is
                    break;
            }
        } else {
            putc(*format); // Regular character, print directly
        }
        format++; // Move to next character
    }

    va_end(args);
}
