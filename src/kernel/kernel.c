#include <stddef.h>
#include <stdint.h>
#include <disp.h>
#include <limine.h>
#include <system.h>
#include <print.h>
#include <serial.h>
#include <idt.h>
#include <gdt.h>


void _start(void) {
    init_framebuffer();
    if (init_serial(COM1) != 0) {
        printf("Error: Failed to initialize COM1 serial port.\n");
    }
    init_gdt();
    init_idt();


    printf("------------------\n");
    printf("-    Tokyo OS    -\n");
    printf("------------------\n");
    
    while (1) {
        asm volatile("hlt"); 
    }
}
