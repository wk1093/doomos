#pragma once
#include <stdbool.h>
#include <stdint.h>

typedef enum {
    KEY_NONE = 0,
    KEY_W, KEY_A, KEY_S, KEY_D,
    KEY_UP, KEY_DOWN, KEY_LEFT, KEY_RIGHT,
    KEY_SPACE, KEY_ESC,
    KEY_COUNT
} Key;

typedef struct {
    Key key;
    bool pressed;   // true = keydown, false = keyup
} kbd_event_t;

void kbd_update();                 // call frequently (or in IRQ1)
bool kbd_pop_event(kbd_event_t *e); // fetch next press/release event
bool kbd_event_pending();
