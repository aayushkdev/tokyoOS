#include <gdt.h>
#include <stddef.h>


static GDTEntry gdt[GDT_ENTRY_COUNT];
static GDTPtr gdtp;


void set_gdt_entry(int index, uint8_t access, uint8_t flags) {
    gdt[index].base_low = 0;  
    gdt[index].base_mid = 0;
    gdt[index].base_high = 0;
    
    gdt[index].limit_low = 0; 
    gdt[index].granularity = flags;
    gdt[index].access = access;

}



void gdt_reload() {
    asm volatile(
        "lgdt %0\n\t"
        "push $0x08\n\t"
        "lea 1f(%%rip), %%rax\n\t"
        "push %%rax\n\t"
        "lretq\n\t"
        "1:\n\t"
        "mov $0x10, %%eax\n\t"
        "mov %%eax, %%ds\n\t"
        "mov %%eax, %%es\n\t"
        "mov %%eax, %%fs\n\t"
        "mov %%eax, %%gs\n\t"
        "mov %%eax, %%ss\n\t"
        :
        : "m"(gdtp)
        : "rax", "memory"
    );
}


void initiateGDT() {
    set_gdt_entry(0, 0, 0);         
    set_gdt_entry(1, 0x9A, 0xA0);  //kernel code
    set_gdt_entry(2, 0x92, 0x00);  //kernel data
    set_gdt_entry(3, 0xFA, 0xA0);  //user code 
    set_gdt_entry(4, 0xF2, 0x00);  //user data

    gdtp.limit = sizeof(gdt) - 1;
    gdtp.base = (uint64_t)&gdt;

    gdt_reload();
}

