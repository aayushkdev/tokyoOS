#include <acpi.h>
#include <apic.h>
#include <stddef.h>
#include <stdint.h>
#include <system.h>

#define PIC_MASTER_COMMAND 0x20
#define PIC_MASTER_DATA 0x21
#define PIC_SLAVE_COMMAND 0xA0
#define PIC_SLAVE_DATA 0xA1

#define PIC_ICW1_INIT 0x11
#define PIC_ICW2_MASTER_OFFSET 0x20
#define PIC_ICW2_SLAVE_OFFSET 0x28
#define PIC_ICW3_MASTER_HAS_SLAVE 0x04
#define PIC_ICW3_SLAVE_ID 0x02
#define PIC_ICW4_8086_MODE 0x01

#define IA32_APIC_BASE_MSR 0x1B
#define IA32_APIC_BASE_ENABLE (1ULL << 11)
#define IA32_APIC_BASE_ADDRESS_MASK 0xFFFFF000ULL

#define LAPIC_ID_REG 0x20
#define LAPIC_VERSION_REG 0x30
#define LAPIC_EOI_REG 0xB0
#define LAPIC_SPURIOUS_REG 0xF0
#define LAPIC_TIMER_LVT_REG 0x320
#define LAPIC_TIMER_INITIAL_COUNT_REG 0x380
#define LAPIC_TIMER_CURRENT_COUNT_REG 0x390
#define LAPIC_TIMER_DIVIDE_REG 0x3E0

#define LAPIC_SPURIOUS_VECTOR 0xFF
#define LAPIC_SPURIOUS_ENABLE (1 << 8)
#define LAPIC_LVT_MASKED (1 << 16)
#define LAPIC_TIMER_PERIODIC (1 << 17)
#define LAPIC_TIMER_DIVIDE_BY_16 0x3

#define PIT_CHANNEL0_DATA 0x40
#define PIT_COMMAND 0x43
#define PIT_FREQUENCY 1193182
#define PIT_MODE0_COMMAND 0x30
#define PIT_LATCH_CHANNEL0 0x00

#define IOAPIC_REG_SELECT 0x00
#define IOAPIC_REG_WINDOW 0x10
#define IOAPIC_VERSION_REG 0x01
#define IOAPIC_REDIRECTION_TABLE 0x10
#define IOAPIC_REDIRECTION_MASKED (1 << 16)
#define IOAPIC_REDIRECTION_POLARITY_LOW (1 << 13)
#define IOAPIC_REDIRECTION_TRIGGER_LEVEL (1 << 15)

#define MADT_ENTRY_LOCAL_APIC 0
#define MADT_ENTRY_IOAPIC 1
#define MADT_ENTRY_INTERRUPT_SOURCE_OVERRIDE 2
#define MADT_ENTRY_LOCAL_APIC_ADDRESS_OVERRIDE 5

#define LOCAL_APIC_ENABLED (1 << 0)
#define LOCAL_APIC_ONLINE_CAPABLE (1 << 1)

#define MAX_INTERRUPT_SOURCE_OVERRIDES 16

typedef struct {
    acpi_sdt_header_t header;
    uint32_t local_apic_address;
    uint32_t flags;
    uint8_t entries[];
} __attribute__((packed)) madt_t;

typedef struct {
    uint8_t type;
    uint8_t length;
} __attribute__((packed)) madt_entry_header_t;

typedef struct {
    madt_entry_header_t header;
    uint8_t processor_id;
    uint8_t apic_id;
    uint32_t flags;
} __attribute__((packed)) madt_local_apic_t;

typedef struct {
    madt_entry_header_t header;
    uint8_t ioapic_id;
    uint8_t reserved;
    uint32_t ioapic_address;
    uint32_t global_system_interrupt_base;
} __attribute__((packed)) madt_ioapic_t;

typedef struct {
    madt_entry_header_t header;
    uint8_t bus_source;
    uint8_t irq_source;
    uint32_t global_system_interrupt;
    uint16_t flags;
} __attribute__((packed)) madt_interrupt_source_override_t;

