#pragma once
#include <stdint.h>
#include <stdbool.h>

// Initialize PIT channel 0 to the desired frequency
void pit_init();

// Read the current counter of PIT channel 0 (16-bit)
uint16_t pit_read_counter(void);

// Get the number of Doom tics since PIT initialization
int pit_get_tics(void);