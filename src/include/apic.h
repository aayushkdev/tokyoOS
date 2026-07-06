#pragma once

#include <stdint.h>

int init_apic(void);
uint64_t apic_local_base(void);
uint32_t apic_local_id(void);
uint32_t apic_local_version(void);
uint32_t apic_processor_count(void);
uint32_t apic_ioapic_count(void);
uint32_t apic_ioapic_redirection_count(void);
void lapic_eoi(void);
int ioapic_route_irq(uint8_t irq, uint8_t vector);
int lapic_timer_calibrate(uint32_t calibration_ms);
void lapic_timer_start_periodic(uint32_t hz, uint8_t vector);
uint32_t lapic_timer_ticks_per_ms(void);
