#include "kbd.h"

#include <stdio.h>

#define KBD_DATA   0x60
#define KBD_STATUS 0x64

// ---------- Event queue (Doom-style) ----------
#define KBD_EVENT_QUEUE_SIZE 64

static kbd_event_t event_queue[KBD_EVENT_QUEUE_SIZE];
static uint8_t queue_head = 0, queue_tail = 0;

static inline void queue_push(Key k, bool pressed) {
    uint8_t next = (queue_head + 1) % KBD_EVENT_QUEUE_SIZE;
    if(next == queue_tail) return;   // queue full, drop event
    event_queue[queue_head].key = k;
    event_queue[queue_head].pressed = pressed;
    queue_head = next;
}

bool kbd_pop_event(kbd_event_t *out) {
    if(queue_head == queue_tail) return false;
    *out = event_queue[queue_tail];
    queue_tail = (queue_tail + 1) % KBD_EVENT_QUEUE_SIZE;
    return true;
}

// ---------- Raw scancode IO ----------
static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    asm volatile("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

static inline bool kbd_available() {
    return inb(KBD_STATUS) & 1;
}

static inline uint8_t kbd_read_scancode() {
    while(!kbd_available());
    return inb(KBD_DATA);
}

static bool e0_prefix = false;

// ---------- Scancode → Key ----------
Key scancode_to_key(uint8_t sc, bool *released) {
    *released = sc & 0x80;
    sc &= 0x7F;

    switch(sc) {
        case 0x11: return KBD_W;
        case 0x1E: return KBD_A;
        case 0x1F: return KBD_S;
        case 0x20: return KBD_D;

        case 0x48: return KBD_UP;
        case 0x50: return KBD_DOWN;
        case 0x4B: return KBD_LEFT;
        case 0x4D: return KBD_RIGHT;

        case 0x39: return KBD_SPACE;
        case 0x01: return KBD_ESC;

        case 0x1C: return KBD_ENTER;
        case 0x0F: return KBD_TAB;

        case 0x38: return KBD_LALT;
        case 0xB8: return KBD_RALT;
        case 0x1D: return KBD_LCTRL;
        case 0x9D: return KBD_RCTRL;

        case 0x2A:
        case 0x36: return KBD_SHIFT;

        default: return KBD_NONE;
    }
}



void kbd_handle_byte(uint8_t sc) {
    if (sc == 0xE0) {
        e0_prefix = true;
        return;
    }

    bool released = sc & 0x80;

    Key k = KBD_NONE;

    if (e0_prefix) {
        sc &= 0x7F;
        // Extended keys
        switch (sc) {
            case 0x48: k = KBD_UP;    break;
            case 0x50: k = KBD_DOWN;  break;
            case 0x4B: k = KBD_LEFT;  break;
            case 0x4D: k = KBD_RIGHT; break;
        }
        e0_prefix = false;
    } else {
        // Normal keys
        k = scancode_to_key(sc, &released);
    }

    if (k != KBD_NONE) {
        queue_push(k, !released);
    }
}


bool kbd_event_pending() {
    return queue_head != queue_tail;
}