#pragma once
#include <stdint.h>

#define PS2_DATA_PORT   0x60
#define PS2_STATUS_PORT 0x64

// Keyboard Scancodes (Set 1) for WASD and release (F0)
#define SCANCODE_W      0x11
#define SCANCODE_A      0x1E
#define SCANCODE_S      0x1F
#define SCANCODE_D      0x20
#define SCANCODE_RELEASE 0xF0


// --- INLINE ASSEMBLY I/O FUNCTIONS ---

// Read a byte from an I/O port
static inline uint8_t inb(uint16_t port) {
    uint8_t data;
    // a = AL, d = DX (required by 'in' instruction)
    // The instruction 'in' reads from the port stored in DX and puts the result in AL.
    // Constraints: "a" (AL) is both input/output, "d" (DX) is input.
    asm volatile("inb %w1, %0" : "=a" (data) : "Nd" (port));
    return data;
}

// Write a byte to an I/O port
static inline void outb(uint16_t port, uint8_t data) {
    // a = AL, d = DX (required by 'out' instruction)
    // The instruction 'out' writes the byte in AL to the port stored in DX.
    // Constraints: "a" (AL) is input, "d" (DX) is input.
    asm volatile("outb %0, %w1" : : "a" (data), "Nd" (port));
}