#include <stddef.h>
#include <stdint.h>
#include <disp.h>
#include <limine.h>
#include <system.h>
#include <kprintf.h>
#include <serial.h>
#include <idt.h>
#include <gdt.h>
#include <acpi.h>
#include <apic.h>
#include <timer.h>
#include <kb.h>


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
    if (init_acpi() == 0) {
        if (init_apic() == 0 && init_timer() == 0 && init_kb() == 0) {
            enable_interrupts();
        }
    }
    
    while (1) {
        kb_event_t event;
        while (kb_read_event(&event)) {
            char printable = kb_event_to_char(event);
            if (printable != 0) {
                kprintf("%c", printable);
            }
        }

        asm volatile("hlt"); 
    }
}
