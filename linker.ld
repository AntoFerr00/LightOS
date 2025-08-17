/* --- File: linker.ld --- */
/* This script tells the linker how to lay out our kernel in memory. */

OUTPUT_FORMAT(elf32-i386)
ENTRY(main) /* Set the entry point of our kernel to the 'main' function */
SECTIONS
{
    . = 0x100000; /* The kernel will be loaded at 1MB */

    .text :
    {
        *(.text)
    }

    .rodata :
    {
        *(.rodata)
    }

    .data :
    {
        *(.data)
    }

    .bss :
    {
        *(.bss)
    }
}
