#pragma once

#include <stdint.h>

// Read a byte from an I/O port
uint8_t inb(uint16_t port);

// Write a byte to an I/O port
void outb(uint16_t port, uint8_t value);

// Read a model-specific register
uint64_t rdmsr(uint32_t msr);

// Write a model-specific register
void wrmsr(uint32_t msr, uint64_t value);

// Enable maskable interrupts
void enable_interrupts(void);

// Disable maskable interrupts
void disable_interrupts(void);

// Reboot through the PS/2 controller when available
void reboot(void);

//kernel panic
void panic();
