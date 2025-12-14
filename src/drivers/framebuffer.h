#pragma once
#include <stdint.h>
#include <stdbool.h>

#define FB_WIDTH   320
#define FB_HEIGHT  200
#define FB_SIZE   (FB_WIDTH * FB_HEIGHT)

typedef enum {
    FB_SINGLE_BUFFER = 0,
    FB_DOUBLE_BUFFER = 1
} fb_mode_t;

// Initialization
void fb_init(fb_mode_t mode);

// Drawing
void fb_putpixel(int x, int y, uint8_t color);
void fb_clear(uint8_t color);
void fb_hline(int x, int y, int w, uint8_t color);
void fb_rect(int x, int y, int w, int h, uint8_t color);

// Buffer handling (only if double buffered)
void fb_swap();

uint8_t* fb_frontbuffer();
uint8_t* fb_backbuffer();

void fb_draw_char(int x, int y, char c, uint8_t color);
void fb_draw_string(int x, int y, const char* str, uint8_t color);

typedef struct FakeXImage {
    uint8_t* data;
} FakeXImage;

extern FakeXImage* image;

#define X_width FB_WIDTH
#define X_height FB_HEIGHT

void fb_draw_image();