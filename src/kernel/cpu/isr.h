#pragma once
#include <stdint.h>

typedef struct registers
{
   uint32_t ds;                  // Pushed by isr_common: 'push eax' (old DS)
   uint32_t edi, esi, ebp, esp_dummy, ebx, edx, ecx, eax; // Pushed by 'pusha'
   uint32_t int_no;              // Pushed by macro: 'push %1'
   uint32_t err_code;            // Pushed by macro: 'push 0' or 'push [Error Code]'
   
   // --- These next fields are the CPU-pushed stack frame ---
   uint32_t eip;                 // CRITICAL: EIP of the faulting instruction
   uint32_t cs;
   uint32_t eflags;
   uint32_t useresp;             // Pushed only for privilege level change (not relevant here)
   uint32_t ss;                  // Pushed only for privilege level change (not relevant here)
} registers_t;

void isr_handler(registers_t *r);

void isr_install(void);