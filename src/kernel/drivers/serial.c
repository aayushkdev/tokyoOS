#include <serial.h>
#include <stdarg.h>
#include <system.h>

static int currentSerial = COM1;

// Initialize the serial port
int initSerial(int device) {
    currentSerial = device;
    outb(device + 1, 0x00);  // Disable all interrupts
    outb(device + 3, 0x80);  // Enable DLAB
    outb(device + 0, 0x03);  // Set divisor to 3 (38400 baud)
    outb(device + 1, 0x00);  
    outb(device + 3, 0x03);  // 8 bits, no parity, one stop bit
    outb(device + 2, 0xC7);  // Enable FIFO, clear with 14-byte threshold
    outb(device + 4, 0x0B);  // IRQs enabled, RTS/DSR set
    outb(device + 4, 0x1E);  // Set in loopback mode
    outb(device + 0, 0xAE);  // Test the serial chip

    if (inb(device + 0) != 0xAE) {
        return 1;  // Faulty serial port
    }

    outb(device + 4, 0x0F);  // Set normal operation mode
    return 0;
}

// Check if the transmit buffer is empty
int is_transmit_empty() {
    return inb(currentSerial + 5) & 0x20;
}

// Write a single character to the serial port
void write_serial(char a) {
    while (!is_transmit_empty());
    outb(currentSerial, a);
}

// Write a string to the serial port
void writeSerial(char *str) {
    while (*str) {
        write_serial(*str++);
    }
}


