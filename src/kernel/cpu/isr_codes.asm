[bits  64]
[extern interrupt_handler]



isr_common:
    ; Save all general-purpose registers
    push rax
    push rbx
    push rcx
    push rdx
    push rsi
    push rdi
    push rbp
    push r8
    push r9
    push r10
    push r11
    push r12
    push r13
    push r14
    push r15

    mov rdi, rsp  ; Pass the stack pointer (pointer to the saved registers) as the first argument
    
    call interrupt_handler

    ; Restore all general-purpose registers
    pop r15
    pop r14
    pop r13
    pop r12
    pop r11
    pop r10
    pop r9
    pop r8
    pop rbp
    pop rdi
    pop rsi
    pop rdx
    pop rcx
    pop rbx
    pop rax

    ; Clean up the error code and interrupt number
    ;add rsp, 16

    ; Return from the interrupt
    iretq


%macro ISR_ERROR_CODE 1
global isr%1
isr%1:
    push %1   
    jmp isr_common
%endmacro


%macro ISR_NO_ERROR_CODE 1
global isr%1
isr%1:
    push 0   
    push %1  
    jmp isr_common
%endmacro



; Exceptions
ISR_NO_ERROR_CODE 0   ; Divide by zero
ISR_NO_ERROR_CODE 1   ; Debug
ISR_NO_ERROR_CODE 2   ; Non-maskable interrupt
ISR_NO_ERROR_CODE 3   ; Breakpoint
ISR_NO_ERROR_CODE 4   ; Overflow
ISR_NO_ERROR_CODE 5   ; Bound range exceeded
ISR_NO_ERROR_CODE 6   ; Invalid opcode
ISR_NO_ERROR_CODE 7   ; Device not available
ISR_ERROR_CODE 8      ; Double fault
ISR_NO_ERROR_CODE 9   ; Coprocessor segment overrun
ISR_ERROR_CODE 10     ; Invalid TSS
ISR_ERROR_CODE 11     ; Segment not present
ISR_ERROR_CODE 12     ; Stack-segment fault
ISR_ERROR_CODE 13     ; General protection fault
ISR_ERROR_CODE 14     ; Page fault
ISR_NO_ERROR_CODE 15  ; Reserved
ISR_NO_ERROR_CODE 16  ; x87 floating-point exception
ISR_ERROR_CODE 17     ; Alignment check
ISR_NO_ERROR_CODE 18  ; Machine check
ISR_NO_ERROR_CODE 19  ; SIMD floating-point exception
ISR_NO_ERROR_CODE 20  ; Virtualization exception

ISR_NO_ERROR_CODE 21  ; Reserved
ISR_NO_ERROR_CODE 22  ; Reserved
ISR_NO_ERROR_CODE 23  ; Reserved
ISR_NO_ERROR_CODE 24  ; Reserved
ISR_NO_ERROR_CODE 25  ; Reserved
ISR_NO_ERROR_CODE 26  ; Reserved
ISR_NO_ERROR_CODE 27  ; Reserved
ISR_NO_ERROR_CODE 28  ; Reserved
ISR_NO_ERROR_CODE 29  ; Reserved
ISR_NO_ERROR_CODE 30  ; Reserved
ISR_NO_ERROR_CODE 31  ; Reserved