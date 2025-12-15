#include "init.h"

#include "framebuffer.h"
#include "kbd.h"
#include "cpu/isr.h"
#include "cpu/idt.h"
#include "cpu/gdt.h"
#include "cpu/pic.h"
#include <string.h>
#include <stdio.h>

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


    // some random code
    int i = 0;
    int* p = &i;
    *p = 42;

    fb_clear(0x00111111);

    fb_swap();


    printf("Framebuffer initialized: %ux%u, %u bpp\n", fb_width, fb_height, fb_bpp);

    // char* args[2] = { "-wad", "doom.wad" };
    main(NULL, 0);

    // fb_clear(0x00111111); // Dark gray background
    // fb_draw_string(50, 50, "Welcome to DoomOS!", 0x00FFFFFF); // White text

    // fb_swap();

    // int px = fb_width / 2;
    // int py = fb_height / 2;

    // const int box_size = 50;

    // while (1) {
    //     kbd_update();

    //     kbd_event_t ev;
    //     while (kbd_pop_event(&ev)) {
    //         if (ev.pressed) switch (ev.key) {
    //             case KEY_W:
    //             case KEY_UP: py -= 2; break;
    //             case KEY_S:
    //             case KEY_DOWN: py += 2; break;
    //             case KEY_A:
    //             case KEY_LEFT: px -= 2; break;
    //             case KEY_D:
    //             case KEY_RIGHT: px += 2; break;

    //             default: break;
    //         }

    //         if (px < 0) px = 0;
    //         if (py < 0) py = 0;
    //         if (px >= (int)fb_width) px = fb_width - box_size;
    //         if (py >= (int)fb_height) py = fb_height - box_size;

    //         fb_clear(0x00111111); // Dark gray background
    //         fb_rect(px, py, box_size, box_size, 0x00FF0000); // Red box
    //         fb_swap();

    //     }
    // }

    
    // Halt the CPU indefinitely
    while(1) {
        asm("hlt");
    }
}