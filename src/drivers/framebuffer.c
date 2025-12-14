#include "framebuffer.h"
#include <string.h>

#define FB_ADDR   0xA0000  // VGA Mode 13h VRAM

static uint8_t *front = (uint8_t*)FB_ADDR;
static uint8_t *back  = NULL;
static bool use_double_buffer = false;

FakeXImage* image = NULL;

void fb_init(fb_mode_t mode) {
    use_double_buffer = (mode == FB_DOUBLE_BUFFER);

    if(use_double_buffer) {
        back = (uint8_t*)0x90000;        // pick any free memory area you like
        memset(back, 0, FB_SIZE);
    }

    // setup fake XImage
    image = (FakeXImage*)0x91000; // pick any free memory area
    image->data = use_double_buffer ? back : front;
}

// ---- pixel ----
void fb_putpixel(int x, int y, uint8_t color) {
    if ((unsigned)x >= FB_WIDTH || (unsigned)y >= FB_HEIGHT) return;

    if(use_double_buffer)
        back[y * FB_WIDTH + x] = color;
    else
        front[y * FB_WIDTH + x] = color;
}

// ---- clear ----
void fb_clear(uint8_t color) {
    if(use_double_buffer)
        memset(back, color, FB_SIZE);
    else
        memset(front, color, FB_SIZE);
}

// ---- horizontal line ----
void fb_hline(int x, int y, int w, uint8_t color) {
    if ((unsigned)y >= FB_HEIGHT) return;
    if (x < 0) { w += x; x = 0; }
    if (x + w > FB_WIDTH) w = FB_WIDTH - x;
    if (w <= 0) return;

    uint8_t *row = (use_double_buffer ? back : front) + y * FB_WIDTH + x;
    memset(row, color, w);
}

// ---- rectangle ----
void fb_rect(int x, int y, int w, int h, uint8_t color) {
    for(int i = 0; i < h; i++)
        fb_hline(x, y + i, w, color);
}

// ---- buffer swap ----
void fb_swap() {
    if(!use_double_buffer) return;
    memcpy(front, back, FB_SIZE);
}

// ---- raw access (useful for scaling or Doom blit) ----
uint8_t* fb_frontbuffer() { return front; }
uint8_t* fb_backbuffer()  { return use_double_buffer ? back : front; }

void fb_draw_char(int x, int y, char c, uint8_t color) {
    extern uint8_t font8x8_basic[128][8];
    if (c < 0 || c > 127) return; // unsupported character

    for (int row = 0; row < 8; row++) {
        uint8_t row_data = font8x8_basic[(int)c][row];
        for (int col = 0; col < 8; col++) {
            if (row_data & (1 << (col))) {
                fb_putpixel(x + col, y + row, color);
            }
        }
    }
}

void fb_draw_string(int x, int y, const char* str, uint8_t color) {
    while (*str) {
        fb_draw_char(x, y, *str++, color);
        x += 8; // Move to the next character position
    }
}

void fb_draw_image() {
    // convert from X image format to framebuffer format
    memcpy(fb_backbuffer(), image->data, FB_SIZE);

}

void panic(const char *msg) {
    fb_init(FB_SINGLE_BUFFER);
    fb_clear(0); // clear screen to black
    fb_draw_string(10, 10, "PANIC:", 15);
    fb_draw_string(10, 30, msg, 15);
    while(1); // halt
}