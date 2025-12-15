#include <stdint.h>

#define PIC1_COMMAND 0x20
#define PIC1_DATA 0x21
#define PIC2_COMMAND 0xA0
#define PIC2_DATA 0xA1

inline static void outb(uint16_t port, uint8_t value) {
    asm volatile ("outb %0, %1" : : "a"(value), "Nd"(port));
}

void pic_disable()
{
    // 1. Send all 0xFF to the data ports to mask all IRQs
    outb(PIC1_DATA, 0xFF);
    outb(PIC2_DATA, 0xFF);
    
    // 2. Optionally, send an End of Interrupt (EOI) command
    // to clear any pending interrupts that may have already fired.
    outb(PIC1_COMMAND, 0x20); 
    outb(PIC2_COMMAND, 0x20); 
}