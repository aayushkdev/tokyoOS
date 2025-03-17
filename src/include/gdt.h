#pragma once

#include <stdint.h>

typedef struct GDTEntry {
    uint16_t limit_low;
    uint16_t base_low;
    uint8_t  base_mid;
    uint8_t  access;
    uint8_t  granularity;
    uint8_t  base_high;

} __attribute__((packed)) GDTEntry;


typedef struct GDTPtr {
    uint16_t limit;
    uint64_t base;
} __attribute__((packed)) GDTPtr;


#define GDT_ENTRY_COUNT 3

void init_gdt();




