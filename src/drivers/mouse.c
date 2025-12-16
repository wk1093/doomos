#include "mouse.h"

// PS/2 controller ports (same as keyboard)
#define PS2_DATA    0x60
#define PS2_STATUS  0x64
#define PS2_CMD     0x64

// ---------- Event queue (Doom-style) ----------
#define MOUSE_EVENT_QUEUE_SIZE 64

static mouse_event_t event_queue[MOUSE_EVENT_QUEUE_SIZE];
static uint8_t queue_head = 0, queue_tail = 0;

static inline void queue_push(mouse_event_t ev) {
    uint8_t next = (queue_head + 1) % MOUSE_EVENT_QUEUE_SIZE;
    if (next == queue_tail) return; // queue full, drop event
    event_queue[queue_head] = ev;
    queue_head = next;
}

bool mouse_pop_event(mouse_event_t *out) {
    if (queue_head == queue_tail) return false;
    *out = event_queue[queue_tail];
    queue_tail = (queue_tail + 1) % MOUSE_EVENT_QUEUE_SIZE;
    return true;
}

bool mouse_event_pending() {
    return queue_head != queue_tail;
}

// ---------- Raw IO ----------
static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    asm volatile("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

static inline void outb(uint16_t port, uint8_t val) {
    asm volatile("outb %0, %1" :: "a"(val), "Nd"(port));
}

static inline bool ps2_can_read() {
    return inb(PS2_STATUS) & 1;
}

static inline bool ps2_is_mouse() {
    return inb(PS2_STATUS) & (1 << 5);
}

static inline uint8_t ps2_read() {
    while (!ps2_can_read());
    return inb(PS2_DATA);
}

static inline void ps2_wait_write() {
    while (inb(PS2_STATUS) & 2);
}

// ---------- Mouse init ----------
static inline void mouse_write(uint8_t val) {
    ps2_wait_write();
    outb(PS2_CMD, 0xD4);   // next byte goes to mouse
    ps2_wait_write();
    outb(PS2_DATA, val);
}

void mouse_init() {
    // enable auxiliary device
    ps2_wait_write();
    outb(PS2_CMD, 0xA8);

    mouse_write(0xF6);    // set defaults
    ps2_read();           // ACK

    mouse_write(0xF4);    // enable streaming
    ps2_read();           // ACK
}

// ---------- Packet decode ----------
static uint8_t packet[3];
static uint8_t packet_index = 0;

// ---------- Process + push Doom-style events ----------
void mouse_handle_byte(uint8_t data) {
    if (packet_index == 0 && !(data & 0x08))
        return;

    packet[packet_index++] = data;

    if (packet_index < 3)
        return;

    packet_index = 0;

    mouse_event_t ev;
    ev.buttons = packet[0] & 0x07;
    ev.dx = (int8_t)packet[1];
    ev.dy = -(int8_t)packet[2];

    queue_push(ev);
}
