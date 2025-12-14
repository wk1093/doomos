/* The address where the kernel will be loaded (1MB) */
ENTRY(kernel_main)
LOAD_ADDR = 0x00100000;

SECTIONS
{
  /* Start at the base address 1MB */
  . = LOAD_ADDR;

  /* Multiboot header section */
  .multiboot ALIGN(8) :
  {
    *(.multiboot)
  }

  . = 0x00101000;

  /* Text section (code) */
  .text :
  {
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
}