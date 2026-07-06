#include <isr.h>
#include <stdint.h>
#include <idt.h>
#include <serial.h>
#include <system.h>
#include <timer.h>
#include <kb.h>

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
        case TIMER_VECTOR:
            timer_handle_tick();
            return context;
        case KB_VECTOR:
            kb_handle_irq();
            return context;
        case 255:
            return context;
        default:
            writeSerial("Unhandled interrupt: %d\n", context->vector_number);
            break;
    }

    panic();
    return context;
}
