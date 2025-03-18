#include "disp.h"
#include <stddef.h>
#include <hackFont.h>
#include <serial.h>

volatile struct limine_framebuffer_request framebuffer_request = {
    .id = LIMINE_FRAMEBUFFER_REQUEST,
    .revision = 0
};

struct fb_info fb_info = {
    .width = 0,
    .height = 0,
    .framebuffer = NULL
};

void init_framebuffer(void) {
    if (framebuffer_request.response == NULL || framebuffer_request.response->framebuffer_count < 1) {
        writeSerial("Error: Failed to initialize framebuffer. No framebuffer found or invalid response.\n");
        return;
    }
    struct limine_framebuffer *framebuffer = framebuffer_request.response->framebuffers[0];
    fb_info.framebuffer = (uint32_t *)framebuffer->address;
    fb_info.width = framebuffer->width;
    fb_info.height = framebuffer->height;
    fb_info.pitch = framebuffer->pitch / sizeof(uint32_t);
}


void putPixel(int x, int y, uint32_t color) {
    fb_info.framebuffer[y * fb_info.pitch + x] = color;
}

void clearScreen(uint32_t color) {
    for (uint64_t i = 0; i < fb_info.width; i++) {
        for (uint64_t j = 0; j < fb_info.height; j++) {
            putPixel(i, j, color);
        }

    }
}

void drawChar(int x, int y, char c, uint32_t color) {
    int scale = 3;
    uint8_t* font1 = font[(uint8_t)c];
    for (int i = 0; i < 8; i++) {
        uint8_t row = font1[i];
        for (int j = 0; j < 8; j++) {
            if (row & (1 << (j))) {
                for (int dy = 0; dy < scale; dy++) {
                    for (int dx = 0; dx < scale; dx++) {
                        putPixel(x + j * scale + dx, y + i * scale + dy, color);
                    }
                }
            }
        }
    }
}


