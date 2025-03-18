#include <serial.h>
#include <stdarg.h>
#include <system.h>
#include <print.h>

static int currentSerial = COM1;

int init_serial(int device) {
    currentSerial = device;
    outb(device + 1, 0x00); 
    outb(device + 3, 0x80);  
    outb(device + 0, 0x03); 
    outb(device + 1, 0x00);  
    outb(device + 3, 0x03); 
    outb(device + 2, 0xC7); 
    outb(device + 4, 0x0B); 
    outb(device + 4, 0x1E); 
    outb(device + 0, 0xAE);  

    if (inb(device + 0) != 0xAE) {
        return 1;  
    }

    outb(device + 4, 0x0F); 
    return 0;
}

int is_transmit_empty() {
    return inb(currentSerial + 5) & 0x20;
}

void write_serial(char a) {
    while (!is_transmit_empty());
    if (a == '\n') {
        outb(currentSerial, '\r'); 
        while (!is_transmit_empty());
    }
    outb(currentSerial, a);
}

void writeSerial(const char *format, ...) {
    va_list args;
    va_start(args, format);
    vsprintf(write_serial, format, args);
    va_end(args);
}


