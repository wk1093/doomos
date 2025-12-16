#include "ps2.h"

#define PS2_DATA    0x60
#define PS2_STATUS  0x64

void kbd_handle_byte(uint8_t sc);
void mouse_handle_byte(uint8_t data);

static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    asm volatile("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

void ps2_update() {
    while (inb(PS2_STATUS) & 1) {
        uint8_t status = inb(PS2_STATUS);
        uint8_t data   = inb(PS2_DATA);

        if (status & (1 << 5)) {
            mouse_handle_byte(data);
        } else {
            kbd_handle_byte(data);
        }
    }
}