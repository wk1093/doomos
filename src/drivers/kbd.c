#include "kbd.h"

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

// ---------- Scancode → Key ----------
Key scancode_to_key(uint8_t sc, bool *released) {
    *released = sc & 0x80;
    sc &= 0x7F;

    switch(sc) {
        case 0x11: return KEY_W;
        case 0x1E: return KEY_A;
        case 0x1F: return KEY_S;
        case 0x20: return KEY_D;

        case 0x48: return KEY_UP;
        case 0x50: return KEY_DOWN;
        case 0x4B: return KEY_LEFT;
        case 0x4D: return KEY_RIGHT;

        case 0x39: return KEY_SPACE;
        case 0x01: return KEY_ESC;

        default: return KEY_NONE;
    }
}

// ---------- Process + push Doom-style events ----------
void kbd_update() {
    if(!kbd_available()) return;

    uint8_t sc = kbd_read_scancode();
    bool released;
    Key k = scancode_to_key(sc, &released);

    if(k != KEY_NONE) {
        queue_push(k, !released); // true = keydown, false = keyup
    }
}

bool kbd_event_pending() {
    return queue_head != queue_tail;
}