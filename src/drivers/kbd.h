#pragma once
#include <stdbool.h>
#include <stdint.h>

typedef enum {
    KBD_NONE = 0,
    KBD_W, KBD_A, KBD_S, KBD_D,
    KBD_UP, KBD_DOWN, KBD_LEFT, KBD_RIGHT,
    KBD_SPACE, KBD_ESC,
    KBD_COUNT
} Key;

typedef struct {
    Key key;
    bool pressed;   // true = keydown, false = keyup
} kbd_event_t;

void kbd_update();                 // call frequently (or in IRQ1)
bool kbd_pop_event(kbd_event_t *e); // fetch next press/release event
bool kbd_event_pending();
