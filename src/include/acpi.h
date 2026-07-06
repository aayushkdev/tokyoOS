#pragma once

#include <stdint.h>

typedef struct {
    char signature[4];
    uint32_t length;
    uint8_t revision;
    uint8_t checksum;
    char oem_id[6];
    char oem_table_id[8];
    uint32_t oem_revision;
    uint32_t creator_id;
    uint32_t creator_revision;
} __attribute__((packed)) acpi_sdt_header_t;

int init_acpi(void);
const acpi_sdt_header_t *acpi_find_table(const char signature[4]);
void *acpi_map_physical(uint64_t physical_address);
