; --- File: boot.asm ---
section .multiboot
align 4
    ; GRUB magic numbers
    MULTIBOOT_HEADER_MAGIC      equ 0x1BADB002
    MULTIBOOT_HEADER_FLAGS      equ 0x00000003
    MULTIBOOT_CHECKSUM          equ -(MULTIBOOT_HEADER_MAGIC + MULTIBOOT_HEADER_FLAGS)

    dd MULTIBOOT_HEADER_MAGIC
    dd MULTIBOOT_HEADER_FLAGS
    dd MULTIBOOT_CHECKSUM

section .bss
align 16
    ; Reserve space for stack
    stack_bottom:
        resb 16384   ; 16KB stack
    stack_top:

section .text
global start

start:
    ; Disable interrupts and keep them disabled
    cli
    
    ; Set up the stack pointer
    mov esp, stack_top
    
    ; Clear direction flag
    cld
    
    ; Call our kernel entry point
    extern kernel_main
    call kernel_main
    
    ; If kernel_main returns, hang forever
.hang:
    cli
    hlt
    jmp .hang