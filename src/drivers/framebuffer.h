#pragma once
#include <stdint.h>
#include <stdbool.h>

#define FB_WIDTH   1024
#define FB_HEIGHT  768
#define FB_SIZE   (FB_WIDTH * FB_HEIGHT)

extern uint32_t* front;
extern uint32_t* back;

typedef enum {
    FB_SINGLE_BUFFER = 0,
    FB_DOUBLE_BUFFER = 1
} fb_mode_t;

// Initialization
void fb_init(fb_mode_t mode, void* fb_address);

// Drawing
void fb_putpixel(int x, int y, uint32_t color);
void fb_clear(uint32_t color);
void fb_hline(int x, int y, int w, uint32_t color);
void fb_rect(int x, int y, int w, int h, uint32_t color);

// Buffer handling (only if double buffered)
void fb_swap();

uint32_t* fb_frontbuffer();
uint32_t* fb_backbuffer();

void fb_draw_char(int x, int y, char c, uint32_t color);
void fb_draw_string(int x, int y, const char* str, uint32_t color);
void fb_draw_hex(int x, int y, uint32_t value, uint32_t color);

typedef struct FakeXImage {
    uint32_t* data;
} FakeXImage;

extern FakeXImage* image;

#define X_width 320
#define X_height 200

void fb_draw_image();