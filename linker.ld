/* The address where the kernel will be loaded (1MB) */
ENTRY(_start)
LOAD_ADDR = 0x00100000;

SECTIONS
{
  /* Start at the base address 1MB */
  . = LOAD_ADDR;

  /* Multiboot header section */

  . = 0x00100000;

  /* Text section (code) */
  .text :
  {
    KEEP(*(.multiboot))
    *(.text)
  }
  .rodata : { *(.rodata*) }
  .data : { *(.data*) }

  /* BSS section (zero-initialized data) */
  .bss :
    {
        __bss_start = .; 
        *(COMMON)
        *(.bss*)
        . = ALIGN(4);     
        __bss_end = .;
    }

        /DISCARD/ : {
        *(.eh_frame)
        *(.note.gnu.build-id)
        *(.stab)
        *(.stabstr)
        *(.comment)
        *(.risc.attributes)
        *(.stack_metadata)
    }

    . = ALIGN(16);
    stack_bottom = .;
    . += 0x4000; /* 16KB stack */
    stack_top = .;
}