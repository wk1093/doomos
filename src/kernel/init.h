#pragma once
#include <stdint.h>

// --- New Corrected Size Definitions ---
// 16 (Mandatory) + 16 (Address Tag) + 20 (FB Type 5) + 4 (Padding) + 8 (End Tag) = 64
#define MULTIBOOT2_HEADER_LENGTH    64

// --- Constants (same as before) ---
#define MULTIBOOT2_MAGIC            0xE85250D6
#define MULTIBOOT2_ARCHITECTURE_I386 0x00000000
#define KERNEL_LOAD_ADDR            0x00101000 // Must match linker.ld

// Calculate the checksum
#define MULTIBOOT2_CHECKSUM         (-(MULTIBOOT2_MAGIC + MULTIBOOT2_ARCHITECTURE_I386 + MULTIBOOT2_HEADER_LENGTH))


__attribute__((section(".multiboot")))
__attribute__((used))
__attribute__((aligned(8)))
const uint32_t multiboot2_header[] = {
    // 1. Mandatory Fields (16 bytes)
    MULTIBOOT2_MAGIC,
    MULTIBOOT2_ARCHITECTURE_I386,
    MULTIBOOT2_HEADER_LENGTH,  // 64
    MULTIBOOT2_CHECKSUM,

    // 2. ADDRESS TAG (Type 3) - Explicitly sets the kernel addresses (16 bytes)
    0x00000003,      // Type: Address Tag
    0x00000010,      // Size: 16 bytes
    KERNEL_LOAD_ADDR, // load_addr
    0,               // load_end_addr (Set to 0, GRUB figures it out)

    // 3. CORRECT FRAMEBUFFER HEADER TAG (Type 5) (20 bytes, followed by 4 bytes padding)
    0x00000005,      // Type: Framebuffer Request
    0x00000014,      // Size: 20 bytes
    1024,            // Width (Desired)
    768,             // Height (Desired)
    32,              // Depth (Desired BPP)

    // 4. PADDING - MUST be added by the programmer to align the next tag
    // Since the FB tag is 20 bytes, we need 4 bytes of padding (0) to reach 24 (8-byte alignment)
    0x00000000, 

    // 5. End Tag (Type 0) (8 bytes)
    0x00000000,      // Type: End Tag
    0x00000008       // Size: 8 bytes
};


#define KERNEL_STACK_SIZE 0x4000 // 16 KB

// Use attribute to place the stack in the .bss section
__attribute__((section(".bss")))
static uint8_t stack_area[KERNEL_STACK_SIZE];

// Global variable to mark the top of the stack
// The linker will automatically place this after stack_area
static uint32_t *stack_top = (uint32_t*) (stack_area + KERNEL_STACK_SIZE);


// --- Kernel Entry Point (The Inline Assembly Block) ---

// Define the function prototype matching the arguments GRUB provides
void kernel_main(uint32_t mb_magic, uint32_t mb_info_ptr);

// Use a compiler trick: Define the actual linker entry point (_start) 
// using assembly, which calls kernel_main.
// This function will never return.
void _start(void) __attribute__((naked));
void _start(void)
{
    // The 'naked' attribute tells the compiler *not* to generate a C function
    // prologue/epilogue (like pushing/popping EBP), giving us full control.
    
    // Inline Assembly Block
    asm volatile(
        // 1. Set up the stack pointer (ESP) to point to the top of our stack space.
        // The address of stack_top is computed by the linker.
        "movl %0, %%esp \n" 
        
        // 2. Multiboot 2 Contract: GRUB has already loaded EAX (Magic) and EBX (Info Pointer).
        // Push the arguments onto the stack in reverse order (right-to-left for C call).
        "pushl %%ebx \n" // Push mb_info_ptr
        "pushl %%eax \n" // Push mb_magic
        
        // 3. Call the C function.
        "call kernel_main \n"
        
        // 4. If kernel_main returns (it shouldn't), halt the CPU.
        "cli \n"
        "hlt \n"
        
        // Output Operands (tells the assembler where to get values)
        // %0 gets the address of stack_top. The 'g' constraint means 'any register or memory location'.
        : /* no output registers */
        : "g" (stack_top)
        : "eax", "ebx", "memory" // Clobber list: registers used/modified
    );
}

typedef struct {
    uint32_t type;
    uint32_t size;
} multiboot_tag_t;

// Framebuffer Information Tag (Type 8) Structure
typedef struct {
    uint32_t type; // Should be 8
    uint32_t size;
    uint64_t fb_addr;   // The physical address of the framebuffer
    uint32_t fb_pitch;  // Bytes per horizontal line
    uint32_t fb_width;
    uint32_t fb_height;
    uint8_t fb_bpp;     // Bits per pixel
    uint8_t fb_type;    // 1 for RGB color
    uint8_t reserved;
    // ... color info follows (ignored for simple access)
} __attribute__((packed)) multiboot_tag_framebuffer_t;