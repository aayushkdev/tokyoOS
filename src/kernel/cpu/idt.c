#include <idt.h>
#include <isr.h>


//__attribute__((aligned(0x10))) 
static idt_entry_t idt[256]; // Create an array of IDT entries; aligned for performance
static idtr_t idtr;

//static GDT_KERNEL_CODE = 0x80;

void set_idt(uint8_t vector, void* isr, uint8_t flags) {
    idt_entry_t* descriptor = &idt[vector];

    descriptor->isr_low        = (uint64_t)isr & 0xFFFF;
    descriptor->kernel_cs      = 0x08;
    descriptor->ist            = 0;
    descriptor->attributes     = flags;
    descriptor->isr_mid        = ((uint64_t)isr >> 16) & 0xFFFF;
    descriptor->isr_high       = ((uint64_t)isr >> 32) & 0xFFFFFFFF;
    descriptor->reserved       = 0;
}


void load_idt() {
    idtr.base = (uint64_t)&idt;   // Base address of the IDT
    idtr.limit = sizeof(idt) - 1; // Size of the IDT


    set_idt(0, isr0, 0x8E);
    set_idt(1, isr1, 0x8E);
    set_idt(2, isr2, 0x8E);
    set_idt(3, isr3, 0x8E);
    set_idt(4, isr4, 0x8E);
    set_idt(5, isr5, 0x8E);
    set_idt(6, isr6, 0x8E);
    set_idt(7, isr7, 0x8E);
    set_idt(8, isr8, 0x8E);
    set_idt(9, isr9, 0x8E);
    set_idt(10, isr10, 0x8E);
    set_idt(11, isr11, 0x8E);
    set_idt(12, isr12, 0x8E);
    set_idt(13, isr13, 0x8E);
    set_idt(14, isr14, 0x8E);
    set_idt(15, isr15, 0x8E);
    set_idt(16, isr16, 0x8E);
    set_idt(17, isr17, 0x8E);
    set_idt(18, isr18, 0x8E);
    set_idt(19, isr19, 0x8E);
    set_idt(20, isr20, 0x8E);
    set_idt(21, isr21, 0x8E);
    set_idt(22, isr22, 0x8E);
    set_idt(23, isr23, 0x8E);
    set_idt(24, isr24, 0x8E);
    set_idt(25, isr25, 0x8E);
    set_idt(26, isr26, 0x8E);
    set_idt(27, isr27, 0x8E);
    set_idt(28, isr28, 0x8E);
    set_idt(29, isr29, 0x8E);
    set_idt(30, isr30, 0x8E);
    set_idt(31, isr31, 0x8E);

    

    asm volatile("lidt %0" : : "m"(*(&idtr)));
}