#include <acpi.h>
#include <limine.h>
#include <stddef.h>
#include <stdint.h>

typedef struct {
    char signature[8];
    uint8_t checksum;
    char oem_id[6];
    uint8_t revision;
    uint32_t rsdt_address;
} __attribute__((packed)) rsdp_descriptor_t;

typedef struct {
    rsdp_descriptor_t first_part;
    uint32_t length;
    uint64_t xsdt_address;
    uint8_t extended_checksum;
    uint8_t reserved[3];
} __attribute__((packed)) rsdp2_descriptor_t;

typedef struct {
    acpi_sdt_header_t header;
    uint32_t entries[];
} __attribute__((packed)) rsdt_t;

typedef struct {
    acpi_sdt_header_t header;
    uint64_t entries[];
} __attribute__((packed)) xsdt_t;

static volatile struct limine_rsdp_request rsdp_request = {
    .id = LIMINE_RSDP_REQUEST,
    .revision = 0
};

static volatile struct limine_hhdm_request hhdm_request = {
    .id = LIMINE_HHDM_REQUEST,
    .revision = 0
};

static const rsdt_t *root_rsdt;
static const xsdt_t *root_xsdt;

static void *phys_to_virt(uint64_t physical_address) {
    return (void *)(physical_address + hhdm_request.response->offset);
}

void *acpi_map_physical(uint64_t physical_address) {
    if (hhdm_request.response == NULL) {
        return NULL;
    }

    return phys_to_virt(physical_address);
}

static int memory_equal(const char *left, const char *right, size_t length) {
    for (size_t i = 0; i < length; i++) {
        if (left[i] != right[i]) {
            return 0;
        }
    }

    return 1;
}

static uint8_t checksum_bytes(const void *data, size_t length) {
    const uint8_t *bytes = data;
    uint8_t sum = 0;

    for (size_t i = 0; i < length; i++) {
        sum += bytes[i];
    }

    return sum;
}

static int has_valid_checksum(const void *data, size_t length) {
    return checksum_bytes(data, length) == 0;
}

int init_acpi(void) {
    root_rsdt = NULL;
    root_xsdt = NULL;

    if (rsdp_request.response == NULL || rsdp_request.response->address == 0) {
        return -1;
    }

    if (hhdm_request.response == NULL) {
        return -1;
    }

    const rsdp_descriptor_t *rsdp = rsdp_request.response->address;
    if (!memory_equal(rsdp->signature, "RSD PTR ", 8)) {
        return -1;
    }

    if (!has_valid_checksum(rsdp, sizeof(rsdp_descriptor_t))) {
        return -1;
    }

    const rsdp2_descriptor_t *rsdp2 = (const rsdp2_descriptor_t *)rsdp;
    if (rsdp->revision >= 2 && rsdp2->xsdt_address != 0) {
        if (!has_valid_checksum(rsdp2, rsdp2->length)) {
            return -1;
        }

        const xsdt_t *xsdt = phys_to_virt(rsdp2->xsdt_address);
        if (!memory_equal(xsdt->header.signature, "XSDT", 4)) {
            return -1;
        }

        if (!has_valid_checksum(&xsdt->header, xsdt->header.length)) {
            return -1;
        }

        root_xsdt = xsdt;
        return 0;
    }

    const rsdt_t *rsdt = phys_to_virt(rsdp->rsdt_address);
    if (!memory_equal(rsdt->header.signature, "RSDT", 4)) {
        return -1;
    }

    if (!has_valid_checksum(&rsdt->header, rsdt->header.length)) {
        return -1;
    }

    root_rsdt = rsdt;
    return 0;
}

const acpi_sdt_header_t *acpi_find_table(const char signature[4]) {
    if (root_xsdt != NULL) {
        size_t entry_count = (root_xsdt->header.length - sizeof(acpi_sdt_header_t)) / sizeof(uint64_t);

        for (size_t i = 0; i < entry_count; i++) {
            const acpi_sdt_header_t *header = phys_to_virt(root_xsdt->entries[i]);
            if (memory_equal(header->signature, signature, 4) &&
                has_valid_checksum(header, header->length)) {
                return header;
            }
        }

        return NULL;
    }

    if (root_rsdt != NULL) {
        size_t entry_count = (root_rsdt->header.length - sizeof(acpi_sdt_header_t)) / sizeof(uint32_t);

        for (size_t i = 0; i < entry_count; i++) {
            const acpi_sdt_header_t *header = phys_to_virt(root_rsdt->entries[i]);
            if (memory_equal(header->signature, signature, 4) &&
                has_valid_checksum(header, header->length)) {
                return header;
            }
        }
    }

    return NULL;
}
