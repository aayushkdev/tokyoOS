#include <stddef.h>
#include <stdint.h>
#include <disp.h>
#include <limine.h>
#include <system.h>
#include <print.h>
#include <serial.h>
#include <idt.h>
#include <gdt.h>


// Kernel entry point
void _start(void) {
    initialiseFrameBuffer();
    if (initSerial(COM1) != 0) {
        printf("Error: Failed to initialize COM1 serial port.\n");
    }
    initSerial(COM1);
    initiateGDT();
    load_idt();
    printf("---Tokyo OS---");
    while (1) {
        asm volatile("hlt"); 
    }
}
