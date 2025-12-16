#include "framebuffer.h"
#include <string.h>

static bool use_double_buffer = false;

uint32_t* front = NULL;
uint32_t* back = NULL;

FakeXImage* image = NULL;

void fb_init(fb_mode_t mode, void* fb_address) {
    use_double_buffer = (mode == FB_DOUBLE_BUFFER);
    front = fb_address;
    if(use_double_buffer) {
        back = (uint32_t*)malloc(FB_SIZE * sizeof(uint32_t));        // pick any free memory area you like
        memset(back, 0, FB_SIZE * sizeof(uint32_t));
    }

    // setup fake XImage
    image = (FakeXImage*)malloc(sizeof(FakeXImage)); // pick any free memory area
    image->data = (uint32_t*)malloc(FB_SIZE * sizeof(uint32_t));
}

// ---- pixel ----
void fb_putpixel(int x, int y, uint32_t color) {
    if ((unsigned)x >= FB_WIDTH || (unsigned)y >= FB_HEIGHT) return;

    if(use_double_buffer)
        back[y * FB_WIDTH + x] = color;
    else
        front[y * FB_WIDTH + x] = color;
}

// ---- clear ----
void fb_clear(uint32_t color) {
    if(use_double_buffer)
        memset(back, color, FB_SIZE * sizeof(uint32_t));
    else
        memset(front, color, FB_SIZE * sizeof(uint32_t));
}

// ---- horizontal line ----
void fb_hline(int x, int y, int w, uint32_t color) {
    if (y < 0 || y >= FB_HEIGHT) return;
    if (x < 0) {
        w += x;
        x = 0;
    }
    if (x + w > FB_WIDTH) {
        w = FB_WIDTH - x;
    }
    if (w <= 0) return;

    uint32_t* buffer = use_double_buffer ? back : front;
    for(int i = 0; i < w; i++) {
        buffer[y * FB_WIDTH + (x + i)] = color;
    }
}

// ---- rectangle ----
void fb_rect(int x, int y, int w, int h, uint32_t color) {
    for(int j = 0; j < h; j++) {
        fb_hline(x, y + j, w, color);
    }
}

// ---- buffer swap ----
void fb_swap() {
    if(!use_double_buffer) return;
    memcpy(front, back, FB_SIZE * sizeof(uint32_t));
}

// ---- raw access (useful for scaling or Doom blit) ----
uint32_t* fb_frontbuffer() { return front; }
uint32_t* fb_backbuffer()  { return use_double_buffer ? back : front; }

void fb_draw_char(int x, int y, char c, uint32_t color) {
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

void fb_draw_string(int x, int y, const char* str, uint32_t color) {
    while (*str) {
        fb_draw_char(x, y, *str++, color);
        x += 8; // Move to the next character position
    }
}

void fb_draw_string_bg(int x, int y, const char *str, uint32_t fgcolor, uint32_t bgcolor) {
    while (*str) {
        // Draw background rectangle for character
        fb_rect(x, y, 8, 8, bgcolor);
        // Draw character on top
        fb_draw_char(x, y, *str++, fgcolor);
        x += 8; // Move to the next character position
    }
}

void fb_draw_hex(int x, int y, uint32_t value, uint32_t color) {
    const char* hex_chars = "0123456789ABCDEF";
    char hex_string[9]; // 8 digits + null terminator
    for (int i = 0; i < 8; i++) {
        hex_string[7 - i] = hex_chars[(value >> (i * 4)) & 0xF];
    }
    hex_string[8] = '\0';
    fb_draw_string(x, y, hex_string, color); // White color
}

uint8_t fb_palette[256 * 3];

void fb_draw_image() {
    // convert from X image format to framebuffer format
    // we cannot assume the X image format is 32bpp (because it isn't)
    // it uses a pallette index per byte, since we don't know the pallete (right now, TODO) we will just expand the bytes to 32bpp greyscale
    // this buffer is actually only 320x200, so we need to scale it up to framebuffer size (1024x768)
    // for now just double it, and deal with the rest later
    // for(int y = 0; y < X_height; y++) {
    //     for(int x = 0; x < X_width; x++) {
    //         uint8_t index = ((uint8_t*)image->data)[y * X_width + x];
    //         uint32_t color = (index << 16) | (index << 8) | index; // greyscale
    //         // scale to framebuffer size
    //         int fb_x = x * 2;
    //         int fb_y = y * 2;
    //         fb_putpixel(fb_x, fb_y, color);
    //         fb_putpixel(fb_x + 1, fb_y, color);
    //         fb_putpixel(fb_x, fb_y + 1, color);
    //         fb_putpixel(fb_x + 1, fb_y + 1, color);

    //     }
    // }

    // use palette
    for(int y = 0; y < X_height; y++) {
        for(int x = 0; x < X_width; x++) {
            uint8_t index = ((uint8_t*)image->data)[y * X_width + x];
            uint8_t r = fb_palette[index * 3 + 0];
            uint8_t g = fb_palette[index * 3 + 1];
            uint8_t b = fb_palette[index * 3 + 2];
            uint32_t color = (r << 16) | (g << 8) | b;
            // scale to framebuffer size
            int fb_x = x * 2;
            int fb_y = y * 2;
            fb_putpixel(fb_x, fb_y, color);
            fb_putpixel(fb_x + 1, fb_y, color);
            fb_putpixel(fb_x, fb_y + 1, color);
            fb_putpixel(fb_x + 1, fb_y + 1, color);
        }
    }

}

void fb_set_palette(uint8_t* palette) {
    memcpy(fb_palette, palette, 256 * 3);
}