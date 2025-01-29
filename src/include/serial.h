#pragma once

//define serial ports
#define COM1 0x3f8          // COM1
#define COM2 0x2f8          // COM2
#define COM3 0x3e8          // COM3
#define COM4 0x2e8          // COM4

int initSerial(int device);
void writeSerial(char *a);
void write_serial(char a);