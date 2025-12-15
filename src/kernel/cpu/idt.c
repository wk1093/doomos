#include "idt.h"

#include <string.h>

struct idt_entry {
    uint16_t base_lo;
    uint16_t sel;
    uint8_t  always0;
    uint8_t  flags;
    uint16_t base_hi;
} __attribute__((packed));

struct idt_ptr {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed));

static struct idt_entry idt[256];
static struct idt_ptr idtp;

extern void idt_load(uint32_t);

void set_idt_gate(uint8_t n, uint32_t handler)
{
    idt[n].base_lo = handler & 0xFFFF;
    idt[n].base_hi = (handler >> 16) & 0xFFFF;
    idt[n].sel     = 0x08;     // kernel code segment
    idt[n].always0 = 0;
    idt[n].flags   = 0x8E;     // present, ring0, interrupt gate
}

void idt_install(void)
{
    idtp.limit = sizeof(idt) - 1;
    idtp.base  = (uint32_t)&idt;

    memset(&idt, 0, sizeof(idt));

    idt_load((uint32_t)&idtp);
}
