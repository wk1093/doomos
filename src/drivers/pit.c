#include "pit.h"


#define PIT_CHANNEL0 0x40
#define PIT_COMMAND  0x43
#define PIT_FREQUENCY 1193182 // Hz of PIT
#define TICRATE 35            // Doom tics per second

static inline void outb(uint16_t port, uint8_t val) {
    asm volatile("outb %0, %1" :: "a"(val), "Nd"(port));
}

static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    asm volatile("inb %1, %0" : "=a"(ret) : "Nd"(port));
}

// Number of Doom tics since PIT init
static volatile int accumulated_tics = 0;

// Last counter value we observed
static uint16_t last_count = 0;

// PIT reload value
static uint16_t pit_divisor = 0;

// Initialize PIT channel 0 in mode 3, 35Hz
void pit_init() {
    pit_divisor = PIT_FREQUENCY / TICRATE; // 1193182 / 35 ≈ 34054

    // mode 3, lobyte/hibyte
    outb(PIT_COMMAND, 0x36);
    outb(PIT_CHANNEL0, pit_divisor & 0xFF); // low byte
    outb(PIT_CHANNEL0, pit_divisor >> 8);   // high byte

    last_count = pit_divisor; // initial value
    accumulated_tics = 0;
}

// Latch + read 16-bit channel 0 counter
uint16_t pit_read_counter(void) {
    uint8_t lo, hi;
    outb(PIT_COMMAND, 0x00); // latch
    lo = inb(PIT_CHANNEL0);
    hi = inb(PIT_CHANNEL0);
    return (hi << 8) | lo;
}

// Returns Doom tics since PIT init
static uint16_t leftover = 0;

int pit_get_tics(void) {
    uint16_t current = pit_read_counter();
    uint16_t delta;

    if (last_count >= current)
        delta = last_count - current;
    else
        delta = last_count + (pit_divisor - current);

    delta += leftover;
    accumulated_tics += delta / pit_divisor;
    leftover = delta % pit_divisor;

    last_count = current;
    return accumulated_tics;
}