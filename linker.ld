/* --- File: linker.ld --- */
OUTPUT_FORMAT("elf32-i386")
ENTRY(start)

SECTIONS {
    . = 0x100000; /* Load at 1 Megabyte */

    /* Multiboot header must be first */
    .multiboot : {
        *(.multiboot)
    }

    /* Code section */
    .text ALIGN(4K) : {
        *(.text)
    }

    /* Read-only data */
    .rodata ALIGN(4K) : {
        *(.rodata*)
    }
    
    /* Initialized data */
    .data ALIGN(4K) : {
        *(.data)
    }

    /* Uninitialized data */
    .bss ALIGN(4K) : {
        *(.bss)
        *(COMMON)
    }
    
    /* Mark the end of the kernel */
    _kernel_end = .;
}