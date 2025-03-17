#pragma once
#include "limine.h"
#include <stdint.h>

// Framebuffer info structure
struct fb_info {
    uint64_t width;
    uint64_t height;
    uint64_t pitch;
    uint32_t* framebuffer;
};

// Global framebuffer info instance
extern struct fb_info fb_info;

// Function declarations
void init_framebuffer(void);

void clearScreen(uint32_t color);

void drawChar(int x, int y, char c, uint32_t color);