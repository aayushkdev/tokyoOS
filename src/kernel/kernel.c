#include <stddef.h>
#include <stdint.h>
#include <disp.h>
#include <limine.h>
#include <system.h>
#include <print.h>
#include <serial.h>
#include <idt.h>


// Kernel entry point
void _start(void) {
    initialiseFrameBuffer();
    if (initSerial(COM1) != 0) {
        print("Error: Failed to initialize COM1 serial port.\n");
        panic();  // Halt the system if serial fails
    }
    initSerial(COM1);

    load_idt();

    int a = 5/0;
    
    while (1) {
        asm volatile("hlt"); 
    }
}
