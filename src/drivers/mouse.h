#pragma once
#include <stdbool.h>
#include <stdint.h>

#include "ps2.h"

// ---------- Mouse buttons ----------
typedef enum {
    MOUSE_BTN_LEFT   = 1 << 0,
    MOUSE_BTN_RIGHT  = 1 << 1,
    MOUSE_BTN_MIDDLE = 1 << 2,
} MouseButton;

// ---------- Doom-style mouse event ----------
typedef struct {
    int8_t dx;
    int8_t dy;
    uint8_t buttons;   // bitmask of MouseButton
} mouse_event_t;

void mouse_init();                    // call once during init
bool mouse_pop_event(mouse_event_t *e);
bool mouse_event_pending();