typedef struct {
    madt_entry_header_t header;
    uint16_t reserved;
    uint64_t local_apic_address;
} __attribute__((packed)) madt_local_apic_address_override_t;

typedef struct {
    uint8_t irq_source;
    uint32_t global_system_interrupt;
    uint16_t flags;
} interrupt_source_override_t;

static const madt_t *madt;
static uint64_t local_apic_base;
static uintptr_t local_apic_virt;
static uint32_t local_apic_id;
static uint32_t local_apic_version;
static uint32_t processor_count;
static uint32_t timer_ticks_per_ms;

static uint32_t ioapic_count;
static uint64_t ioapic_base;
static uintptr_t ioapic_virt;
static uint32_t ioapic_gsi_base;
static uint32_t ioapic_redirection_count;

static interrupt_source_override_t interrupt_source_overrides[MAX_INTERRUPT_SOURCE_OVERRIDES];
static uint32_t interrupt_source_override_count;

static void pic_io_wait(void) {
    outb(0x80, 0);
}

static void disable_pic(void) {
    outb(PIC_MASTER_COMMAND, PIC_ICW1_INIT);
    pic_io_wait();
    outb(PIC_SLAVE_COMMAND, PIC_ICW1_INIT);
    pic_io_wait();
    outb(PIC_MASTER_DATA, PIC_ICW2_MASTER_OFFSET);
    pic_io_wait();
    outb(PIC_SLAVE_DATA, PIC_ICW2_SLAVE_OFFSET);
    pic_io_wait();
    outb(PIC_MASTER_DATA, PIC_ICW3_MASTER_HAS_SLAVE);
    pic_io_wait();
    outb(PIC_SLAVE_DATA, PIC_ICW3_SLAVE_ID);
    pic_io_wait();
    outb(PIC_MASTER_DATA, PIC_ICW4_8086_MODE);
    pic_io_wait();
    outb(PIC_SLAVE_DATA, PIC_ICW4_8086_MODE);
    pic_io_wait();
    outb(PIC_MASTER_DATA, 0xFF);
    outb(PIC_SLAVE_DATA, 0xFF);
}

static uint32_t mmio_read32(uintptr_t address) {
    return *(volatile uint32_t *)address;
}

static void mmio_write32(uintptr_t address, uint32_t value) {
    *(volatile uint32_t *)address = value;
}

static uint32_t lapic_read(uint32_t reg) {
    return mmio_read32(local_apic_virt + reg);
}

static void lapic_write(uint32_t reg, uint32_t value) {
    mmio_write32(local_apic_virt + reg, value);
}

static uint32_t ioapic_read(uint32_t reg) {
    mmio_write32(ioapic_virt + IOAPIC_REG_SELECT, reg);
    return mmio_read32(ioapic_virt + IOAPIC_REG_WINDOW);
}

static void ioapic_write(uint32_t reg, uint32_t value) {
    mmio_write32(ioapic_virt + IOAPIC_REG_SELECT, reg);
    mmio_write32(ioapic_virt + IOAPIC_REG_WINDOW, value);
}

static const interrupt_source_override_t *find_irq_override(uint8_t irq) {
    for (uint32_t i = 0; i < interrupt_source_override_count; i++) {
        if (interrupt_source_overrides[i].irq_source == irq) {
            return &interrupt_source_overrides[i];
        }
    }

    return NULL;
}

static void pit_start_oneshot(uint16_t count) {
    outb(PIT_COMMAND, PIT_MODE0_COMMAND);
    outb(PIT_CHANNEL0_DATA, count & 0xFF);
    outb(PIT_CHANNEL0_DATA, count >> 8);
}

static uint16_t pit_read_count(void) {
    outb(PIT_COMMAND, PIT_LATCH_CHANNEL0);
    uint16_t low = inb(PIT_CHANNEL0_DATA);
    uint16_t high = inb(PIT_CHANNEL0_DATA);
    return low | (high << 8);
}

