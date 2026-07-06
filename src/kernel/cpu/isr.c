#include <isr.h>
#include <stdint.h>
#include <idt.h>
#include <serial.h>
#include <system.h>

cpu_status_t *interrupt_dispatch(cpu_status_t *context) {
    switch (context->vector_number) {
        case 0:
            writeSerial("Divide by zero error.\n");
            break;
        case 6:
            writeSerial("Invalid opcode.\n");
            break;
        case 8:
            writeSerial("Double fault, Error Code: %d\n", context->error_code);
            break;
        case 13:
            writeSerial("General protection fault, Error Code: %d\n", context->error_code);
            break;
        case 14:
            writeSerial("Page fault, Error Code: %d\n", context->error_code);
            break;
        default:
            writeSerial("Unhandled interrupt: %d\n", context->vector_number);
            break;
    }

    panic();
    return context;
}
