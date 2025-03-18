#include <isr.h>
#include <stdint.h>
#include <idt.h>
#include <print.h>
#include <serial.h>
#include <system.h>

void interrupt_handler(uint64_t *regs) {
    uint64_t vector_number = regs[0];
    uint64_t error_code = regs[1];   

    switch (vector_number) {
        case 0:
            writeSerial("Divide by zero error.\n");
            break;
        case 6:
            writeSerial("Invalid opcode.\n");
            break;
        case 8:
            writeSerial("Double fault, Error Code: %d\n", error_code);
            break;
        case 13:
            writeSerial("General protection fault, Error Code: %d\n", error_code);
            break;
        case 14:
            writeSerial("Page fault, Error Code: %d\n", error_code);
            break;
        default:
            writeSerial("Unhandled interrupt: %d\n", vector_number);
            break;
    }

    panic();
}