static int parse_madt(void) {
    local_apic_base = madt->local_apic_address;
    processor_count = 0;
    ioapic_count = 0;
    ioapic_base = 0;
    ioapic_virt = 0;
    ioapic_gsi_base = 0;
    ioapic_redirection_count = 0;
    interrupt_source_override_count = 0;

    uintptr_t entry_address = (uintptr_t)madt->entries;
    uintptr_t table_end = (uintptr_t)madt + madt->header.length;

    while (entry_address + sizeof(madt_entry_header_t) <= table_end) {
        const madt_entry_header_t *entry = (const madt_entry_header_t *)entry_address;
        if (entry->length < sizeof(madt_entry_header_t) || entry_address + entry->length > table_end) {
            return -1;
        }

        switch (entry->type) {
            case MADT_ENTRY_LOCAL_APIC: {
                const madt_local_apic_t *local_apic = (const madt_local_apic_t *)entry;
                if ((local_apic->flags & (LOCAL_APIC_ENABLED | LOCAL_APIC_ONLINE_CAPABLE)) != 0) {
                    processor_count++;
                }
                break;
            }
            case MADT_ENTRY_IOAPIC: {
                const madt_ioapic_t *ioapic = (const madt_ioapic_t *)entry;
                if (ioapic_count == 0) {
                    ioapic_base = ioapic->ioapic_address;
                    ioapic_gsi_base = ioapic->global_system_interrupt_base;
                }
                ioapic_count++;
                break;
            }
            case MADT_ENTRY_INTERRUPT_SOURCE_OVERRIDE: {
                if (interrupt_source_override_count < MAX_INTERRUPT_SOURCE_OVERRIDES) {
                    const madt_interrupt_source_override_t *override =
                        (const madt_interrupt_source_override_t *)entry;
                    interrupt_source_overrides[interrupt_source_override_count].irq_source =
                        override->irq_source;
                    interrupt_source_overrides[interrupt_source_override_count].global_system_interrupt =
                        override->global_system_interrupt;
                    interrupt_source_overrides[interrupt_source_override_count].flags = override->flags;
                    interrupt_source_override_count++;
                }
                break;
            }
            case MADT_ENTRY_LOCAL_APIC_ADDRESS_OVERRIDE: {
                const madt_local_apic_address_override_t *override =
                    (const madt_local_apic_address_override_t *)entry;
                local_apic_base = override->local_apic_address;
                break;
            }
            default:
                break;
        }

        entry_address += entry->length;
    }

    return 0;
}

static void enable_local_apic(void) {
    uint64_t apic_base_msr = rdmsr(IA32_APIC_BASE_MSR);
    uint64_t msr_base = apic_base_msr & IA32_APIC_BASE_ADDRESS_MASK;

    if (msr_base != 0) {
        local_apic_base = msr_base;
    }

    wrmsr(IA32_APIC_BASE_MSR,
          (apic_base_msr & ~IA32_APIC_BASE_ADDRESS_MASK) |
          (local_apic_base & IA32_APIC_BASE_ADDRESS_MASK) |
          IA32_APIC_BASE_ENABLE);

    local_apic_virt = (uintptr_t)acpi_map_physical(local_apic_base);

    uint32_t spurious = lapic_read(LAPIC_SPURIOUS_REG);
    lapic_write(LAPIC_SPURIOUS_REG,
                (spurious & ~0xFF) | LAPIC_SPURIOUS_VECTOR | LAPIC_SPURIOUS_ENABLE);

    local_apic_id = lapic_read(LAPIC_ID_REG) >> 24;
    local_apic_version = lapic_read(LAPIC_VERSION_REG) & 0xFF;
}

static void init_ioapic(void) {
    if (ioapic_base == 0) {
        return;
    }

    ioapic_virt = (uintptr_t)acpi_map_physical(ioapic_base);
    uint32_t version = ioapic_read(IOAPIC_VERSION_REG);
    ioapic_redirection_count = ((version >> 16) & 0xFF) + 1;
}

int init_apic(void) {
    madt = (const madt_t *)acpi_find_table("APIC");
    if (madt == NULL) {
        return -1;
    }

    if (parse_madt() != 0) {
        return -1;
    }

    disable_pic();
    enable_local_apic();
    init_ioapic();

    return 0;
}

