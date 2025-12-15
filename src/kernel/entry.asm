; entry.asm — Multiboot2 + kernel entry
; NASM syntax, 32-bit protected mode

[BITS 32]

; -----------------------------------------------------------------------------
; Multiboot2 header
; Must be within first 8 KiB of the file
; Must be 8-byte aligned
; -----------------------------------------------------------------------------

section .multiboot
align 8

MB2_MAGIC        equ 0xE85250D6
MB2_ARCH_I386    equ 0
MB2_HEADER_LEN   equ header_end - header_start
MB2_CHECKSUM     equ -(MB2_MAGIC + MB2_ARCH_I386 + MB2_HEADER_LEN)

header_start:
    dd MB2_MAGIC
    dd MB2_ARCH_I386
    dd MB2_HEADER_LEN
    dd MB2_CHECKSUM

    ; ---------------------------------------------------------
    ; Framebuffer request tag (optional but common)
    ; ---------------------------------------------------------
    dw 5              ; tag type: framebuffer
    dw 0              ; flags
    dd 20             ; size
    dd 1024           ; width
    dd 768            ; height
    dd 32             ; depth

    ; padding to 8-byte alignment
    dd 0

    ; ---------------------------------------------------------
    ; End tag
    ; ---------------------------------------------------------
    dw 0
    dw 0
    dd 8

header_end:

; -----------------------------------------------------------------------------
; Code section
; -----------------------------------------------------------------------------

section .text
align 16
global _start
extern kernel_main
extern stack_top

_start:
    ; GRUB guarantees:
    ;   - 32-bit protected mode
    ;   - Paging disabled
    ;   - A20 enabled
    ;   - Flat GDT
    ;   - Interrupts disabled
    ;   - eax = multiboot magic
    ;   - ebx = multiboot info pointer

    ; Set up stack
    mov esp, stack_top

    ; Align stack to 16 bytes (SysV ABI)
    and esp, 0xFFFFFFF0

    ; Push args for kernel_main(magic, info)
    push ebx
    push eax

    call kernel_main

.hang:
    cli
    hlt
    jmp .hang
