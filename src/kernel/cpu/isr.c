#include <isr.h>
#include <stdint.h>
#include <idt.h>
#include <print.h>
#include <serial.h>
#include <system.h>

void interrupt_handler(uint64_t *regs) {
    uint64_t vector_number = regs[0];  // The interrupt number
    uint64_t error_code = regs[1];     // The error code 

    switch (vector_number) {
        case 0: 
            writeSerial("Divide by zero error.\n");
            break;
        default:
            writeSerial("Random error\n");
            break;
    }

    panic();
}