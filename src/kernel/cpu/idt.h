#pragma once
#include <stdint.h>

void idt_install(void);
void set_idt_gate(uint8_t n, uint32_t handler);
