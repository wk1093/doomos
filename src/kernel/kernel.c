#include "init.h"


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

    // --- 3. Draw Pixels ---
    if (fb_addr != 0 && fb_width > 0 && fb_bpp == 32) {
        
        // Cast the 64-bit physical address to a volatile 32-bit pointer.
        // We assume 32 BPP, so one pixel is a uint32_t (0x00RRGGBB).
        // Since the kernel is loaded in protected mode, we can typically 
        // access physical memory directly, but we use uintptr_t for safe conversion.
        volatile uint32_t *pixel_addr = (volatile uint32_t*)(uintptr_t)fb_addr;

        // Set the color for demonstration (pure Red: 0x00FF0000)
        const uint32_t RED = 0x00FF0000;
        
        // Draw a small red square in the top-left corner
        for (uint32_t y = 0; y < 100; y++) {
            for (uint32_t x = 0; x < 100; x++) {
                // Calculate the pixel index: (y * width) + x
                pixel_addr[(y * fb_width) + x] = RED;
            }
        }
    }
    
    // Halt the CPU indefinitely
    while(1) {
        asm("hlt");
    }
}