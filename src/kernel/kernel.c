#include <stddef.h>
#include <stdint.h>
#include <disp.h>
#include <limine.h>
#include <system.h>
#include <kprintf.h>
#include <serial.h>
#include <idt.h>
#include <gdt.h>


void _start(void) {
    init_framebuffer();
    if (init_serial(COM1) != 0) {
        kprintf("Error: Failed to initialize COM1 serial port.\n");
    }
    init_gdt();
    init_idt();


    kprintf("------------------\n");
    kprintf("-    Tokyo OS    -\n");
    kprintf("------------------\n");
    
    while (1) {
        asm volatile("hlt"); 
    }
}
