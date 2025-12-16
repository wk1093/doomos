#include "init.h"

#include "framebuffer.h"
#include "kbd.h"
#include "mouse.h"
#include "pit.h"
#include "cpu/isr.h"
#include "cpu/idt.h"
#include "cpu/gdt.h"
#include "cpu/pic.h"
#include <string.h>
#include <stdio.h>

#include "doom/doomdef.h"

int main(char** argv, int argc); // Forward declaration of main function


static inline void cli(void) {
    asm volatile ("cli");
}

static inline void sti(void) {
    asm volatile ("sti");
}

// --- High-Level C Kernel Function ---
void kernel_main(uint32_t mb_magic, uint32_t mb_info_ptr) {
    const uint32_t MULTIBOOT2_BOOTLOADER_MAGIC = 0x36D76289; 
    
    // Safety check: Ensure we were loaded correctly
    if (mb_magic != MULTIBOOT2_BOOTLOADER_MAGIC) {
        return; // Triple-fault or halt if not loaded by MB2
    }

    // Start of the tags is 8 bytes into the structure (Total Size + Reserved)
    multiboot_tag_t *tag = (multiboot_tag_t *)(uintptr_t)(mb_info_ptr + 8);
    
    // Variables to store framebuffer properties
    uint64_t fb_addr = 0;
    uint32_t fb_width = 0;
    uint32_t fb_height = 0;
    uint8_t fb_bpp = 0;

    // --- 2. Locate the Framebuffer Tag (Type 8) ---
    while (tag->type != 0) { // Loop until End Tag (Type 0) is found
        
        if (tag->type == 8) { // Framebuffer Information Tag
            multiboot_tag_framebuffer_t *fb_tag = (multiboot_tag_framebuffer_t *)tag;
            
            fb_addr = fb_tag->fb_addr;
            fb_width = fb_tag->fb_width;
            fb_height = fb_tag->fb_height;
            fb_bpp = fb_tag->fb_bpp;
            
            // Found it, exit loop
            break; 
        }

        // --- Advance to the Next Tag ---
        uint32_t tag_size = tag->size;
        
        // Advance pointer and ensure 8-byte alignment (CRITICAL for MB2)
        // (A + 7) & ~7 is the standard way to align A up to the next multiple of 8.
        tag = (multiboot_tag_t *)((uint8_t *)tag + ((tag_size + 7) & ~7));
    }

    cli();

    gdt_install();

    idt_install();
    isr_install();

    pic_disable();

    sti();

    fb_init(FB_DOUBLE_BUFFER, (void*)(uintptr_t)fb_addr);


    fb_clear(0x00111111);

    fb_swap();

    pit_init();


    printf("Framebuffer initialized: %ux%u, %u bpp\n", fb_width, fb_height, fb_bpp);

    mouse_init();

    // // char* args[2] = { "-wad", "doom.wad" };
    main(NULL, 0);
    
    // Halt the CPU indefinitely
    while(1) {
        asm("hlt");
        // printf("pit tics: %d", pit_get_tics());
    }
}