uint64_t apic_local_base(void) {
    return local_apic_base;
}

uint32_t apic_local_id(void) {
    return local_apic_id;
}

uint32_t apic_local_version(void) {
    return local_apic_version;
}

uint32_t apic_processor_count(void) {
    return processor_count;
}

uint32_t apic_ioapic_count(void) {
    return ioapic_count;
}

uint32_t apic_ioapic_redirection_count(void) {
    return ioapic_redirection_count;
}

void lapic_eoi(void) {
    lapic_write(LAPIC_EOI_REG, 0);
}

int ioapic_route_irq(uint8_t irq, uint8_t vector) {
    if (ioapic_virt == 0 || ioapic_redirection_count == 0) {
        return -1;
    }

    uint32_t gsi = ioapic_gsi_base + irq;
    uint16_t flags = 0;
    const interrupt_source_override_t *override = find_irq_override(irq);
    if (override != NULL) {
        gsi = override->global_system_interrupt;
        flags = override->flags;
    }

    if (gsi < ioapic_gsi_base) {
        return -1;
    }

    uint32_t redirection_index = gsi - ioapic_gsi_base;
    if (redirection_index >= ioapic_redirection_count) {
        return -1;
    }

    uint32_t low = vector;
    uint32_t polarity = flags & 0x3;
    uint32_t trigger = (flags >> 2) & 0x3;

    if (polarity == 0x3) {
        low |= IOAPIC_REDIRECTION_POLARITY_LOW;
    }

    if (trigger == 0x3) {
        low |= IOAPIC_REDIRECTION_TRIGGER_LEVEL;
    }

    uint32_t high = apic_local_id() << 24;
    uint32_t redirection_reg = IOAPIC_REDIRECTION_TABLE + (redirection_index * 2);

    ioapic_write(redirection_reg + 1, high);
    ioapic_write(redirection_reg, low & ~IOAPIC_REDIRECTION_MASKED);

    return 0;
}

int lapic_timer_calibrate(uint32_t calibration_ms) {
    if (calibration_ms == 0 || local_apic_virt == 0) {
        return -1;
    }

    uint32_t pit_count = (PIT_FREQUENCY * calibration_ms) / 1000;
    if (pit_count == 0 || pit_count > 0xFFFF) {
        return -1;
    }

    lapic_write(LAPIC_TIMER_DIVIDE_REG, LAPIC_TIMER_DIVIDE_BY_16);
    lapic_write(LAPIC_TIMER_LVT_REG, LAPIC_LVT_MASKED);
    lapic_write(LAPIC_TIMER_INITIAL_COUNT_REG, 0xFFFFFFFF);

    pit_start_oneshot((uint16_t)pit_count);

    uint16_t last_count = pit_read_count();
    for (;;) {
        uint16_t current_count = pit_read_count();
        if (current_count == 0 || current_count > last_count) {
            break;
        }

        last_count = current_count;
    }

    uint32_t elapsed_ticks = 0xFFFFFFFF - lapic_read(LAPIC_TIMER_CURRENT_COUNT_REG);
    lapic_write(LAPIC_TIMER_INITIAL_COUNT_REG, 0);

    timer_ticks_per_ms = elapsed_ticks / calibration_ms;
    return timer_ticks_per_ms == 0 ? -1 : 0;
}

void lapic_timer_start_periodic(uint32_t hz, uint8_t vector) {
    if (timer_ticks_per_ms == 0 || hz == 0) {
        return;
    }

    uint32_t ticks = (timer_ticks_per_ms * 1000) / hz;
    lapic_write(LAPIC_TIMER_DIVIDE_REG, LAPIC_TIMER_DIVIDE_BY_16);
    lapic_write(LAPIC_TIMER_LVT_REG, LAPIC_TIMER_PERIODIC | vector);
    lapic_write(LAPIC_TIMER_INITIAL_COUNT_REG, ticks);
}

uint32_t lapic_timer_ticks_per_ms(void) {
    return timer_ticks_per_ms;
}
