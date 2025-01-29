#pragma once

#include <stdint.h>

// Read a byte from an I/O port
uint8_t inb(uint16_t port);

// Write a byte to an I/O port
void outb(uint16_t port, uint8_t value);

//kernel panic
void panic();

