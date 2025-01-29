#include <isr.h>
#include <stdint.h>
#include <idt.h>
#include <print.h>
#include <serial.h>

void interrupt_handler(uint64_t *regs) {
    uint64_t vector_number = regs[0];  // The interrupt vector number is typically passed in regs[0]
    uint64_t error_code = regs[1];     // The error code is often passed in regs[1]

    // Handle specific interrupts based on the vector number
    switch (vector_number) {
        case 0: // Divide by zero exception
            print("Divide by zero error.");
            break;
        default: // Catch-all for any unexpected interrupt
            // Print the vector number and the error code
            print("Random error");
            //print("Unexpected interrupt: Vector %d, Error Code %d\n", vector_number, error_code);
            break;
    }

    // Return from the interrupt
    asm volatile("iretq");
}